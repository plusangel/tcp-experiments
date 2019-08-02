#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class con_handler : public boost::enable_shared_from_this<con_handler>
{
    private:
    
    tcp::socket socket_;
    
    char data[1024];

    public:  
    typedef boost::shared_ptr<con_handler> pointer;  
        
    con_handler(boost::asio::io_service& io_service): socket_(io_service)
    {}  


    pointer create(boost::asio::io_service& io_service){  
        return pointer(new con_handler(io_service));  
    }  

    tcp::socket& socket(){  
        return socket_;  
    }  

    void start(){  
        socket_.async_read_some(boost::asio::buffer(data, 1024),  
            boost::bind(&con_handler::handle_read,  
            shared_from_this(),  
            boost::asio::placeholders::error,  
            boost::asio::placeholders::bytes_transferred));  

        socket_.async_write_some(boost::asio::buffer("Hello from server", 1024),  
            boost::bind(&con_handler::handle_write,  
            shared_from_this(),  
            boost::asio::placeholders::error,  
            boost::asio::placeholders::bytes_transferred));  
    }  

    void handle_read(const boost::system::error_code& err,size_t bytes_transferred){  
        if (!err) {  
            std::cout << data << std::endl;  
        }   
        else {  
            std::cerr << "error: " << err.message() << std::endl;  
            socket_.close();  
        }  
    }  

    void handle_write(const boost::system::error_code& err,size_t bytes_transferred){  
        if (!err) {  
            std::cout << "Server sent Hello message!"<< std::endl;  
        }   
        else {  
            std::cerr << "error: " << err.message() << std::endl;  
            socket_.close();  
        }  
    }  
};

class Server {  
    private:

    tcp::acceptor acceptor_;  
    
    void start_accept()
    {  
        con_handler::pointer connection = con_handler::create(acceptor_.get_io_service());

        acceptor_.async_accept(connection->socket(),  
            boost::bind(&Server::handle_accept, this, connection, boost::asio::placeholders::error));  
    }  
    
    public:    
    Server(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
    {  
        start_accept();  
    }  
    
    void handle_accept(con_handler::pointer connection,const boost::system::error_code& err)
    {  
        if (!err) {  
            connection->start();  
        }  
        start_accept();  
    }  
};

int main(int argc, char *argv[])  
{  
    try{  
        boost::asio::io_service io_service;  
        Server server(io_service);  
        io_service.run();  
    }  

    catch(std::exception& e){  
        std::cerr << e.what() << std::endl;  
    }  
    return 0;  
}  

