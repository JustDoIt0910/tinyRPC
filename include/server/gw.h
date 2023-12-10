//
// Created by just do it on 2023/12/10.
//

#ifndef TINYRPC_GW_H
#define TINYRPC_GW_H
#include <memory>

namespace google::protobuf { class Service; }

namespace tinyRPC {

    class AbstractHttpApiGateway {
    public:
        virtual void RegisterService(std::shared_ptr<google::protobuf::Service> service) = 0;
    };

}

#endif //TINYRPC_GW_H
