#include "UdpClient.h"
#include "UdpGzip.h"
#include "platform.h"

using boost::asio::ip::udp;

namespace ak {

    UdpClient::UdpClient(std::string ip, std::string port,bool need_new_thread) : _hasError(false) {

        _ioservice = std::unique_ptr<boost::asio::io_service>(new boost::asio::io_service());
        _socket = std::unique_ptr<udp::socket>(new udp::socket(*_ioservice));
        udp::resolver resolver(*_ioservice);
        auto endpoint_iterator = resolver.resolve({ip, port});

        do_connect(endpoint_iterator);

        _thread=NULL;
        if(need_new_thread){
            _thread = std::unique_ptr<std::thread>(new std::thread([this]() { _ioservice->run(); }));
        }
    }

    void UdpClient::nonBlockingEventLoop(){
        if (_thread != NULL) {
            return;
        }
        _ioservice->poll();
    }

    void UdpClient::write(UdpMessage *msg) {

        _ioservice->post(
                [this, msg]() {
                    msg->encode_header();
                    bool write_in_progress = !_write_messages.empty();
                    _write_messages.push_back(msg);
                    if (!write_in_progress) {
                        do_write();
                    }
                });
    }

    void UdpClient::poll(std::function<void(const UdpMessage *, const boost::system::error_code *)> fn) {
        std::unique_lock<std::mutex> l(_read_mutex);
        if (_hasError) {
            fn(nullptr, &_ec);
        } else {
            while (!_read_messages.empty()) {

                UdpMessage *msg = _read_messages.front();
                fn(msg, nullptr);
                _read_messages.pop_front();
                delete msg;
            }
        }
    }

    void UdpClient::close() {
        _ioservice->post([this]() { _socket->close(); });
        if (_thread != NULL) {
            _thread->join();
            _ioservice->stop();
        }
        _nState = ConnectionState::DISCONNECTED;
    }

    void UdpClient::do_connect(udp::resolver::iterator endpoint_iterator) {
        boost::asio::async_connect(*_socket, endpoint_iterator,
                                   [this](boost::system::error_code ec, udp::resolver::iterator) {
                                       if (!ec) {
                                           do_read_body();

                                       } else {
                                           LogDebug("UdpClient::do_connect failed %d, %s", ec.value(), ec.message().c_str());
                                           _hasError = true;
                                           _ec = ec;
                                           _socket->close();
                                           _nState = ConnectionState::DISCONNECTED;
                                       }
                                   });
    }

    void UdpClient::do_read_body() {
        _socket->async_receive(boost::asio::buffer(_read_buff, sizeof(_read_buff)),
                                [this](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        //解压缩

                                        uLong len_uncompress = sizeof(_uncompress_buff);
                                        gzdecompress((Byte *)_read_buff, (uLong)length,
                                                    (Byte *)_uncompress_buff, &len_uncompress);
                                        _read = nullptr;
                                        _read = std::unique_ptr<UdpMessage>(new UdpMessage(len_uncompress-ak::UdpMessage::_nHeadLength));

                                        memcpy(_read->data(), _uncompress_buff, len_uncompress);
                                        _read->decode_header();
                                        //_read->body_length(length - ak::UdpMessage::_nHeadLength);

                                        std::unique_lock<std::mutex> l2(_read_mutex);

                                        auto msg = _read.release();
                                        _read_messages.push_back(msg);
                                        do_read_body();
                                        _nState = ConnectionState::CONNECTED;
                                    }
                                    else {
                                        _hasError = true;
                                        _ec = ec;
                                        _socket->close();
                                        _nState = ConnectionState::DISCONNECTED;
                                    }
                                });
    }

    void UdpClient::do_write() {
        auto msg = _write_messages.front();
        uLong len_compress = sizeof(_compress_buff);
        gzcompress((Bytef *)msg->data(), (uLong)msg->length(),
                    (Bytef *)_compress_buff, &len_compress);

        _socket->async_send(boost::asio::buffer(_compress_buff, len_compress),
                             [this](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (!ec) {
                                     UdpMessage *msg = _write_messages.front();
                                     _write_messages.pop_front();
                                     delete msg;
                                     if (!_write_messages.empty()) {
                                         do_write();
                                     }
                                 }
                                 else {
                                     LogDebug("UdpClient::do_write failed %d %s", ec.value(), ec.message().c_str());
                                     _hasError = true;
                                     _ec = ec;
                                     _socket->close();
                                     _nState = ConnectionState::DISCONNECTED;
                                 }
                             });
    }
}
