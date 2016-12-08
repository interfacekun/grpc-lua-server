#ifndef _AK_NET_UDPMESSAGE_H_
#define _AK_NET_UDPMESSAGE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#ifdef WINDOWS
#include <Winsock2.h>
#endif
#ifdef LINUX
#include <arpa/inet.h>
#endif

#ifdef ANDROID
#include <arpa/inet.h>
#endif
namespace ak {
    class UdpMessage {
    public:

        enum {
            _nHeadLength = 4
        };

        UdpMessage() : UdpMessage(0) {

        }

        UdpMessage(size_t bodyLength) {
            _nBodyLength = bodyLength;
            _data = new char[length()];
        }

        UdpMessage(std::uint32_t protoID, size_t bodyLength) {
            _nBodyLength = bodyLength;
            _data = new char[length()];

            _protoID = protoID;
        }

        ~UdpMessage() {
            if (_data != nullptr) {
                delete[] _data;
            }
        }

        const char *data() const {
            return _data;
        }

        char *data() {
            return _data;
        }

        std::size_t length() const {
            return _nHeadLength + _nBodyLength;
        }

        const char *body() const {
            return _data + _nHeadLength;
        }

        char *body() {
            return _data + _nHeadLength;
        }

        std::size_t body_length() const {
            return _nBodyLength;
        }

        void body_length(std::size_t new_length) {
            _nBodyLength = new_length;
        }

        std::uint32_t ProtoID() const {
            return _protoID;
        }

        void ProtoID(std::uint32_t new_proto) {
            _protoID = new_proto;
        }

        bool decode_header() {
            /*
             * decode will resize the data
             */
            std::memcpy(&_protoID, _data, _nHeadLength);
            _protoID = ntohl(_protoID);

            //delete [] _data;
            //_data = new char[length()];
            return true;
        }

        void encode_header() {
            // std::memcpy(_data, &_nBodyLength, _nHeadLength);

            std::memset(_data, 0, _nHeadLength);

            std::uint32_t tmp_protoID = htonl(_protoID);

            std::memcpy(_data, &tmp_protoID, _nHeadLength);
        }

    private:
        char *_data;
        std::uint32_t _nBodyLength;
        std::uint32_t _protoID;
    };

}

#endif // _AK_NET_MESSAGE_H_
