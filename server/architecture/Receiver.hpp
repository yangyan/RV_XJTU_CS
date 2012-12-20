#ifndef RECEIVER_CLASS
#define RECEIVER_CLASS

#include  <iostream>

#include  <boost/asio.hpp>
#include  <boost/bind.hpp>
#include  <boost/archive/text_oarchive.hpp>
#include  <boost/archive/text_iarchive.hpp>

#include    "../../modules/event/Event.hpp"
#include    "EventDispatcher.hpp"

using boost::asio::ip::tcp;

const int max_length = 1024;

namespace rv_xjtu_yangyan
{
#define PORT 23451
    class Receiver 
    {
        public:
            Receiver(EventDispatcher &ed)
                :acceptor_(io_service_, tcp::endpoint(tcp::v4(), PORT)),
                eventDispatcher_(ed)
            {
            }
            void readAndProcess()
            {
                try{
                    boost::asio::ip::tcp::socket socket(io_service_);
                    acceptor_.accept(socket);
                    std::stringstream ss;
                    //for(;;)
                    {
                        char data[max_length];
                        boost::system::error_code error;
                        //这里只读一次，因为确定Event不会超过max_length
                        size_t length = socket.read_some(boost::asio::buffer(data), error);
                        if(error == boost::asio::error::eof)
                        {
                            //do nothing
                            //break;
                        }
                        else if (error)
                            throw boost::system::system_error(error);
                        data[length] = 0;
                        ss << data;
                    }
                    boost::archive::text_iarchive ia(ss);
                    Event e;
                    ia >> e;
                    Result result;
                    eventDispatcher_.inputEvent(e, result);
                    /*
                     *here we want some process

                     可以将这个Event插入服务器端队列之中
                     等待这个任务完成之后，将得到的结果返回来

                     每个程序对应服务器端一个命名的文件，文件中包含规则，
                     如果程序没有请求与服务器连接，那么服务器不会使用这些规则，
                     当第一次连接的时候，服务器从文件初始化出来若干个自动机，
                     对来的事件进行推理

                     */
                    std::stringstream oss;
                    boost::archive::text_oarchive oa(oss);
                    oa << result;
                    cout << oss.str() << endl;
                    std::string os(oss.str());
                    boost::asio::write(socket, boost::asio::buffer(os, os.length()));
                }
                catch (std::exception &e)
                {
                    std::cerr << "Exception in thread: " << e.what() << std::endl;
                }
            }
        private:

        private:
            boost::asio::io_service io_service_;
            //boost::shared_ptr<tcp::socket> socket_;
            tcp::acceptor acceptor_;
            EventDispatcher &eventDispatcher_;
    };

}

#endif
