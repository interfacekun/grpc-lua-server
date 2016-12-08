#ifndef _AK_PLATFORM_H_
#define _AK_PLATFORM_H_

#include <iostream>
#include <string>
#include <string.h>
#include <cassert>
#include <cstdio>
#include <cerrno>


//#define LogDebug(M, ...)   __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
//#define LogDebug2(M, ...)  __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
//#define LogReleaseDump()   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"" )
//#define LogInfo(M, ...)    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,M, __VA_ARGS__)
//#define clean_errno() (errno == 0 ? "None" : strerror(errno))
//#define LogWarn(M, ...)    __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
//#define LogError(M, ...)   __android_log_print(ANDROID_LOG_ERROR, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
#if defined(ANDROID) || defined(_AK_ANDROID_)
    #include <android/log.h>
    #define TP_STR_HELPER(x) #x
    #define TP_STR(x) TP_STR_HELPER(x)

        #define LogDebug(M, ...) __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
//        #define LogDebug2(M, ...) __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
        #define LogReleaseDump()
        #define LogInfo(M, ...)
        #define clean_errno()
        #define LogWarn(M, ...)
        #define LogError(M, ...) __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
        #define LogTest(M, ...) __android_log_print(ANDROID_LOG_DEBUG, "BLAH", "%s:%s " M "\n", __PRETTY_FUNCTION__, TP_STR(__LINE__), ##__VA_ARGS__)
        #define LogBenchmark(M, ...) fprintf(stdout, "[BENCHMARK] " M " (%s:%d) \n", ##__VA_ARGS__, __FILE__, __LINE__)


#else

    #ifndef NDEBUG
        #define LogDebug(M, ...) fprintf(stdout, "[DEBUG] " M " (%s:%d) \n", ##__VA_ARGS__, __FILE__, __LINE__)
//        #define LogDebug2(M, ...) fprintf(stdout, "[DEBUG] (%s:%d)" M " \n", __FILE__, __LINE__, ##__VA_ARGS__)
        #define LogBenchmark(M, ...) fprintf(stdout, "[BENCHMARK] " M " (%s:%d) \n", ##__VA_ARGS__, __FILE__, __LINE__)
        #define LogReleaseDump() fprintf(stdout, "[RELEASE DUMP] %s (%s:%d) \n", typeid(this).name()  , __FILE__, __LINE__)
    #else
        #define LogDebug(M, ...)
//        #define LogDebug2(M, ...)
        #define LogReleaseDump()
        #define LogBenchmark(M, ...)
    #endif

    #ifdef SERVER
        #define LogInfo(M, ...) fprintf(stdout, "[Info] " M " (%s:%d) \n", ##__VA_ARGS__, __FILE__, __LINE__)
    #else
        #define LogInfo(M, ...)
    #endif

    #define clean_errno() (errno == 0 ? "None" : strerror(errno))
    #define LogWarn(M, ...) fprintf(stdout, "[WARN] " M " (%s:%d: errno: %s)\n", ##__VA_ARGS__, __FILE__, __LINE__, clean_errno())
    #define LogError(M, ...) fprintf(stderr, "[Error] " M " (%s:%d: errno: %s)\n", ##__VA_ARGS__, __FILE__, __LINE__, clean_errno())
    #define LogTest(M, ...)
#endif



/// when define returns true it means that our architecture uses big endian
#define HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100)
#define SWAP64(i)  ((i & 0x00000000000000ff) << 56 | (i & 0x000000000000ff00) << 40 | (i & 0x0000000000ff0000) << 24 | (i & 0x00000000ff000000) << 8 | (i & 0xff00000000000000) >> 56 | (i & 0x00ff000000000000) >> 40 | (i & 0x0000ff0000000000) >> 24 | (i & 0x000000ff00000000) >> 8)
#define SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)
#define SWAP_INT64_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? SWAP64(i) : (i) )
#define SWAP_INT32_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? SWAP32(i) : (i) )
#define SWAP_INT16_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? SWAP16(i) : (i) )
#define SWAP_INT64_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i) : SWAP64(i) )
#define SWAP_INT32_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i) : SWAP32(i) )
#define SWAP_INT16_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i):  SWAP16(i) )

#define SWAP_HOST_TO_BIG_INT64(i)  ((HOST_IS_BIG_ENDIAN == true)? (i):  SWAP64(i) )
#define SWAP_HOST_TO_BIG_INT32(i)  ((HOST_IS_BIG_ENDIAN == true)? (i):  SWAP32(i) )
#define SWAP_HOST_TO_BIG_INT16(i)  ((HOST_IS_BIG_ENDIAN == true)? (i):  SWAP16(i) )



inline int nextPOT(int x) {
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x + 1;
}

#ifndef MIN
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif  // MAX


#define PLATFORM_MOBILE 0
#define PLATFORM_DESKTOP 1

#if defined(NDEBUG) || defined(RELEASE) || defined(_AK_IOS_) || defined(_AK_ANDROID_)
    #define RES_MODE_PACK   1
#endif
#if defined(_AK_IOS_) || defined(_AK_ANDROID_)
    #define PLATFORM_DEVICE PLATFORM_MOBILE
#else
    #define PLATFORM_DEVICE PLATFORM_DESKTOP
#endif

#endif
