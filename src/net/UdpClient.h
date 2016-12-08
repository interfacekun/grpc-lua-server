#ifndef _AK_NET_UDP_CLIENT_H_
#define _AK_NET_UDP_CLIENT_H_

#include "UdpMessage.h"

#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <string>
#include <memory>
#include <mutex>
namespace ak { ;

    enum ConnectionState{
        INIT = 0,
        CONNECTING = 1,
        CONNECTED = 2,
        DISCONNECTED = 3
    };

    class UdpClient {
    public:
        UdpClient(std::string ip, std::string port,bool need_new_thread=true);

        void nonBlockingEventLoop();

        bool canPollMsg(){return !_read_messages.empty();};

        void write(UdpMessage *msg);

        void poll(std::function<void(const UdpMessage* , const boost::system::error_code* )> fn) ;

        void close();
        ConnectionState getConnectionState() const{
            return _nState;
        }
    private:
        void do_connect(boost::asio::ip::udp::resolver::iterator endpoint_iterator);

        void do_read_body();

        void do_write();

    private:
        std::unique_ptr<boost::asio::io_service> _ioservice;
        std::unique_ptr<boost::asio::ip::udp::socket> _socket;
        std::unique_ptr<std::thread> _thread;

        std::deque<UdpMessage *> _read_messages;
        std::deque<UdpMessage *> _write_messages;

        std::mutex _read_mutex;
        boost::system::error_code _ec;

        std::unique_ptr<UdpMessage> _read;
        unsigned char _read_buff[40960];
        unsigned char _compress_buff[40960];
        unsigned char _uncompress_buff[40960];

    private:
        bool _hasError;
        ConnectionState _nState;
    };

}
#endif
