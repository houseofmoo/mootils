#if defined(MOO_WIN32)
#include "mootils/sock/socket_result.hpp"
#include "windows_hdr.hpp"

namespace sock {
    SockErr map_err(int err) noexcept {
        switch (err) {
            // state
            case WSANOTINITIALISED: return SockErr::NotInitialized;
            case WSAENOTSOCK: return SockErr::InvalidHandle;
            case WSAEINVAL: return SockErr::InvalidArgument;
            case WSAENOTCONN: return SockErr::NotConnected;
            case WSAEISCONN: return SockErr::AlreadyConnected;
            case WSAESHUTDOWN: return SockErr::Shutdown;
            case WSAECONNRESET: return SockErr::Closed; // recv() == 0 hadnled seperately

            // bind
            case WSAEADDRINUSE: return SockErr::AddressInUse;
            case WSAEADDRNOTAVAIL: return SockErr::AddressNotAvailable;
            case WSAEACCES: return SockErr::PermissionDenied;

            // connect
            case WSAECONNREFUSED: return SockErr::ConnectionRefused;
            case WSAETIMEDOUT: return SockErr::TimedOut;
            case WSAEHOSTUNREACH: // fallthrough
            case WSAENETUNREACH: return SockErr::Unreachable;

            // resources
            case WSAENOBUFS: // fallthrough
            case WSAEMFILE: return SockErr::ResourceExhausted;

            // non-blocking
            case WSAEWOULDBLOCK: return SockErr::WouldBlock;

            default: return SockErr::Unknown;
        }
    }

    bool is_fatal_send_err(int e) noexcept {
        switch (e) {
            case WSAECONNRESET:
            case WSAENOTCONN:
            case WSAESHUTDOWN:
            case WSAECONNABORTED:
            case WSAEHOSTUNREACH:
            case WSAENETDOWN:
            case WSAENETRESET:
            case WSAENETUNREACH:
                return true;
            default:
                return false;
        }
    }

    bool is_fatal_recv_err(int e) noexcept {
        switch (e) {
            case WSAECONNRESET:     // peer reset connection
            case WSAECONNABORTED:   // connection aborted locally/stack
            case WSAENOTCONN:       // socket is not connected
            case WSAESHUTDOWN:      // socket has been shut down
            case WSAENETDOWN:       // network subsystem failed
            case WSAENETRESET:      // connection broken due to keep-alive/network reset
            case WSAENETUNREACH:    // network unreachable
            case WSAEHOSTUNREACH:   // host unreachable
            case WSAETIMEDOUT:      // connection timed out
                return true;

            default:
                return false;
        }
    }
}
#endif