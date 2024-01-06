//
// Created by just do it on 2024/1/4.
//
#include <utility>

#include "tinyRPC/registry/etcd.h"
#include "tinyRPC/comm/string_util.h"
#include "cpp-httplib/httplib.h"
#include "nlohmann/json.hpp"

namespace tinyRPC {

    EtcdClient::EtcdClient(const std::string &endpoints, LB lb):
    lb_strategy_(lb), endpoint_index_(0), random_engine_(std::random_device()()) {
        std::vector<std::string> urls;
        StringUtil::Split(endpoints, ";", urls);
        if(endpoints.empty()) {
            throw std::runtime_error("no endpoint available");
        }
        for(std::string& url: urls) {
            http_clients_.emplace_back(new httplib::Client(url));
        }
    }

    size_t EtcdClient::GetEndpointIndex() {
        if(http_clients_.empty()) {
            throw std::runtime_error("no endpoint available");
        }
        if(lb_strategy_ == LB::PICK_FIRST) { return 0; }
        if(lb_strategy_ == LB::ROUND_ROBIN) {
            int index = endpoint_index_++;
            if(endpoint_index_ >= http_clients_.size()) {
                endpoint_index_ = 0;
            }
            return index;
        }
        std::uniform_int_distribution<size_t> u(0, http_clients_.size() - 1);
        return u(random_engine_);
    }

    std::shared_ptr<EtcdResponse> EtcdClient::SendReq(std::string url, std::string body) {
        httplib::Request req;
        req.method = "POST";
        req.path = std::move(url);
        req.headers = {
                {"Content-Type", "application/json"}
        };
        req.body = std::move(body);

        auto& c = http_clients_[GetEndpointIndex()];
        httplib::Result res = c->send(req);
        if(!res) {
            std::cout << res.error() << std::endl;
            return {};
        }
        else {
            httplib::Response resp = res.value();
            return std::make_shared<EtcdResponse>(resp.body);
        }
    }

    std::shared_ptr<EtcdResponse>
    EtcdClient::Put(const std::string &key, const std::string &value, uint64_t lease, bool prev_kv,
                    bool ignore_lease, bool ignore_value) {
        nlohmann::json content;
        content["key"] = Base64Encode(key);
        content["value"] = Base64Encode(value);
        content["lease"] = lease;
        content["prev_kv"] = prev_kv;
        content["ignore_lease"] = ignore_lease;
        content["ignore_value"] = ignore_value;

        return SendReq("/v3/kv/put", content.dump());
    }

    std::shared_ptr<EtcdResponse> EtcdClient::Put(const std::string &key, const std::string &value) {
        return Put(key, value, 0, false, false, false);
    }

    std::shared_ptr<EtcdResponse> EtcdClient::Put(const std::string &key, const std::string &value, uint64_t lease) {
        return Put(key, value, 0, lease, false, false);
    }

    EtcdClient::GetRequest& EtcdClient::Get(const std::string &key) {
        get_request_ = std::make_unique<GetRequest>(this, Base64Encode(key));
        return *get_request_;
    }

    EtcdClient::GetRequest& EtcdClient::GetPrefix(const std::string &prefix) {
        std::string range_end = prefix;
        range_end.at(range_end.length() - 1) += 1;
        get_request_ = std::make_unique<GetRequest>(this, Base64Encode(prefix), Base64Encode(range_end));
        return *get_request_;
    }

    void EtcdClient::GetOptions::SetMaxCreateRevision(uint64_t revision) {
        max_create_revision_ = std::to_string(revision);
    }

    void EtcdClient::GetOptions::SetMinCreateRevision(uint64_t revision) {
        min_create_revision_ = std::to_string(revision);
    }

    void EtcdClient::GetOptions::SetMaxModRevision(uint64_t revision) {
        max_mod_revision_ = std::to_string(revision);
    }

    void EtcdClient::GetOptions::SetMinModRevision(uint64_t revision) {
        min_mod_revision_ = std::to_string(revision);
    }

    void EtcdClient::GetOptions::SetRevision(uint64_t revision) {
        revision_ = std::to_string(revision);
    }

    void EtcdClient::GetOptions::SetSerializable(bool enable) { serializable_ = enable; }

    EtcdClient::GetRequest::GetRequest(tinyRPC::EtcdClient *client, std::string key, std::string range_end)
                                       :client_(client), key_(std::move(key)), range_end_(std::move(range_end)),
                                       count_only_(false), keys_only_(false),
                                       sort_order_(SortOrder::NONE), sort_target_(Target::KEY),
                                       limit_(0) {}

    EtcdClient::GetRequest& EtcdClient::GetRequest::Options(GetOptions options) {
        options_ = std::move(options);
        return *this;
    }

    EtcdClient::GetRequest& EtcdClient::GetRequest::Sort(Target target, SortOrder order) {
        sort_target_ = target;
        sort_order_ = order;
        return *this;
    }

    EtcdClient::GetRequest& EtcdClient::GetRequest::Keys() {
        keys_only_ = true;
        return *this;
    }

    EtcdClient::GetRequest& EtcdClient::GetRequest::Count() {
        count_only_ = true;
        return *this;
    }

    std::shared_ptr<EtcdResponse> EtcdClient::GetRequest::Limit(int64_t limit) {
        limit_ = limit;
        return All();
    }

    std::shared_ptr<EtcdResponse> EtcdClient::GetRequest::All() {
        nlohmann::json content;
        content["key"] = key_;
        if(!range_end_.empty()) {
            content["range_end"] = range_end_;
        }
        if(options_.max_create_revision_.has_value()) {
            content["max_create_revision"] = *options_.max_create_revision_;
        }
        if(options_.min_create_revision_.has_value()) {
            content["min_create_revision"] = *options_.min_create_revision_;
        }
        if(options_.max_mod_revision_.has_value()) {
            content["max_mod_revision"] = *options_.max_mod_revision_;
        }
        if(options_.min_mod_revision_.has_value()) {
            content["min_mod_revision"] = *options_.min_mod_revision_;
        }
        if(options_.revision_.has_value()) {
            content["revision"] = *options_.revision_;
        }
        if(options_.serializable_.has_value() && *options_.serializable_) {
            content["serializable"] = true;
        }
        if(keys_only_) { content["keys_only"] = true; }
        if(count_only_) { content["count_only"] = true; }
        if(sort_order_ != SortOrder::NONE) {
            content["sort_order"] = sort_order_str_[static_cast<int>(sort_order_)];
            content["sort_target"] = target_str_[static_cast<int>(sort_target_)];
        }
        if(limit_ > 0) { content["limit"] = std::to_string(limit_); }

        return client_->SendReq("/v3/kv/range", content.dump());
    }

    std::string EtcdClient::target_str_[] = {"KEY", "VERSION", "CREATE", "MOD", "VALUE"};

    std::string EtcdClient::sort_order_str_[] = {"NONE", "ASCEND", "DESCEND"};

    EtcdClient::~EtcdClient() = default;
}