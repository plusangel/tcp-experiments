/*
    TCP client
    compile:
    g++ -o simple_client simple_client.cpp -lboost_system -lboost_thread -std=c++11

 */
#include <iostream>
#include <string>
#include <stdlib.h>

#include <boost/asio.hpp>
#include "message.pb.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    boost::asio::io_service io_service;

    tcp::socket socket(io_service);

    if (argc < 2) 
    {   
        std::cerr << "Use it properly: simple_client num [num is the required action]" << std::endl;
        return 1;
    }

    std::cout << "Try to connect to the server port" << std::endl;

    boost::system::error_code error;
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 13), error);

    if (error)
    {
        std::cerr << "Error in connection: " << error.message() << std::endl;
        return 1;
    } else {
        std::cout << "Send the action" << std::endl;
    }

    robot::comm msg;
    msg.set_uuid("hello");
    msg.set_timestamp("world");
    msg.set_action(std::atoi(argv[1]));
    
    boost::asio::streambuf buf;
    std::ostream os(&buf);
    std::string proto_string;
    proto_string = msg.SerializeAsString();
    boost::asio::write(socket, boost::asio::buffer(proto_string), error);

    
    if (!error)
    {
        std::cout << "request sent to server" << std::endl;
    } else
    {
        std::cerr << "sent failed because of " << error.message() << std::endl;
    }
    
    std::cout << "waiting for reply" << std::endl;
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);

    if (error && error != boost::asio::error::eof)
    {
        std::cout << "receive failed: " << error.message() << std::endl;
    } else
    {
        const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());  
        std::cout << data << std::endl; 
    }
    std::cout << "finished" << std::endl;

    return 0;
}