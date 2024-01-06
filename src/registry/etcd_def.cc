//
// Created by just do it on 2024/1/6.
//
#include "tinyRPC/registry/etcd_def.h"
#include "nlohmann/json.hpp"

namespace tinyRPC {

    EtcdResponse::EtcdResponse(const std::string& body) {
        nlohmann::json resp = nlohmann::json::parse(body);
        if(resp.contains("header")) {
            const nlohmann::json& header = resp["header"];
            cluster_id_ = std::stoull(header["cluster_id"].template get<std::string>());
            member_id_ = std::stoull(header["member_id"].template get<std::string>());
            raft_term_ = std::stoull(header["raft_term"].template get<std::string>());
            revision_ = std::stoull(header["revision"].template get<std::string>());
            if(resp.contains("kvs")) {

            }
        }
        else if(resp.contains("error")) {
            struct Error err;
            err.code_ = resp["code"];
            err.error_ = resp["error"];
            err.message_ = resp["message"];
            error_ = err;
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

}