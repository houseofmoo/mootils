#if defined(MOO_WIN32)
#include "mootils/sock/tcp_socket.hpp"
#include "windows_hdr.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include <span>

namespace sock {
    static SOCKET as_native(socket_handle handle) noexcept {
        return static_cast<SOCKET>(handle);
    }

    // static socket_handle from_native(SOCKET handle) noexcept {
    //     return static_cast<socket_handle>(handle);
    // }

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
        addr.sin_port = htons(port);

        // parse IP
        if (::inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
            close();
            return SockResult{ SockErr::InvalidIp, SockOp::Connect, 0, 0 };
        }

        // connect
        if (::connect(as_native(m_handle), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
            int err = ::WSAGetLastError();
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
        int sent_bytes = 0;
        {
            std::lock_guard lock(m_send_mtx);
            sent_bytes = ::send(
                as_native(m_handle),
                static_cast<const char*>(data),
                static_cast<int>(size),
                0
            );
        }

        if (sent_bytes > 0) {
            return SockResult{ SockErr::None, SockOp::Send, 0, sent_bytes };
        }

        if (sent_bytes == 0) {
            m_connected = false;
            return SockResult{ SockErr::Closed, SockOp::Send, 0, 0 };
        }

        int err = ::WSAGetLastError();
        if (err == WSAECONNRESET) {
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
            int to_send = static_cast<int>(size - total);
            int sent_bytes = ::send(as_native(m_handle), ptr + total, to_send, 0);

            if (sent_bytes > 0) {
                total += static_cast<std::size_t>(sent_bytes);
                continue;
            }

            if (sent_bytes == 0) {
                m_connected = false;
                return SockResult{ SockErr::Closed, SockOp::Send, 0, static_cast<int>(total) };
            }

            int err = ::WSAGetLastError();
            if (err == WSAEINTR) {
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

        int recv_bytes = ::recv(as_native(m_handle), static_cast<char*>(data), static_cast<int>(size), 0);
        if (recv_bytes > 0) {
            return SockResult{ SockErr::None, SockOp::Recv, 0, recv_bytes };
        }

        if (recv_bytes == 0) {
            // peer performed orderly shutdown
            m_connected = false;
            return SockResult{ SockErr::Closed, SockOp::Recv, 0, 0 };
        }

        int err = ::WSAGetLastError();
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

        char* out = static_cast<char*>(data);
        std::size_t total = 0;

        while (total < size) {
            int recv_bytes = ::recv(
                as_native(m_handle),
                out + total,
                static_cast<int>(size - total),
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

            int err = ::WSAGetLastError();
            if (err == WSAEINTR) {
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