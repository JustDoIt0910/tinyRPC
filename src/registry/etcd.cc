//
// Created by just do it on 2024/1/4.
//
#include <utility>
#include <future>
#include "tinyRPC/registry/etcd.h"
#include "cpp-httplib/httplib.h"
#include "nlohmann/json.hpp"

namespace tinyRPC {
    EtcdClient::EtcdClient(std::vector<std::string> endpoints, tinyRPC::EtcdClient::LB lb):
    endpoints_(std::move(endpoints)), lb_strategy_(lb), endpoint_index_(0), random_engine_(std::random_device()()) {
        if(endpoints_.empty()) {
            throw std::runtime_error("no endpoint available");
        }
    }

    size_t EtcdClient::GetEndpointIndex() {
        if(endpoints_.empty()) {
            throw std::runtime_error("no endpoint available");
        }
        if(lb_strategy_ == LB::PICK_FIRST) { return 0; }
        if(lb_strategy_ == LB::ROUND_ROBIN) {
            int index = endpoint_index_++;
            if(endpoint_index_ >= endpoints_.size()) {
                endpoint_index_ = 0;
            }
            return index;
        }
        std::uniform_int_distribution<size_t> u(0, endpoints_.size() - 1);
        return u(random_engine_);
    }

    std::shared_ptr<EtcdResponse> EtcdClient::SendReq(std::string url, std::string body) {
        httplib::Request req;
        req.method = "POST";
        req.path = std::move(url);
        req.headers = {{"Content-Type", "application/json"}};
        if(!body.empty()) {
            req.body = std::move(body);
        }
        httplib::Client c(endpoints_[GetEndpointIndex()]);
        httplib::Result res = c.send(req);
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
        content["lease"] = std::to_string(lease);
        content["prev_kv"] = prev_kv;
        content["ignore_lease"] = ignore_lease;
        content["ignore_value"] = ignore_value;

        return SendReq("/v3/kv/put", content.dump());
    }

    std::shared_ptr<EtcdResponse> EtcdClient::Put(const std::string &key, const std::string &value) {
        return Put(key, value, 0, false, false, false);
    }

    std::shared_ptr<EtcdResponse> EtcdClient::Put(const std::string &key, const std::string &value, uint64_t lease) {
        return Put(key, value, lease, false, false, false);
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

    EtcdClient::WatchRequest& EtcdClient::Watch(const std::string &key, uint64_t watch_id, WatchFilter filter) {
        watch_request_ = std::make_shared<WatchRequest>(this, watch_id, filter, Base64Encode(key));
        return *watch_request_;
    }

    EtcdClient::WatchRequest& EtcdClient::WatchPrefix(const std::string& prefix, uint64_t watch_id, WatchFilter filter) {
        std::string range_end = prefix;
        range_end.at(range_end.length() - 1) += 1;
        watch_request_ = std::make_shared<WatchRequest>(this, watch_id, filter,
                                                        Base64Encode(prefix), Base64Encode(range_end));
        return *watch_request_;
    }

    std::shared_ptr<EtcdResponse> EtcdClient::GrantLease(int64_t ttl, int64_t id) {
        nlohmann::json content;
        content["ID"] = std::to_string(id);
        content["TTL"] = std::to_string(ttl);
        auto resp = SendReq("/v3/lease/grant", content.dump());
        if(resp && resp->Ok()) {
            Lease lease = resp->Lease();
            granted_leases_[lease.id] = lease;
        }
        return resp;
    }

    std::shared_ptr<EtcdResponse> EtcdClient::RevokeLease(int64_t id) {
        nlohmann::json content;
        content["ID"] = std::to_string(id);
        return SendReq("/v3/lease/revoke", content.dump());
    }

    std::shared_ptr<EtcdResponse> EtcdClient::LeaseTTL(int64_t id, bool keys) {
        nlohmann::json content;
        content["ID"] = std::to_string(id);
        content["keys"] = keys;
        return SendReq("/v3/lease/timetolive", content.dump());
    }

    std::shared_ptr<EtcdResponse> EtcdClient::Leases() {
        return SendReq("/v3/lease/leases", "");
    }

    void EtcdClient::Keepalive(int64_t lease_id) {
        auto it = granted_leases_.find(lease_id);
        if(it == granted_leases_.end()) { return; }
        int64_t keepalive_interval = (*it).second.ttl / 3;

        nlohmann::json content;
        content["ID"] = std::to_string(lease_id);
        std::shared_ptr<httplib::Client> c = std::make_shared<httplib::Client>(endpoints_[GetEndpointIndex()]);
        std::shared_ptr<httplib::Request> req = std::make_shared<httplib::Request>();
        req->method = "POST";
        req->path = "/v3/lease/keepalive";
        req->headers = {{"Content-Type", "application/json"}};
        req->body = content.dump();
        std::thread keepalive_thread = std::thread([c, req, keepalive_interval]() {
            while(true) {
                c->send(*req);
                std::this_thread::sleep_for(std::chrono::seconds(keepalive_interval));
            }
        });
        keepalive_thread.detach();
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
        if(!range_end_.empty()) { content["range_end"] = range_end_; }
        if(options_.max_create_revision_.has_value()) { content["max_create_revision"] = *options_.max_create_revision_; }
        if(options_.min_create_revision_.has_value()) { content["min_create_revision"] = *options_.min_create_revision_; }
        if(options_.max_mod_revision_.has_value()) { content["max_mod_revision"] = *options_.max_mod_revision_; }
        if(options_.min_mod_revision_.has_value()) { content["min_mod_revision"] = *options_.min_mod_revision_; }
        if(options_.revision_.has_value()) { content["revision"] = *options_.revision_; }
        if(options_.serializable_.has_value() && *options_.serializable_) { content["serializable"] = true; }
        if(keys_only_) { content["keys_only"] = true; }
        if(count_only_) { content["count_only"] = true; }
        if(sort_order_ != SortOrder::NONE) {
            content["sort_order"] = sort_order_str_[static_cast<int>(sort_order_)];
            content["sort_target"] = target_str_[static_cast<int>(sort_target_)];
        }
        if(limit_ > 0) { content["limit"] = std::to_string(limit_); }

        return client_->SendReq("/v3/kv/range", content.dump());
    }

    void EtcdClient::WatchOptions::SetFragment(bool enable) { fragment_ = enable; }

    void EtcdClient::WatchOptions::SetPrevKV(bool enable) { prev_kv_ = enable; }

    void EtcdClient::WatchOptions::SetProgressNotify(bool enable) { progress_notify_ = enable; }

    EtcdClient::WatchRequest::WatchRequest(EtcdClient* client, uint64_t watch_id, WatchFilter filter,
                                           std::string key, std::string range_end)
    :client_(client), watch_id_(watch_id), filter_(filter),
    key_(std::move(key)), range_end_(std::move(range_end)),
    start_revision_(0), response_received_(false) {}

    EtcdClient::WatchRequest& EtcdClient::WatchRequest::Options(WatchOptions options) {
        options_ = options;
        return *this;
    }

    EtcdClient::WatchRequest& EtcdClient::WatchRequest::From(uint64_t start_revision) {
        start_revision_ = start_revision;
        return *this;
    }

    std::shared_ptr<EtcdResponse> EtcdClient::WatchRequest::HandleEvent(const WatchEventHandler& handler) {
        nlohmann::json content;
        nlohmann::json create_req;
        create_req["key"] = key_;
        if(!range_end_.empty()) { create_req["range_end"] = range_end_; }
        if(filter_ != WatchFilter::NONE) { create_req["filter"] = watch_filter_str_[static_cast<int>(filter_)]; }
        if(start_revision_ > 0) { create_req["start_revision"] = std::to_string(start_revision_); }
        if(watch_id_ != 0) { create_req["watch_id"] = std::to_string(watch_id_); }
        if(options_.fragment_.has_value()) { create_req["fragment"] = *options_.fragment_; }
        if(options_.prev_kv_.has_value()) { create_req["prev_kv"] = *options_.prev_kv_; }
        if(options_.progress_notify_.has_value()) { create_req["progress_notify"] = *options_.progress_notify_; }
        content["create_request"] = create_req;

        std::shared_ptr<httplib::Request> req = std::make_shared<httplib::Request>();
        req->method = "POST";
        req->path = "/v3/watch";
        req->headers = {{"Content-Type", "application/json"}};
        req->body = content.dump();
        auto p = std::make_shared<std::promise<std::shared_ptr<EtcdResponse>>>();
        auto fu = p->get_future();
        std::weak_ptr<std::promise<std::shared_ptr<EtcdResponse>>> wp = p;
        req->content_receiver = [this, wp, &handler](const char *data, size_t data_length, uint64_t offset, uint64_t total_length) {
            auto response = std::make_shared<EtcdResponse>(std::string(data, data_length));
            auto p = wp.lock();
            if(p && !response_received_) {
                response_received_ = true;
                p->set_value(response);
            }
            if(response->HasEvent()) {
                handler(response->Events());
            }
            return true;
        };
        auto self = shared_from_this();
        std::thread watch_thread = std::thread([this, self, req, wp](){
            httplib::Client c(client_->endpoints_[client_->GetEndpointIndex()]);
            c.set_read_timeout(-1);
            auto res = c.send(*req);
            if(!res) {
                auto p = wp.lock();
                if(p) { p->set_value({}); }
            }
        });
        watch_thread.detach();
        return fu.get();
    }

    std::string EtcdClient::target_str_[] = {"KEY", "VERSION", "CREATE", "MOD", "VALUE"};

    std::string EtcdClient::sort_order_str_[] = {"NONE", "ASCEND", "DESCEND"};

    std::string EtcdClient::watch_filter_str_[] = {"NONE", "NOPUT", "NODELETE"};

    EtcdClient::~EtcdClient() = default;
}