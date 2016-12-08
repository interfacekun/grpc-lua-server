#include "zlib.h"
#include "UdpGzip.h"
//Bytef 就是 unsigned char
//uLong 就是 unsigned long

/* Compress gzip data */
/* data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度 */
int gzcompress(Bytef *data, uLong ndata,
               Bytef *zdata, uLong *nzdata)
{
    z_stream c_stream;
    int err = 0;

    if(data && ndata > 0) {
        c_stream.zalloc = nullptr;
        c_stream.zfree = nullptr;
        c_stream.opaque = nullptr;
        //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
        if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                        MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
        c_stream.next_in  = data;
        c_stream.avail_in  = ndata;
        c_stream.next_out = zdata;
        c_stream.avail_out  = *nzdata;
        while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
            if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
        }
        if(c_stream.avail_in != 0) return c_stream.avail_in;
        for(;;) {
            if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
            if(err != Z_OK) return -1;
        }
        if(deflateEnd(&c_stream) != Z_OK) return -1;
        *nzdata = c_stream.total_out;
        return 0;
    }
    return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzdecompress(Byte *zdata, uLong nzdata,
                 Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = nullptr;
    d_stream.zfree = nullptr;
    d_stream.opaque = nullptr;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
    while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK) {
            if(err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
                    return -1;
                }
            } else return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}


namespace ak {

     std::string GZIP::Compress(std::string s){
         if (s.length() ==0) {
             return "";
         }

         char *_compress_buff=new char[s.length()*10];
         if (_compress_buff ==NULL) {
             return "";
         }
         uLong len_compress = s.length()*10;
         gzcompress((Bytef *)s.c_str(), (uLong)s.length(),
                     (Bytef *)_compress_buff, &len_compress);

         std::string ret((const char *)_compress_buff,len_compress);
         delete []_compress_buff;
         _compress_buff=NULL;
         return ret;
     }

     std::string GZIP::Decompress(std::string s){
        if (s.length() ==0) {
            return "";
        }
        char *_uncompress_buff=new char[s.length()*100];
        if (_uncompress_buff ==NULL) {
            return "";
        }
        uLong len_uncompress = s.length()*100;
        gzdecompress((Byte *)s.c_str(),(uLong)s.length(),(Byte *)_uncompress_buff,&len_uncompress);
        std::string ret((const char *)_uncompress_buff,len_uncompress);
        delete []_uncompress_buff;
        _uncompress_buff=NULL;
        return ret;
    }
}
