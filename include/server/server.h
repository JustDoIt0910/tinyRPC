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

    using ServicePtr = std::shared_ptr<google::protobuf::Service>;

    class Server {
    public:
        enum class RpcProtocol {PROTOBUF, HTTP};

        Server(const std::string& addr, uint16_t port, RpcProtocol proto = RpcProtocol::PROTOBUF);

        explicit Server(uint16_t port, RpcProtocol proto = RpcProtocol::PROTOBUF);

        void SetWorkerNum(int num);

        void RegisterService(ServicePtr service);

        RpcProtocol Protocol();

        void Serve();

        ~Server();

    private:
        friend class Session;
        void RemoveSession(const std::string& session_id);

        class Impl;
        std::unique_ptr<Impl> pimpl_;
        RpcProtocol protocol_;
        std::vector<std::thread> workers_;
    };

}

#endif //TINYRPC_SERVER_H
