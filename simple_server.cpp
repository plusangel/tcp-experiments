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
    private:
    volatile bool emergency_stop;

    public:
    Worker() = default;

    
    void move()
    {
        emergency_stop = false;
        boost::posix_time::seconds workTime(0.5); 
        std::cout << "moving begins " << emergency_stop << std::endl;
        
        while (!emergency_stop){   
            boost::this_thread::sleep(workTime); 
        }
        std::cout << "moving ends" << std::endl;
    }

    void stop()
    {  
        emergency_stop = true;
        std::cout << "Stop immediatelly " << emergency_stop << std::endl;
    }
};



class con_handler : public boost::enable_shared_from_this<con_handler>
{
    private:
    boost::thread move_t;
    boost::thread stop_t;
    tcp::socket socket_;
    Worker robot_;     
    char data[1024];

    public:  
    typedef boost::shared_ptr<con_handler> pointer;  
        
    con_handler(boost::asio::io_service& io_service, Worker &robot): socket_(io_service), robot_(robot)
    {
        std::cout << "The connection handler just created" << std::endl;
    }  


    static pointer create(boost::asio::io_service& io_service, Worker& robot){  
        return pointer(new con_handler(io_service, robot));  
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
            if (std::atoi(data) == 1)
            {
                move_t = boost::thread(&Worker::move, &robot_);          
            } else if (std::atoi(data) == 2)
            {
                stop_t = boost::thread(&Worker::stop, &robot_);             
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
    Worker coscr;
    tcp::acceptor acceptor_;  
    
    void start_accept()
    {  
        con_handler::pointer connection = con_handler::create(acceptor_.get_io_service(), coscr);

        acceptor_.async_accept(connection->socket(),  
            boost::bind(&Server::handle_accept, this, connection, boost::asio::placeholders::error));  
    }  
    
    public:    
    Server(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
    {   
        std::cout << "The server is up and running" << std::endl;
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