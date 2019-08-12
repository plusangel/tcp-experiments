cmake_minimum_required(VERSION 2.8.3)
project(systems_bridge)

add_compile_options(-std=c++11)

find_package( Boost 1.58 COMPONENTS system thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR})

find_package(Protobuf REQUIRED)

# check if protobuf was found
if(PROTOBUF_FOUND)
    message ("protobuf found")
else()
    message (FATAL_ERROR "Cannot find Protobuf")
endif()

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)

# Print path to generated files
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

add_executable(simple_client simple_client.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(simple_client
    PUBLIC
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(simple_client 
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
)

add_executable(simple_server simple_server.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(simple_server
    PUBLIC
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(simple_server
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
)