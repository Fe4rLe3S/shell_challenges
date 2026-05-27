#pragma once

#include <mutex>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <grpcpp/server_context.h>
#include <grpcpp/server_interface.h>
#include <grpcpp/support/status.h>

#include "fleet.grpc.pb.h"
#include "fleet.pb.h"

using grpc::ServerContext;
using grpc::Status;

class FleetService final : public fleet::Fleet::Service {
  public:
    FleetService();

    Status EnrollNode(ServerContext *ctx, const fleet::EnrollRequest *request,
                      fleet::EnrollResponse *response) override;
    Status ListNodes(ServerContext *ctx, const fleet::ListRequest *request,
                     fleet::ListResponse *response) override;

  private:
    std::vector<fleet::Node> nodes_;
    std::mutex mutex_;
    int next_id_;
};
