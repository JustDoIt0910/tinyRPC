//
// Created by just do it on 2024/1/6.
//

#ifndef TINYRPC_ETCD_DEF_H
#define TINYRPC_ETCD_DEF_H
#include <string>
#include <optional>
#include <vector>

namespace tinyRPC {

    struct KV {
        std::string key_;
        std::string value_;
        uint64_t create_revision_;
        uint64_t mod_revision_;
        uint64_t lease_;
        uint64_t version_;
    };

    struct Error {
        uint32_t code_;
        std::string error_;
        std::string message_;
    };

    class EtcdResponse {
    public:
        explicit EtcdResponse(const std::string& body);

        bool Ok();
        KV& Value();

        uint32_t Code();
        std::string Message();
        std::string Error();

        uint64_t ClusterId() const;
        uint64_t MemberId() const;
        uint64_t RaftTerm() const;
        uint64_t Revision() const;

        std::vector<KV>& Results();

    private:
        uint64_t cluster_id_;
        uint64_t member_id_;
        uint64_t raft_term_;
        uint64_t revision_;

        std::optional<KV> kv_;
        std::vector<KV> kvs_;
        std::optional<struct Error> error_;
    };

    std::string Base64Encode(const std::string& data);

    std::string Base64Decode(const std::string& data);
}

#endif //TINYRPC_ETCD_DEF_H
