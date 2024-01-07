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
        std::string key;
        std::string value;
        uint64_t create_revision;
        uint64_t mod_revision;
        uint64_t lease;
        uint64_t version;
    };

    struct WatchEvent {
        enum class Type { PUT, DELETE };

        WatchEvent(Type _type, KV _kv): type(_type), kv(std::move(_kv)) {}
        Type type;
        KV kv;
        std::optional<KV> prev_kv;
    };
    using EventList = std::vector<WatchEvent>;

    struct Lease {
        int64_t id;
        int64_t ttl;
        int64_t granted_ttl;
        std::vector<std::string> attacked_keys;
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
        bool HasEvent();
        KV& Value();
        std::vector<KV>& Values();
        uint64_t Count() const;
        EventList& Events();
        struct Lease& Lease();
        std::vector<int64_t>& Leases();

        uint32_t Code();
        std::string Message();
        std::string Error();

        uint64_t ClusterId() const;
        uint64_t MemberId() const;
        uint64_t RaftTerm() const;
        uint64_t Revision() const;

    private:
        uint64_t cluster_id_;
        uint64_t member_id_;
        uint64_t raft_term_;
        uint64_t revision_;

        KV kv_;
        std::vector<KV> kvs_;
        uint64_t count_;
        std::optional<struct Error> error_;
        EventList events_;
        struct Lease lease_{};
        std::vector<int64_t> leases_;
    };

    std::string Base64Encode(const std::string& data);

    std::string Base64Decode(const std::string& data);
}

#endif //TINYRPC_ETCD_DEF_H
