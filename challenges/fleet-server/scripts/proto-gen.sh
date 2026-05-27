#!/bin/bash

# Dynamically find the project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

mkdir -p "$PROJECT_ROOT/genproto"
rm -rf "$PROJECT_ROOT/genproto/"*

# Compile all protos in the protos/ directory
protoc -I "$PROJECT_ROOT/protos/" \
       --cpp_out="$PROJECT_ROOT/genproto/" \
       --grpc_out="$PROJECT_ROOT/genproto/" \
       --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
       "$PROJECT_ROOT/protos/"*.proto

echo "Protobuf files successfully generated in $PROJECT_ROOT/genproto/"
