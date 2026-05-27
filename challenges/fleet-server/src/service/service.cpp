#include "service/service.hpp"

FleetService::FleetService() : next_id_(1000) {
}

Status FleetService::EnrollNode(ServerContext *ctx,
                                const fleet::EnrollRequest *request,
                                fleet::EnrollResponse *response) {
    std::lock_guard<std::mutex> lock(this->mutex_);

    fleet::Node new_node;
    new_node.set_id(std::to_string(++next_id_));
    new_node.set_name(request->name());
    new_node.set_os_type(request->os_type());

    this->nodes_.push_back(new_node);
    response->mutable_node()->CopyFrom(new_node);

    return grpc::Status::OK;
}

Status FleetService::ListNodes(ServerContext *ctx,
                               const fleet::ListRequest *request,
                               fleet::ListResponse *response) {
    std::lock_guard<std::mutex> lock(this->mutex_);

    for (const auto &node : this->nodes_) {
        response->add_nodes()->CopyFrom(node);
    }

    return grpc::Status::OK;
}
