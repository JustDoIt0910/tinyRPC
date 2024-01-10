//
// Created by just do it on 2024/1/1.
//
#include "tinyRPC/server_v2/server.h"
#include "tinyRPC/server_v2/session.h"
#include "tinyRPC/server_v2/gw.h"
#include "tinyRPC/exec/executor.h"
#include "tinyRPC/codec/protobuf_codec.h"
#include "tinyRPC/codec/http_codec.h"
#include "tinyRPC/router/protobuf_router.h"
#include "tinyRPC/registry/registry.h"
#include "google/protobuf/descriptor.h"
#include "yaml-cpp/yaml.h"
#include <unordered_map>

using namespace std::chrono_literals;

namespace tinyRPC {

    constexpr static char DefaultIP[] = "0.0.0.0";

    constexpr static uint16_t DefaultPort = 9999;

    constexpr static int DefaultMaxQueueSize = 500;

    constexpr static int DefaultMaxPoolSize = 200;

    constexpr static int DefaultCorePoolSize = 50;

    constexpr static int DefaultMaxIdle = 30;

    constexpr static ThreadPoolExecutor::RejectPolicy DefaultRejectPolicy =
            ThreadPoolExecutor::RejectPolicy::ABORT;

    constexpr static int DefaultRegistryTTL = 10;

    class IOThreadPool {
    public:
        explicit IOThreadPool(std::vector<std::thread>& threads):
        threads_(threads), next(0) {
            for (int i = 0; i < threads.size(); i++) {
                std::shared_ptr<io_context> ctx = std::make_shared<io_context>();
                std::shared_ptr<io_context::work> work = std::make_shared<io_context::work>(*ctx);
                io_contexts_.push_back(ctx);
                works_.push_back(work);
            }
        }
        void Run() {
            for(int i = 0; i < threads_.size(); i++) {
                std::thread& th = threads_[i];
                th = std::thread([this, i] () { io_contexts_[i]->run(); });
            }
        }

        void Stop() {
            for(auto& ctx: io_contexts_) { ctx->stop(); }
            for(std::thread& th: threads_) { th.join(); }
        }

        io_service& GetContext() {
            io_context& ctx = *io_contexts_[next++];
            if (next == io_contexts_.size() - 1) { next = 0; }
            return ctx;
        }

    private:
        std::vector<std::thread>& threads_;
        std::vector<std::shared_ptr<io_context>> io_contexts_;
        std::vector<std::shared_ptr<io_context::work>> works_;
        int next;
    };

    class Server::Impl {
    public:
        Impl(const std::string& config_file, Server* server):
        acceptor_(ioc_),
        server_(server),
        config_(YAML::LoadFile(config_file)) {
            router_ = std::make_unique<ProtobufRpcRouter>();
            std::string ip = DefaultIP;
            uint16_t port = DefaultPort;
            if(config_["server"]) {
                YAML::Node server_conf = config_["server"];
                if(server_conf["ip"]) { ip = server_conf["ip"].as<std::string>(); }
                if(server_conf["port"]) { port = server_conf["port"].as<uint16_t>(); }
                if(server_conf["io-thread-num"]) { SetThreadNum(server_conf["io-thread-num"].as<int>()); }
            }
            int max_queue_size = DefaultMaxQueueSize;
            int max_pool_size = DefaultMaxPoolSize;
            int core_pool_size = DefaultCorePoolSize;
            int max_idle_seconds = DefaultMaxIdle;
            ThreadPoolExecutor::RejectPolicy reject = DefaultRejectPolicy;
            if(config_["executor"]) {
                YAML::Node exec_conf = config_["executor"];
                if(exec_conf["max-queue-size"]) { max_queue_size = exec_conf["max-queue-size"].as<int>(); }
                if(exec_conf["core-pool-size"]) { core_pool_size = exec_conf["core-pool-size"].as<int>(); }
                if(exec_conf["max-pool-size"]) { max_pool_size = exec_conf["max-pool-size"].as<int>(); }
                if(exec_conf["max-idle-time"]) { max_idle_seconds = exec_conf["max-idle-time"].as<int>(); }
                if(exec_conf["reject-policy"]) {
                    reject = ThreadPoolExecutor::Policy(exec_conf["reject-policy"].as<std::string>());
                }
            }
            executor_ = std::make_unique<ThreadPoolExecutor>(max_queue_size, core_pool_size,
                                                             max_pool_size, max_idle_seconds * 1000ms,
                                                             reject);
            ip::address addr = ip::make_address(ip);
            ip::tcp::endpoint ep(addr, port);
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
            if(config_["registry"]) {
                YAML::Node registry_conf = config_["registry"];
                if(registry_conf["endpoints"]) {
                    YAML::Node endpoints_conf = registry_conf["endpoints"];
                    std::vector<std::string> etcd_endpoints;
                    for(YAML::const_iterator it = endpoints_conf.begin(); it != endpoints_conf.end(); it++) {
                        etcd_endpoints.push_back(it->as<std::string>());
                    }
                    int ttl = registry_conf["ttl"] ? registry_conf["ttl"].as<int>() : DefaultRegistryTTL;
                    registry_ = std::make_unique<Registry>(std::move(etcd_endpoints), ep, ttl);
                }
            }
        }

        void RegisterService(const ServicePtr& service, bool exec_in_pool) {
            router_->RegisterService(service, exec_in_pool);
            if(registry_) {
                std::cout << "register service " << service->GetDescriptor()->name() << std::endl;
                registry_->Register(service->GetDescriptor()->name());
            }
        }

        void RemoveSession(const std::string& session_id) {
            std::shared_ptr<Session> session;
            {
                std::lock_guard<std::mutex> lg(sessions_mu_);
                auto it = sessions_.find(session_id);
                if(it != sessions_.end()) {
                    session = it->second;
                    sessions_.erase(it);
                }
            }
        }

        void SetThreadNum(int num) { workers_.resize(num); }

        void Run() {
            if(!workers_.empty()) {
                io_pool_ = std::make_unique<IOThreadPool>(workers_);
                io_pool_->Run();
            }
            ioc_.run();
        }

        void StartAccept() {
            io_context& ctx = GetIOContext();
            std::unique_ptr<Codec> codec = std::make_unique<ProtobufRpcCodec>();
            session_ptr session = std::make_shared<Session>(server_, ctx, codec, router_.get(), executor_.get());
            acceptor_.async_accept(session->Socket(), [this, session] (std::error_code ec) {
                AddSession(session);
                StartAccept();
            });
        }

        void AddSession(const std::shared_ptr<Session>& session) {
            session->Start();
            std::lock_guard lg(sessions_mu_);
            sessions_[session->Id()] = session;
        }

        io_context& GetMainContext() { return ioc_; };

        io_context& GetIOContext() { return io_pool_ ? io_pool_->GetContext(): ioc_; }

        ThreadPoolExecutor* GetExecutor() { return executor_.get(); }

        ~Impl() { io_pool_->Stop(); }
    private:
        using session_ptr = std::shared_ptr<Session>;

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
        std::unique_ptr<Router> router_;
        std::unique_ptr<ThreadPoolExecutor> executor_;
        std::mutex sessions_mu_;
        std::unordered_map<std::string, session_ptr> sessions_;
        std::vector<std::thread> workers_;
        std::unique_ptr<IOThreadPool> io_pool_;
        std::unique_ptr<Registry> registry_;
        YAML::Node config_;
    };

    Server::Server(const std::string& config_file) {
        pimpl_ = std::make_unique<Impl>(config_file, this);
    }

    void Server::RegisterService(const ServicePtr& service, bool exec_in_pool) {
        pimpl_->RegisterService(service, exec_in_pool);
        if(gw_) { gw_->RegisterService(service, exec_in_pool); }
    }

    void Server::SetWorkerNum(int num) { pimpl_->SetThreadNum(num); }

    void Server::SetGateway(AbstractHttpApiGateway *gw) {
        gw->Init(pimpl_->GetMainContext());
        gw_ = gw;
    }

    void Server::RemoveSession(const std::string &session_id) {
        pimpl_->RemoveSession(session_id);
    }

    void Server::Serve() {
        pimpl_->StartAccept();
        pimpl_->Run();
    }

    void Server::AddSession(const std::shared_ptr<Session>& session) { pimpl_->AddSession(session); }

    std::shared_ptr<Session> Server::GetHttpSession(Router* router) {
        io_context& ctx = pimpl_->GetIOContext();
        std::unique_ptr<Codec> codec = std::make_unique<HttpRpcCodec>();
        return std::make_shared<Session>(this, ctx, codec, router, pimpl_->GetExecutor());
    }

    Server::~Server() = default;

}