#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


class printer {
    public:
    printer(boost::asio::io_service& io) :
        strand_(io), 
        timer1_(io, boost::posix_time::seconds(1)),
        timer2_(io, boost::posix_time::seconds(1)),
        count_(0)
    {
        timer1_.async_wait(strand_.wrap(boost::bind(&printer::print1, this)));
        timer2_.async_wait(strand_.wrap(boost::bind(&printer::print2, this)));
    }

    ~printer()
    {
        std::cout << "The final count is " << count_ << std::endl;
    }

    void print1()
    {
        if (count_ < 10)
        {
        std::cout << "Timer1: " << count_ << std::endl;
        ++count_;

        timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
        timer1_.async_wait(strand_.wrap(boost::bind(&printer::print1, this)));
        }
    }

    void print2()
    {
        if (count_ < 10)
        {
        std::cout << "Timer2: " << count_ << std::endl;
        ++count_;

        timer2_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
        timer2_.async_wait(strand_.wrap(boost::bind(&printer::print2, this)));
        }
    }

    private:
    boost::asio::io_service::strand strand_;
    boost::asio::deadline_timer timer1_;
    boost::asio::deadline_timer timer2_;
    int count_;

};

int main()
{
    boost::asio::io_service io;
    printer p(io);

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io));

    io.run();
    t.join();

    return 0;
}