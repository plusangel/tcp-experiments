#include <iostream>
#include <string>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main()
{
    boost::asio::io_service io_service;

    tcp::socket socket(io_service);

    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 13));

    const std::string msg = "Hello from client!\n";
    boost::system::error_code error;

    boost::asio::write(socket, boost::asio::buffer(msg), error);

    if (!error)
    {
        std::cout << "request sent to server" << std::endl;
    } else
    {
        std::cerr << "sent failed because of " << error.message() << std::endl;
    }
    

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
    return 0;
}