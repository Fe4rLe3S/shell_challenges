#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "fleet.grpc.pb.h"

using fleet::Fleet;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class FleetClient {
  public:
    FleetClient(std::shared_ptr<Channel> channel)
        : stub_(Fleet::NewStub(channel)) {
    }

    void Enroll(const std::string &name, const std::string &os_type) {
        fleet::EnrollRequest request;
        request.set_name(name);
        request.set_os_type(os_type);

        fleet::EnrollResponse reply;
        ClientContext context;

        Status status = stub_->EnrollNode(&context, request, &reply);
        if (status.ok()) {
            std::cout << "[\033[32mSUCCESS\033[0m] Enrolled node ID: "
                      << reply.node().id() << " | Name: " << reply.node().name()
                      << "\n";
        } else {
            std::cout << "[\033[31mFAILED\033[0m] " << status.error_message()
                      << "\n";
        }
    }

    void List() {
        fleet::ListRequest request;
        fleet::ListResponse reply;
        ClientContext context;

        Status status = stub_->ListNodes(&context, request, &reply);
        if (status.ok()) {
            std::cout << "\n=== FLEET NODE INVENTORY (" << reply.nodes_size()
                      << " Nodes) ===\n";
            for (int i = 0; i < reply.nodes_size(); i++) {
                const fleet::Node &n = reply.nodes(i);
                std::cout << "  - [" << n.id() << "] " << n.name() << " ("
                          << n.os_type() << ")\n";
            }
            std::cout << "======================================\n";
        } else {
            std::cout << "[\033[31mFAILED\033[0m] RPC ListNodes: "
                      << status.error_message() << "\n";
        }
    }

  private:
    std::unique_ptr<Fleet::Stub> stub_;
};

int main(int argc, char **argv) {
    FleetClient client(grpc::CreateChannel("localhost:50051",
                                           grpc::InsecureChannelCredentials()));

    std::cout << "--- Sending 3 Enrollment Requests ---\n";
    client.Enroll("web-frontend-01", "Ubuntu 22.04 LTS");
    client.Enroll("db-primary-node", "Debian 12");
    client.Enroll("cache-worker-04", "Alpine Linux");

    std::cout << "\n--- Requesting Fleet Inventory ---\n";
    client.List();

    return 0;
}
