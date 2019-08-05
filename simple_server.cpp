/*
    TCP async threaded server
    compile:
    g++ -o simple_server simple_server.cpp -lboost_system -pthread -lboost_thread -std=c++11

 */
#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <boost/date_time.hpp>

using boost::asio::ip::tcp;

class Worker
{
    public:
    Worker() = default;

    void move()
    {
        boost::posix_time::seconds workTime(10); 
        std::cout << "moving begins" << std::endl;
        boost::this_thread::sleep(workTime);
        std::cout << "moving ends" << std::endl;
    }

    void stop()
    { 
        std::cout << "Stop immediatelly" << std::endl;
    }
};

class con_handler : public boost::enable_shared_from_this<con_handler>
{
    private:
    
    tcp::socket socket_;
    Worker coscr;     
    char data[1024];

    public:  
    typedef boost::shared_ptr<con_handler> pointer;  
        
    con_handler(boost::asio::io_service& io_service): socket_(io_service)
    {}  


    static pointer create(boost::asio::io_service& io_service){  
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
        
        data[bytes_transferred] = '\0';
        
        if (!err) {
            
            std::cout << "I received: " << data << ", transferred " << bytes_transferred << std::endl;
            if (std::atoi(data) == 1)
            {
                std::cout << "Start to move 1" << std::endl;
                boost::thread t{&Worker::move, &coscr};
            } else if (std::atoi(data) == 2)
            {
                std::cout << "Stop immediatelly 1" << std::endl;
                boost::thread t{&Worker::stop, &coscr};
            }
            
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

