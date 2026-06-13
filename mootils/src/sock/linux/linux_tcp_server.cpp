#if defined(MOO_LINUX)
#include "mootils/sock/tcp_socket.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <utility>

namespace sock {
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
        socklen_t conn_size = static_cast<socklen_t>(sizeof(conn));

        int fd = ::accept(m_handle, reinterpret_cast<sockaddr*>(&conn), &conn_size);
        if (fd < 0) {
            result.sys_error = errno;

            // When request_stop() closes the listen socket, accept commonly fails with EBADF.
            // Also treat ENOTSOCK as closed.
            if (result.sys_error == EBADF || result.sys_error == ENOTSOCK || result.sys_error == EINVAL) {
                result.code = SockErr::Closed;
            } else {
                result.code = map_err(result.sys_error);
            }
            return { nullptr, result };
        }

        auto client = std::make_shared<TCPClient>();
        client->adopt(fd, true);

        result.code = SockErr::None;
        return { client, result };
    }

    void TCPServer::request_stop() noexcept {
        close();
    }

    SockResult TCPServer::bind(uint16_t port, const char* ip) {
        if (!is_handle_valid()) {
            return SockResult{ SockErr::InvalidHandle, SockOp::Bind, 0, 0 };
        }

        // pseudo-validate ip
        if (!ip || *ip == '\0') {
            return SockResult{ SockErr::InvalidIp, SockOp::Bind, 0, 0 };
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);

        if (::inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
            return SockResult{ SockErr::InvalidIp, SockOp::Bind, 0, 0 };
        }

        int reuse = 1;
        if (::setsockopt(
                m_handle,
                SOL_SOCKET,
                SO_REUSEADDR,
                &reuse,
                static_cast<socklen_t>(sizeof(reuse))) != 0) {
            const int err = errno;
            return SockResult{ map_err(err), SockOp::Bind, err, 0 };
        }

        if (::bind(m_handle, reinterpret_cast<sockaddr*>(&addr), static_cast<socklen_t>(sizeof(addr))) != 0) {
            const int err = errno;
            return SockResult{ map_err(err), SockOp::Bind, err, 0 };
        }

        return SockResult{ SockErr::None, SockOp::Bind, 0, 0 };
    }

    SockResult TCPServer::listen(int backlog) {
        if (!is_handle_valid()) {
            return SockResult{ SockErr::InvalidHandle, SockOp::Listen, 0, 0 };
        }

        // backlog <= 0 is interpreted as maximum backlog size
        if (backlog <= 0) backlog = SOMAXCONN;

        if (::listen(m_handle, backlog) != 0) {
            const int err = errno;
            return SockResult{ map_err(err), SockOp::Listen, err, 0 };
        }

        return SockResult{ SockErr::None, SockOp::Listen, 0, 0 };
    }
}
#endif
