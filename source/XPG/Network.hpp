/**
 *  Winsock and BSD are very similar but have just a few misaligned quirks. This
 *  file seeks to rectify that by granting low level control through a unified
 *  API.
 */

#ifndef XPGH_NETWORK
#define XPGH_NETWORK

#include "OperatingSystems.hpp"

#if defined(XPG_OS_WINDOWS)
#   include <winsock2.h>
    typedef int socklen_t;
#elif defined(XPG_OS_UNIX)
#   include <sys/socket.h>
#   include <sys/types.h>
#   include <netinet/in.h>
#   include <unistd.h>
#   define closesocket close
#   define INVALID_SOCKET -1
#   define SOCKET_ERROR -1
    typedef int SOCKET;
#else
#   error [XPG] unsupported network API
#endif

namespace XPG
{
    void NetworkStart();
    void NetworkEnd();
}

#endif
