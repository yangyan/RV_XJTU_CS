#ifndef SENDER_HPP
#define SENDER_HPP

#include  <sstream>

#include  <boost/asio.hpp>
#include  <boost/bind.hpp>
#include  <boost/archive/text_oarchive.hpp>
#include  <boost/archive/text_iarchive.hpp>

#include    "../event/Event.hpp"
#include    "../solution/Result.hpp"

using boost::asio::ip::tcp;

const int max_length = 1024;

namespace rv_xjtu_yangyan
{
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "23451"
    class Sender
    {
        public:
            Sender(std::string host, std::string port)
            {
                tcp::resolver resolver(io_service_);
                tcp::resolver::query query(tcp::v4(), host, port);
                iterator_ = resolver.resolve(query);

            }
            Sender()
            {
                std::string host = DEFAULT_HOST;
                std::string port = DEFAULT_PORT;
                tcp::resolver resolver(io_service_);
                tcp::resolver::query query(tcp::v4(), host, port);
                iterator_ = resolver.resolve(query);

            }
            void write(Event &e)
            {
                std::stringstream ss;
                boost::asio::ip::tcp::socket socket(io_service_);
                boost::asio::connect(socket, iterator_);

                boost::archive::text_oarchive oa(ss);
                oa << e;
                std::string s(ss.str());
                boost::asio::write(socket, boost::asio::buffer(s, s.length()));
                /*
                 *here we should wait for the result

                 只有返回结果之后，我们才能够继续运行程序
                 */
                std::stringstream iss;
                for(;;)
                {
                    char data[max_length];
                    boost::system::error_code error;
                    size_t length = socket.read_some(boost::asio::buffer(data), error);
                    if(error == boost::asio::error::eof)
                        break;
                    else if (error)
                        throw boost::system::system_error(error);
                    data[length] = 0;
                    iss << data;
                }
                boost::archive::text_iarchive ia(iss);
                Result result;
                ia >> result;
                result.print();
            }

        private:
            tcp::resolver::iterator iterator_;
            boost::asio::io_service io_service_;
    };

}

#endif
