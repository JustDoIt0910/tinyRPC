//
// Created by just do it on 2024/1/4.
//

#ifndef TINYRPC_ETCD_H
#define TINYRPC_ETCD_H

#include <vector>
#include <memory>
#include <optional>
#include <random>
#include <thread>
#include <functional>
#include "tinyRPC/registry/etcd_def.h"

namespace httplib { class Client;}

namespace tinyRPC {

    class EtcdClient {
    public:
        enum class LB { ROUND_ROBIN, PICK_FIRST, RANDOM };
        enum class SortOrder { NONE, ASCEND, DESCEND };
        enum class Target { KEY, VERSION, CREATE, MOD, VALUE };
        enum class WatchFilter { NONE, NO_PUT, NO_DELETE };

        class GetRequest;
        class GetOptions {
        public:
            void SetMaxCreateRevision(uint64_t revision);

            void SetMinCreateRevision(uint64_t revision);

            void SetMaxModRevision(uint64_t revision);

            void SetMinModRevision(uint64_t revision);

            void SetRevision(uint64_t revision);

            void SetSerializable(bool enable);

        private:
            friend class EtcdClient::GetRequest;

            std::optional<std::string> max_create_revision_;
            std::optional<std::string> min_create_revision_;
            std::optional<std::string> max_mod_revision_;
            std::optional<std::string> min_mod_revision_;
            std::optional<std::string> revision_;
            std::optional<bool> serializable_;
        };

        class GetRequest {
        public:
            GetRequest(EtcdClient* client, std::string key, std::string range_end = std::string());

            std::shared_ptr<EtcdResponse> All();

            std::shared_ptr<EtcdResponse> Limit(int64_t limit);

            GetRequest& Count();

            GetRequest& Keys();

            GetRequest& Sort(Target target, SortOrder order);

            GetRequest& Options(GetOptions options);

        private:
            EtcdClient* client_;
            std::string key_;
            std::string range_end_;
            GetOptions options_;
            bool count_only_;
            bool keys_only_;
            SortOrder sort_order_;
            Target sort_target_;
            int64_t limit_;
        };

        class WatchRequest;
        class WatchOptions {
        public:
            void SetFragment(bool enable);

            void SetPrevKV(bool enable);

            void SetProgressNotify(bool enable);

        private:
            friend class EtcdClient::WatchRequest;

            std::optional<bool> fragment_;
            std::optional<bool> prev_kv_;
            std::optional<bool> progress_notify_;
        };

        using WatchEventHandler = std::function<void(EventList&)>;

        class WatchRequest {
        public:
            WatchRequest(EtcdClient* client, uint64_t watch_id, WatchFilter filter,
                         std::string key, std::string range_end = std::string());

            WatchRequest& Options(WatchOptions options);

            WatchRequest& From(uint64_t start_revision);

            std::shared_ptr<EtcdResponse> HandleEvent(const WatchEventHandler& handler);

        private:
            EtcdClient* client_;
            uint64_t watch_id_;
            WatchFilter filter_;
            std::string key_;
            std::string range_end_;
            WatchOptions options_;
            uint64_t start_revision_;
            std::thread watch_thread_;
        };

        explicit EtcdClient(const std::string& endpoints, LB lb = LB::ROUND_ROBIN);

        std::shared_ptr<EtcdResponse> Put(const std::string& key, const std::string& value);

        std::shared_ptr<EtcdResponse> Put(const std::string& key, const std::string& value, uint64_t lease);

        std::shared_ptr<EtcdResponse> Put(const std::string& key, const std::string& value, uint64_t lease,
                                          bool prev_kv, bool ignore_lease, bool ignore_value);

        GetRequest& Get(const std::string& key);

        GetRequest& GetPrefix(const std::string& prefix);

        WatchRequest& Watch(const std::string& key, uint64_t watch_id = 0, WatchFilter filter = WatchFilter::NONE);

        WatchRequest& WatchPrefix(const std::string& prefix, uint64_t watch_id = 0, WatchFilter filter = WatchFilter::NONE);

        std::shared_ptr<EtcdResponse> CancelWatch(uint64_t watch_id);

        ~EtcdClient();

    private:
        size_t GetEndpointIndex();
        std::shared_ptr<EtcdResponse> SendReq(std::string url, std::string body);

        std::vector<std::unique_ptr<httplib::Client>> http_clients_;
        LB lb_strategy_;
        int endpoint_index_;
        std::mt19937 random_engine_;
        std::unique_ptr<GetRequest> get_request_;
        std::unique_ptr<WatchRequest> watch_request_;

        static std::string target_str_[];
        static std::string sort_order_str_[];
        static std::string watch_filter_str_[];
    };

}

#endif //TINYRPC_ETCD_H
