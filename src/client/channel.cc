//
// Created by just do it on 2023/12/4.
//
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "tinyRPC/client/channel.h"
#include "tinyRPC/rpc/message.h"
#include "tinyRPC/rpc/controller.h"
#include "tinyRPC/registry/registry.h"
#include "tinyRPC/comm/string_util.h"

namespace tinyRPC {

    Channel::Channel(const std::string &registry_endpoints, const std::shared_ptr<Balancer> &lb):
    mode_(ConnectMode::REGISTRY), lb_(lb) {
        resolver_ = std::make_unique<Resolver>(registry_endpoints, lb);
    }

    Channel::Channel(const std::string &address, uint16_t port): mode_(ConnectMode::DIRECT) {
        client_ = std::make_unique<Client>(address, port);
    }

    Channel::Channel(asio::io_context *ctx, const std::string &address, uint16_t port):
    mode_(ConnectMode::DIRECT) {
        client_ = std::make_unique<Client>(ctx, address, port);
    }

    void Channel::WaitForConnect() { client_->WaitForConnect(); }

    void Channel::CallMethod(const google::protobuf::MethodDescriptor *method,
                        google::protobuf::RpcController *controller,
                        const google::protobuf::Message *request,
                        google::protobuf::Message *response,
                        google::protobuf::Closure *done) {
        auto rpc_controller = dynamic_cast<Controller*>(controller);
        RpcRequest req_msg;
        req_msg.msg_id_ = rpc_controller->RequestId();
        req_msg.full_method_name_ = method->full_name();
        if(!request->SerializeToString(&req_msg.data_)) {
            // TODO handle error
        }
        if(mode_ == ConnectMode::DIRECT) {
            Invoke(client_, rpc_controller, req_msg, response, done);
        }
        else {
            if(!resolved_) {
                resolver_->Resolve(method->service()->name());
                resolved_ = true;
            }
            std::string key = rpc_controller->LoadBalanceKey();
            if(key.empty()) {
                key = Registry::GetInterfaceIP();
            }
            std::string endpoint = lb_->GetEndpoint(key);
            if(endpoint.empty()) {
                throw no_endpoint_error(method->service()->name());
            }
            auto it = clients_.find(endpoint);
            if(it == clients_.end()) {
                std::vector<std::string> ip_port;
                StringUtil::Split(endpoint, ":", ip_port);
                it = clients_.insert(
                        std::make_pair(endpoint, std::make_unique<Client>(ip_port[0], std::stoi(ip_port[1])))).first;
            }
            assert(it != clients_.end());
            std::cout << "====> (" << endpoint << ")" << std::endl;
            Invoke((*it).second, rpc_controller, req_msg, response, done);
        }
    }

    void Channel::Invoke(ClientPtr& client, Controller *controller,
                         RpcRequest& request, google::protobuf::Message *response,
                         google::protobuf::Closure *done) {
        auto timeout = controller->Timeout();
        if(timeout) {
            client->SetConnectTimeout(*timeout);
        }
        if(controller->Async()) { client->Call(request, done); }
        else {
            auto fu = client->Call(request);
            RpcResponse resp = fu.get();
            response->ParseFromString(resp.data_);
        }
    }

    Channel::~Channel() = default;
}