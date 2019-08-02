#include <iostream>
#include <string>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string read_(tcp::socket& socket)
{
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    //std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    boost::asio::streambuf::const_buffers_type bufs = buf.data();
    std::string data (boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + buf.size());

    return data;
}

void send_(tcp::socket& socket, const std::string message)
{
    boost::asio::write(socket, boost::asio::buffer(message + "\n"));
}

int main()
{
    boost::asio::io_service io_service;

    //listen for new connection
    tcp::acceptor acceptor_(io_service,tcp::endpoint(tcp::v4(), 13));

    //socket creation
    tcp::socket socket_(io_service);
   
    //waiting for the connection
    std::cout << "waiting for the connection" << std::endl;
    acceptor_.accept(socket_);

    //read
    std::string message = read_(socket_);
    std::cout << message << std::endl;

    //write
    send_(socket_, "Hello from server");
    std::cout << "Server replied to client's request" << std::endl;

    return 0;
}