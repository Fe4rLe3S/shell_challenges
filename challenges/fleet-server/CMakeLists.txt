cmake_minimum_required(VERSION 3.28)
project(fleet_server)

set(CMAKE_CXX_STANDARD 17)

find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

set(GENPROTO_LIB "genproto_lib")
set(GENPROTO_DIR "${PROJECT_SOURCE_DIR}/genproto")

file(GLOB_RECURSE PROTO_SRC "${GENPROTO_DIR}/*.pb.cc")
file(GLOB_RECURSE PROTO_HDR "${GENPROTO_DIR}/*.pb.h")

# 1. Build the Protobuf/gRPC Generated Code as a separate library
add_library(${GENPROTO_LIB} ${PROTO_SRC} ${PROTO_HDR})

target_include_directories(${GENPROTO_LIB} PUBLIC ${GENPROTO_DIR})
target_link_libraries(${GENPROTO_LIB} PUBLIC protobuf::libprotobuf gRPC::grpc++)

# 2. Build the Server Executable
add_executable(server
    src/main.cpp
    src/service/service.cpp
    src/server/server.cpp
)

target_link_libraries(server PRIVATE ${GENPROTO_LIB})
target_include_directories(server PUBLIC ${PROJECT_SOURCE_DIR}/include)

# 3. Build the Client Executable
add_executable(client
    src/client.cpp
)

target_link_libraries(client PRIVATE ${GENPROTO_LIB})
target_include_directories(client PUBLIC ${PROJECT_SOURCE_DIR}/include)
