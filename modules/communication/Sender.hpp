#ifndef SENDER_HPP
#define SENDER_HPP

#include  <sstream>

#include  <boost/asio.hpp>
#include  <boost/bind.hpp>
#include  <boost/archive/text_oarchive.hpp>
#include  <boost/archive/text_iarchive.hpp>

#include    "../event/Event.hpp"

using boost::asio::ip::tcp;

namespace rv_xjtu_yangyan
{
    class Sender
    {
        public:
            Sender(std::string host, std::string port)
            {
                boost::asio::io_service io_service;
                tcp::resolver resolver(io_service);
                tcp::resolver::query query(tcp::v4(), host, port);
                tcp::resolver::iterator iterator = resolver.resolve(query);

                boost::shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
                socket_ = socket;
                boost::asio::connect(*socket_, iterator);
            }
            void write(std::string &msg)
            {
                boost::asio::write(*socket_, boost::asio::buffer(msg, msg.length()));
            }
            void write(Event &e)
            {
                std::stringstream ss;
                boost::archive::text_oarchive oa(ss);
                oa << e;
                std::string s(ss.str());
                boost::asio::write(*socket_, boost::asio::buffer(s, s.length()));
                /*
                 *here we should wait for the result

                 只有返回结果之后，我们才能够继续运行程序
                 */
            }

        private:
            boost::shared_ptr<tcp::socket> socket_;
    };

}

#endif
