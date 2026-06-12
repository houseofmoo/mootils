#if defined(MOO_WIN32)
#include "mootils/sock/socket_context.hpp"
#include "windows_hdr.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>

namespace sock {
    SocketContext::SocketContext() {
        WSADATA wsa{};
        m_ok = WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    }

    SocketContext::~SocketContext() {
        if (m_ok) {
            WSACleanup();
        }
        m_ok = false;
    }

    bool SocketContext::ok() const noexcept {
        return m_ok;
    }
}
#endif