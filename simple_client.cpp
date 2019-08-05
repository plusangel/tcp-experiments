/*
    TCP client
    compile:
    g++ -o simple_client simple_client.cpp -lboost_system -lboost_thread -std=c++11

 */
#include <iostream>
#include <string>
#include <stdlib.h>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;

    tcp::socket socket(io_service);

    if (argc < 2) 
    {   
        std::cout << "Use it properly!" << std::endl;
        return 1;
    }

    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 13));
    
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(std::string(argv[1])), error);

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