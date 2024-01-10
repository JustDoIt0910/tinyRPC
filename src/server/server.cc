//
// Created by just do it on 2023/11/30.
//
#include "tinyRPC/server/server.h"
#include "tinyRPC/server/session.h"
#include "tinyRPC/server/gw.h"
#include "tinyRPC/codec/protobuf_codec.h"
#include "tinyRPC/router/protobuf_router.h"
#include "tinyRPC/registry/registry.h"
#include "google/protobuf/descriptor.h"
#include "yaml-cpp/yaml.h"
#include <unordered_map>

using namespace asio;

namespace tinyRPC {

    constexpr static char DefaultIP[] = "0.0.0.0";

    constexpr static uint16_t DefaultPort = 9999;

    constexpr static int DefaultRegistryTTL = 10;

    class Server::Impl {
    public:
        Impl(const std::string& config_file, Server* server):
        acceptor_(ioc_),
        server_(server),
        config_(YAML::LoadFile(config_file)) {
            router_ = std::make_unique<ProtobufRpcRouter>();
            std::string ip = DefaultIP;
            uint16_t port = DefaultPort;
            if (config_["server"]) {
                YAML::Node server_conf = config_["server"];
                if (server_conf["ip"]) { ip = server_conf["ip"].as<std::string>(); }
                if (server_conf["port"]) { port = server_conf["port"].as<uint16_t>(); }
                if (server_conf["io-thread-num"]) { SetThreadNum(server_conf["io-thread-num"].as<int>()); }
            }
            ip::address addr = ip::make_address(ip);
            ip::tcp::endpoint ep(addr, port);
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
            if (config_["registry"]) {
                YAML::Node registry_conf = config_["registry"];
                if (registry_conf["endpoints"]) {
                    YAML::Node endpoints_conf = registry_conf["endpoints"];
                    std::vector<std::string> etcd_endpoints;
                    for (YAML::const_iterator it = endpoints_conf.begin(); it != endpoints_conf.end(); it++) {
                        etcd_endpoints.push_back(it->as<std::string>());
                    }
                    int ttl = registry_conf["ttl"] ? registry_conf["ttl"].as<int>() : DefaultRegistryTTL;
                    registry_ = std::make_unique<Registry>(std::move(etcd_endpoints), ep, ttl);
                }
            }
        }

        void RegisterService(const ServicePtr& service) {
            router_->RegisterService(service, false);
            if(registry_) {
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

        void StartAccept() {
            acceptor_.async_accept([this] (std::error_code ec, ip::tcp::socket socket) {
                std::unique_ptr<Codec> codec = std::make_unique<ProtobufRpcCodec>();
                SessionPtr session = std::make_shared<Session>(server_, ioc_, socket, codec, router_.get());
                AddSession(session);
                StartAccept();
            });
        }

        void SetThreadNum(int num) { workers_.resize(num); }

        void Run() {
            if(workers_.empty()) { ioc_.run(); }
            else {
                for(auto& worker : workers_) {
                    worker = std::thread([this] () { ioc_.run(); });
                }
                for(auto& worker : workers_) { worker.join(); }
            }
        }

        void AddSession(const std::shared_ptr<Session>& session) {
            session->Start();
            std::lock_guard lg(sessions_mu_);
            sessions_[session->Id()] = session;
        }

        io_context& GetMainContext() { return ioc_; };

    private:
        using SessionPtr = std::shared_ptr<Session>;

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
        std::unique_ptr<Router> router_;
        std::mutex sessions_mu_;
        std::unordered_map<std::string, SessionPtr> sessions_;
        std::vector<std::thread> workers_;
        std::unique_ptr<Registry> registry_;
        YAML::Node config_;
    };

    Server::Server(const std::string& config_file) {
        pimpl_ = std::make_unique<Impl>(config_file, this);
    }

    void Server::RegisterService(const ServicePtr& service, bool exec_in_pool) {
        pimpl_->RegisterService(service);
        if(gw_) { gw_->RegisterService(service); }
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

    Server::~Server() = default;

}
