#if defined(MOO_LINUX)

#include "mootils/sock/tcp_socket.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdint>
#include <cstddef>
#include <mutex>
#include <span>

namespace sock {
    TCPClient::TCPClient() : TCPSocket() {}
    TCPClient::~TCPClient() = default;

    SockResult TCPClient::connect(const char* ip, uint16_t port) {
        // pseudo-validate ip
        if (!ip || *ip == '\0') {
            return SockResult{ SockErr::InvalidIp, SockOp::Connect, 0, 0 };
        }

        // open socket
        const sock::SockResult open_err = open();
        if (open_err.code != SockErr::None) {
            return open_err;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);

        // parse IP
        if (::inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
            close();
            return SockResult{ SockErr::InvalidIp, SockOp::Connect, 0, 0 };
        }

        // connect
        if (::connect(m_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            const int err = errno;
            close();
            return SockResult{ map_err(err), SockOp::Connect, err, 0 };
        }

        m_connected = true;
        return SockResult{ SockErr::None, SockOp::Connect, 0, 0 };
    }

    SockResult TCPClient::send(const void* data, const std::size_t size) {
        if (!is_connected()) {
            return SockResult{ SockErr::NotConnected, SockOp::Send, 0, 0 };
        }

        if (data == nullptr) {
            return SockResult{ SockErr::InvalidArgument, SockOp::Send, 0, 0 };
        }

        if (size <= 0) {
            return SockResult{ SockErr::SizeZero, SockOp::Send, 0, 0 };
        }

        if (size > static_cast<std::size_t>(INT32_MAX)) {
            return SockResult{ SockErr::SizeTooLarge, SockOp::Send, 0, 0 };
        }

        // NOTE: may not always send all bytes, use send_all() for that
        ssize_t sent_bytes = 0;
        {
            std::lock_guard lock(m_send_mtx);
            sent_bytes = ::send(
                m_handle,
                data,
                size,
                MSG_NOSIGNAL
            );
        }

        if (sent_bytes > 0) {
            return SockResult{ SockErr::None, SockOp::Send, 0, static_cast<int>(sent_bytes) };
        }

        if (sent_bytes == 0) {
            m_connected = false;
            return SockResult{ SockErr::Closed, SockOp::Send, 0, 0 };
        }

        const int err = errno;
        if (is_fatal_send_err(err)) {
            m_connected = false;
        }

        return SockResult{ map_err(err), SockOp::Send, err, 0 };
    }

    SockResult TCPClient::send(const std::span<std::byte> blob) {
        return send(blob.data(), blob.size_bytes());
    }

    SockResult TCPClient::send_all(const void* data, const std::size_t size) {
        if (!is_connected()) {
            return SockResult{ SockErr::NotConnected, SockOp::Send, 0, 0 };
        }

        if (data == nullptr) {
            return SockResult{ SockErr::InvalidArgument, SockOp::Send, 0, 0 };
        }

        if (size <= 0) {
            return SockResult{ SockErr::SizeZero, SockOp::Send, 0, 0 };
        }

        if (size > static_cast<std::size_t>(INT32_MAX)) {
            return SockResult{ SockErr::SizeTooLarge, SockOp::Send, 0, 0 };
        }

        std::size_t total = 0;
        auto* ptr = static_cast<const char*>(data);

        std::lock_guard lock(m_send_mtx);
        while (total < size) {
            const std::size_t remaining = size - total;

            const ssize_t sent = ::send(
                m_handle,
                ptr + total,
                remaining,
                MSG_NOSIGNAL
            );

            if (sent > 0) {
                total += static_cast<std::size_t>(sent);
                continue;
            }

            if (sent == 0) {
                m_connected = false;
                return SockResult{ SockErr::Closed, SockOp::Send, 0, static_cast<int>(total) };
            }

            const int err = errno;
            if (err == EINTR) {
                continue; // retry
            }

            if (is_fatal_send_err(err)) {
                m_connected = false;
            }

            return SockResult{ map_err(err), SockOp::Send, err, static_cast<int>(total) };
        }

        return SockResult{ SockErr::None, SockOp::Send, 0, static_cast<int>(total) };
    }

    SockResult TCPClient::send_all(const std::span<std::byte> blob) {
        return send_all(blob.data(), blob.size_bytes());
    }

    SockResult TCPClient::recv(void* data, const std::size_t size) {
        if (!is_connected()) {
            return SockResult{ SockErr::NotConnected, SockOp::Recv, 0, 0 };
        }

        if (data == nullptr) {
            return SockResult{ SockErr::InvalidArgument, SockOp::Recv, 0, 0 };
        }

        if (size <= 0) {
            return SockResult{ SockErr::SizeZero, SockOp::Recv, 0, 0 };
        }

        if (size > static_cast<std::size_t>(INT32_MAX)) {
            return SockResult{ SockErr::SizeTooLarge, SockOp::Recv, 0, 0 };
        }

        ssize_t recv_bytes = ::recv(
            m_handle,
            data,
            size,
            0
        );

        if (recv_bytes > 0) {
            return SockResult{ SockErr::None, SockOp::Recv, 0, static_cast<int>(recv_bytes) };
        }

        if (recv_bytes == 0) {
            // peer performed orderly shutdown
            m_connected = false;
            return SockResult{ SockErr::Closed, SockOp::Recv, 0, 0 };
        }

        const int err = errno;
        if (is_fatal_recv_err(err)) {
            m_connected = false;
        }

        return SockResult{ map_err(err), SockOp::Recv, err, 0 };
    }

    SockResult TCPClient::recv(const std::span<std::byte> blob) {
        return recv(blob.data(), blob.size_bytes());
    }

    SockResult TCPClient::recv_all(void* data, const std::size_t size) {
        if (!is_connected()) {
            return SockResult{ SockErr::NotConnected, SockOp::Recv, 0, 0 };
        }

        if (data == nullptr) {
            return SockResult{ SockErr::InvalidArgument, SockOp::Recv, 0, 0 };
        }

        if (size == 0) {
            return SockResult{ SockErr::SizeZero, SockOp::Recv, 0, 0 };
        }

        if (size > static_cast<std::size_t>(INT32_MAX)) {
            return SockResult{ SockErr::SizeTooLarge, SockOp::Recv, 0, 0 };
        }

        auto* out = static_cast<char*>(data);
        std::size_t total = 0;

        while (total < size) {
            const ssize_t recv_bytes = ::recv(
                m_handle,
                out + total,
                size - total,
                0
            );

            if (recv_bytes > 0) {
                total += static_cast<std::size_t>(recv_bytes);
                continue;
            }

            if (recv_bytes == 0) {
                m_connected = false;
                return SockResult{
                    SockErr::Closed,
                    SockOp::Recv,
                    0,
                    static_cast<int>(total)
                };
            }

            const int err = errno;
            if (err == EINTR) {
                continue; // retry, we were interrupted
            }

            if (is_fatal_recv_err(err)) {
                m_connected = false;
            }

            return SockResult{
                map_err(err),
                SockOp::Recv,
                err,
                static_cast<int>(total)
            };
        }

        return SockResult{
            SockErr::None,
            SockOp::Recv,
            0,
            static_cast<int>(total)
        };
    }

    SockResult TCPClient::recv_all(const std::span<std::byte> blob) {
        return recv_all(blob.data(), blob.size_bytes());
    }
}
#endif
