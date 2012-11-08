#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include  <iostream>

#include  <boost/asio.hpp>
#include  <boost/bind.hpp>
#include  <boost/archive/text_oarchive.hpp>
#include  <boost/archive/text_iarchive.hpp>

#include    "../event/Event.hpp"

using boost::asio::ip::tcp;

const int max_length = 1024;

namespace rv_xjtu_yangyan
{
    class Receiver 
    {
        public:
            Receiver(short port)
            {
                boost::asio::io_service io_service;
                tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

                boost::shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
                socket_ = socket;
                acceptor.accept(*socket_);
            }
            void read()
            {
                try{
                    std::stringstream ss;
                    for(;;)
                    {
                        char data[max_length];
                        boost::system::error_code error;
                        size_t length = socket_->read_some(boost::asio::buffer(data), error);
                        if(error == boost::asio::error::eof)
                            break;
                        else if (error)
                            throw boost::system::system_error(error);
                        data[length] = 0;
                        ss << data;
                    }
                    boost::archive::text_iarchive ia(ss);
                    Event e;
                    ia >> e;
                    //std::cout << e.toString() << std::endl;
                    /*
                     *here we want some process

                     可以将这个Event插入服务器端队列之中
                     等待这个任务完成之后，将得到的结果返回来

                     每个程序对应服务器端一个命名的文件，文件中包含规则，
                     如果程序没有请求与服务器连接，那么服务器不会使用这些规则，
                     当第一次连接的时候，服务器从文件初始化出来若干个自动机，
                     对来的事件进行推理

                     */
                }
                catch (std::exception &e)
                {
                    std::cerr << "Exception in thread: " << e.what() << std::endl;
                }
            }
        private:

        private:
            boost::shared_ptr<tcp::socket> socket_;
    };

}

#endif
