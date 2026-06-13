#if defined(MOO_WIN32)
#include "mootils/sock/tcp_socket.hpp"
#include "windows_hdr.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <utility>

namespace sock {
    static SOCKET as_native(socket_handle handle) noexcept {
        return static_cast<SOCKET>(handle);
    }

    static socket_handle from_native(SOCKET handle) noexcept {
        return static_cast<socket_handle>(handle);
    }

    [[nodiscard]] SockResult TCPServer::open_and_listen(uint16_t port, const char* ip, std::int32_t backlog) {
        sock::SockResult result = open();
        if (!result.ok()) {
            return result;
        }

        result = bind(port, ip);
        if (!result.ok()) {
            close();
            return result;
        }

        result = listen(backlog);
        if (!result.ok()) {
            close();
            return result;
        }

        return result;
    }

    [[nodiscard]] std::pair<std::shared_ptr<TCPClient>, SockResult> TCPServer::accept() {
        SockResult result{ SockErr::Unknown, SockOp::Accept, 0, 0 };

        if (!is_handle_valid()) {
            result.code = SockErr::InvalidHandle;
            return { nullptr, result };
        }

        sockaddr_in conn{};
        int conn_size = sizeof(conn);

        SOCKET sock = ::accept(as_native(m_handle), reinterpret_cast<sockaddr*>(&conn), &conn_size);
        if (sock == INVALID_SOCKET) {
            result.sys_error = ::WSAGetLastError();
            if (result.sys_error == WSAEINVAL || result.sys_error == WSAENOTSOCK) {
                result.code = SockErr::Closed;
            } else {
                result.code = map_err(result.sys_error);
            }
            return { nullptr, result };
        }

        auto client = std::make_shared<TCPClient>();
        client->adopt(from_native(sock), true);

        result.code = SockErr::None;
        return { client, result };
    }

    void TCPServer::request_stop() noexcept {
        close();
    }

    SockResult TCPServer::bind(uint16_t port, const char* ip) {
        if (!is_handle_valid()) {
            return SockResult { SockErr::InvalidHandle, SockOp::Bind, 0, 0 };
        }

        // pseudo-validate ip
        if (!ip || *ip == '\0') {
            return SockResult{ SockErr::InvalidIp, SockOp::Bind, 0, 0 };
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        if (::inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
            return SockResult{ SockErr::InvalidIp, SockOp::Bind, 0, 0 };
        }

        BOOL exclusive = TRUE;
        if (::setsockopt(
            as_native(m_handle),
            SOL_SOCKET,
            SO_EXCLUSIVEADDRUSE, // prevent others from binding to our port while we're bound
            reinterpret_cast<const char*>(&exclusive),
            sizeof(exclusive)) != 0) {
                int err = ::WSAGetLastError();
                return SockResult{ map_err(err), SockOp::Bind, err, 0 };
        }

        if (::bind(as_native(m_handle), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            int err = ::WSAGetLastError();
            return SockResult{ map_err(err), SockOp::Bind, err, 0 };
        }

        return SockResult{ SockErr::None, SockOp::Bind, 0, 0 };
    }

    SockResult TCPServer::listen(int backlog) {
        if (!is_handle_valid()) {
            return SockResult{ SockErr::InvalidHandle, SockOp::Listen, 0, 0 };
        }

        // backlog of 0 is interpreted as maximum backlog size
        if (backlog <= 0) backlog = SOMAXCONN;

        if (::listen(as_native(m_handle), backlog) != 0) {
            int err = ::WSAGetLastError();
            return SockResult{ map_err(err), SockOp::Listen, err, 0 };
        }

        return SockResult{ SockErr::None, SockOp::Listen, 0, 0 };
    }
}
#endif
