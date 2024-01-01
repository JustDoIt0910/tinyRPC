//
// Created by just do it on 2024/1/1.
//
#include "tinyRPC/server_v2/server.h"
#include "tinyRPC/server_v2//session.h"
#include "tinyRPC/server_v2/gw.h"
#include "tinyRPC/codec/protobuf_codec.h"
#include "tinyRPC/router/protobuf_router.h"
#include <unordered_map>

namespace tinyRPC {

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

        io_service& GetContext()
        {
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
        Impl(const ip::tcp::endpoint& ep, Server* server):
                acceptor_(ioc_),
                server_(server) {
            router_ = std::make_unique<ProtobufRpcRouter>();
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
        }

        void RegisterService(ServicePtr service) { router_->RegisterService(service); }

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
            io_context& ctx = io_pool_ ? io_pool_->GetContext(): ioc_;
            std::unique_ptr<Codec> codec = std::make_unique<ProtobufRpcCodec>();
            session_ptr session = std::make_shared<Session>(server_, ctx, codec, router_.get());
            acceptor_.async_accept(session->Socket(), [this, session] (std::error_code ec) {
                AddSession(session);
                StartAccept();
            });
        }

        void AddSession(std::shared_ptr<Session> session) {
            session->Start();
            std::lock_guard lg(sessions_mu_);
            sessions_[session->Id()] = session;
        }

        io_context& GetMainContext() { return ioc_; };

        ~Impl() { io_pool_->Stop(); }
    private:
        using session_ptr = std::shared_ptr<Session>;

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
        std::unique_ptr<Router> router_;
        std::mutex sessions_mu_;
        std::unordered_map<std::string, session_ptr> sessions_;
        std::vector<std::thread> workers_;
        std::unique_ptr<IOThreadPool> io_pool_;
    };

    Server::Server(const std::string &address, uint16_t port) {
        ip::address addr = ip::make_address(address);
        ip::tcp::endpoint ep(addr, port);
        pimpl_ = std::make_unique<Impl>(ep, this);
    }

    Server::Server(uint16_t port) {
        ip::tcp::endpoint ep(ip::tcp::v4(), port);
        pimpl_ = std::make_unique<Impl>(ep, this);
    }

    void Server::RegisterService(ServicePtr service) {
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

    void Server::AddSession(std::shared_ptr<Session> session) { pimpl_->AddSession(session); }

    Server::~Server() = default;

}