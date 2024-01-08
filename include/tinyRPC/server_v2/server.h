//
// Created by just do it on 2023/11/30.
//

#ifndef TINYRPC_SERVER_H
#define TINYRPC_SERVER_H
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace google::protobuf { class Service; }

namespace tinyRPC {

    class Session;
    class Codec;
    class Router;
    class AbstractHttpApiGateway;

    using ServicePtr = std::shared_ptr<google::protobuf::Service>;

    class Server {
    public:
        Server(const std::string& addr, uint16_t port);

        explicit Server(uint16_t port);

        void SetWorkerNum(int num);

        void SetGateway(AbstractHttpApiGateway* gw);

        void RegisterService(const ServicePtr& service, bool exec_in_pool = false);

        void AddSession(const std::shared_ptr<Session>& session);

        void Serve();

        ~Server();

    private:
        friend class Session;
        void RemoveSession(const std::string& session_id);

        class Impl;
        std::unique_ptr<Impl> pimpl_;
        AbstractHttpApiGateway* gw_{nullptr};
    };

}

#endif //TINYRPC_SERVER_H
