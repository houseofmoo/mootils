#if defined(MOO_LINUX)

#include "mootils/sock/socket_result.hpp"
#include <errno.h>

namespace sock {

    SockErr map_err(int err) noexcept {
        switch (err) {
            // state
            case ENOTSOCK:   return SockErr::InvalidHandle;
            case EINVAL:     return SockErr::InvalidArgument;
            case ENOTCONN:   return SockErr::NotConnected;
            case EISCONN:    return SockErr::AlreadyConnected;
            case ESHUTDOWN:  return SockErr::Shutdown;
            case ECONNRESET: return SockErr::Closed; // recv()==0 handled separately

            // bind
            case EADDRINUSE:     return SockErr::AddressInUse;
            case EADDRNOTAVAIL: return SockErr::AddressNotAvailable;
            case EACCES:        return SockErr::PermissionDenied;

            // connect
            case ECONNREFUSED: return SockErr::ConnectionRefused;
            case ETIMEDOUT:    return SockErr::TimedOut;
            case EHOSTUNREACH: // fallthrough
            case ENETUNREACH:  return SockErr::Unreachable;

            // resources
            case ENOBUFS: // fallthrough
            case EMFILE:  // fallthrough
            case ENFILE:  return SockErr::ResourceExhausted;

            // non-blocking
            case EWOULDBLOCK: return SockErr::WouldBlock; // aka EAGAIN

            default:
                return SockErr::Unknown;
        }
    }

    bool is_fatal_send_err(int e) noexcept {
        switch (e) {
            case ECONNRESET:
            case ENOTCONN:
            case ESHUTDOWN:
            case ECONNABORTED:
            case EHOSTUNREACH:
            case ENETDOWN:
            case ENETRESET:
            case ENETUNREACH:
                return true;
            default:
                return false;
        }
    }


    bool is_fatal_recv_err(int e) noexcept {
        switch (e) {
            case ECONNRESET:     // peer reset connection
            case ECONNABORTED:   // connection aborted locally/stack
            case ENOTCONN:       // socket is not connected
            case ESHUTDOWN:      // socket has been shut down
            case ENETDOWN:       // network subsystem failed
            case ENETRESET:      // connection broken due to keep-alive/network reset
            case ENETUNREACH:    // network unreachable
            case EHOSTUNREACH:   // host unreachable
            case ETIMEDOUT:      // connection timed out
                return true;

            default:
                return false;
        }
    }

}

#endif
