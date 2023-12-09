//
// Created by just do it on 2023/11/30.
//

#ifndef TINYRPC_SERVER_H
#define TINYRPC_SERVER_H
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "asio.hpp"

namespace google::protobuf { class Service; }

namespace tinyRPC {

    class Session;
    class Codec;
    class Router;

    using ServicePtr = std::shared_ptr<google::protobuf::Service>;

    class Server {
    public:
        Server(const std::string& addr, uint16_t port);

        explicit Server(uint16_t port);

        void SetWorkerNum(int num);

        void RegisterService(ServicePtr service);

        asio::ip::tcp::acceptor GetAcceptor();

        void NewSession(asio::ip::tcp::socket& socket, std::unique_ptr<Codec>& codec, Router* router);

        void Serve();

        ~Server();

    private:
        friend class Session;
        void RemoveSession(const std::string& session_id);

        class Impl;
        std::unique_ptr<Impl> pimpl_;
        std::vector<std::thread> workers_;
    };

}

#endif //TINYRPC_SERVER_H
