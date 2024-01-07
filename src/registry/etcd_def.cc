//
// Created by just do it on 2024/1/6.
//
#include "tinyRPC/registry/etcd_def.h"
#include "nlohmann/json.hpp"
#include "b64.h"
#include <iostream>

namespace tinyRPC {

    std::string Base64Encode(const std::string& data) {
        char* encoded = b64_encode(reinterpret_cast<const unsigned char *>(data.data()), data.length());
        std::string res = std::string(reinterpret_cast<char*>(encoded));
        free((void*) encoded);
        return res;
    }

    std::string Base64Decode(const std::string& data) {
        unsigned char* decoded = b64_decode(data.data(), data.length());
        std::string res = std::string(reinterpret_cast<char*>(decoded));
        free((void*) decoded);
        return res;
    }

    static void BindKV(const nlohmann::json& kv, KV& res) {
        std::string key = kv["key"];
        res.key = Base64Decode(kv["key"]);
        if(kv.contains("value")) {
            res.value = Base64Decode(kv["value"]);
        }
        if(kv.contains("lease")) {
            res.lease = std::stoull(kv["lease"].template get<std::string>());
        }
        res.create_revision = std::stoull(kv["create_revision"].template get<std::string>());
        res.mod_revision = std::stoull(kv["mod_revision"].template get<std::string>());
        res.version = std::stoull(kv["version"].template get<std::string>());
    }

    EtcdResponse::EtcdResponse(const std::string& body) {
        nlohmann::json resp = nlohmann::json::parse(body);
        if(resp.contains("header")) {
            const nlohmann::json& header = resp["header"];
            cluster_id_ = std::stoull(header["cluster_id"].template get<std::string>());
            member_id_ = std::stoull(header["member_id"].template get<std::string>());
            raft_term_ = std::stoull(header["raft_term"].template get<std::string>());
            revision_ = std::stoull(header["revision"].template get<std::string>());
            if(resp.contains("kvs")) {
                kvs_.clear();
                nlohmann::json kvs = resp["kvs"];
                for(const auto & i : kvs) {
                    KV kv{};
                    BindKV(i, kv);
                    kvs_.push_back(std::move(kv));
                }
            }
            if(resp.contains("count")) {
                count_ = std::stoull(resp["count"].template get<std::string>());
            }
            if(resp.contains("ID")) {
                lease_.id = std::stoll(resp["ID"].template get<std::string>());
                lease_.ttl = std::stoll(resp["TTL"].template get<std::string>());
                if(resp.contains("grantedTTL")) {
                    lease_.granted_ttl = std::stoll(resp["grantedTTL"].template get<std::string>());
                }
                if(resp.contains("keys")) {
                    lease_.attacked_keys.clear();
                    for(auto& key: resp["keys"]) {
                        lease_.attacked_keys.push_back(Base64Decode(key.template get<std::string>()));
                    }
                }
            }
            if(resp.contains("leases")) {
                nlohmann::json leases = resp["leases"];
                for(auto l: leases) {
                    leases_.push_back(std::stoll(l["ID"].template get<std::string>()));
                }
            }
        }
        else if(resp.contains("error")) {
            struct Error err;
            if(resp.contains("code")) { err.code_ = resp["code"]; }
            if(resp.contains("error")) { err.error_ = resp["error"]; }
            err.message_ = resp["message"];
            error_ = err;
        }
        else if(resp.contains("result")) {
            const nlohmann::json& watch_res = resp["result"];
            const nlohmann::json& header = watch_res["header"];
            cluster_id_ = std::stoull(header["cluster_id"].template get<std::string>());
            member_id_ = std::stoull(header["member_id"].template get<std::string>());
            raft_term_ = std::stoull(header["raft_term"].template get<std::string>());
            revision_ = std::stoull(header["revision"].template get<std::string>());
            if(watch_res.contains("events")) {
                const nlohmann::json& events = watch_res["events"];
                for(const auto& e: events) {
                    WatchEvent::Type type;
                    if(!e.contains("type")) { type = WatchEvent::Type::PUT; }
                    else {
                        type = e["type"] == "PUT" ? WatchEvent::Type::PUT: WatchEvent::Type::DELETE;
                    }
                    const nlohmann::json& kv = e["kv"];
                    KV _kv;
                    BindKV(kv, _kv);
                    WatchEvent event(type, std::move(_kv));
                    if(e.contains("prev_kv")) {
                        const nlohmann::json& prev_kv = e["prev_kv"];
                        KV _prev_kv;
                        BindKV(prev_kv, _prev_kv);
                        event.prev_kv = std::move(_prev_kv);
                    }
                    events_.push_back(std::move(event));
                }
            }
        }
    }

    bool EtcdResponse::Ok() { return !error_; }

    uint32_t EtcdResponse::Code() { return error_->code_; }

    std::string EtcdResponse::Error() { return error_->error_; }

    std::string EtcdResponse::Message() { return error_->message_; }

    uint64_t EtcdResponse::ClusterId() const { return cluster_id_; }

    uint64_t EtcdResponse::MemberId() const { return member_id_; }

    uint64_t EtcdResponse::RaftTerm() const { return raft_term_; }

    uint64_t EtcdResponse::Revision() const { return revision_; }

    KV& EtcdResponse::Value() { return kv_; }

    std::vector<KV> &EtcdResponse::Values() { return kvs_; }

    uint64_t EtcdResponse::Count() const { return count_; }

    bool EtcdResponse::HasEvent() { return !events_.empty(); }

    EventList& EtcdResponse::Events() { return events_; }

    struct Lease& EtcdResponse::Lease() { return lease_; }

    std::vector<int64_t>& EtcdResponse::Leases() { return leases_; }

}