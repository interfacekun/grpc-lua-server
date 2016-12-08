#ifndef _AK_NET_UDP_GZIP_H_
#define _AK_NET_UDP_GZIP_H_

#include <string>

typedef unsigned char  Byte;
typedef unsigned char  Bytef;
typedef unsigned long  uLong;

int gzcompress(Bytef *data, uLong ndata,
               Bytef *zdata, uLong *nzdata);

int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata);

namespace ak {

    class GZIP {
        public:
            static std::string Compress(std::string);
            static std::string Decompress(std::string);
    };
}
#endif
