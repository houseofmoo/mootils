#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <cstdint>
#include <cstddef>
#include "socket_result.hpp"
#include "socket_defs.hpp"

namespace sock {
    // raw socket handle wrapper. Use TCPClient or TCPServer instead of TCPSocket directly
    class TCPSocket {
        protected:
            socket_handle m_handle;
            bool m_connected;

        public:
            TCPSocket();
            ~TCPSocket();

            TCPSocket(const TCPSocket&) = delete;
            TCPSocket& operator=(const TCPSocket&) = delete;
            TCPSocket(TCPSocket&&) noexcept;
            TCPSocket& operator=(TCPSocket&&) noexcept;

            void disconnect() noexcept;
            void adopt(socket_handle handle, bool connected = true) noexcept;
            [[nodiscard]] bool is_connected() const noexcept;

        protected:
            [[nodiscard]] SockResult open();
            bool is_handle_valid() const noexcept;
            void shutdown() noexcept;
            void close() noexcept;
    };

    // TCPClient has thread safe sends but thread UNSAFE recvs
    class TCPClient final : public TCPSocket {
        private:
            // to prevent any chance of a send being interrupted and data arriving at
            // destination out of order, we lock on sends. We do not need to lock
            // on recvs since only 1 thread listens to each sockets incoming messages
            std::mutex m_send_mtx;

        public:
            TCPClient();
            ~TCPClient() = default;

            TCPClient(const TCPClient&) = delete;
            TCPClient& operator=(const TCPClient&) = delete;
            TCPClient(TCPClient&&) noexcept = delete;
            TCPClient& operator=(TCPClient&&) noexcept = delete;

            [[nodiscard]] SockResult connect(const char* ip, uint16_t port);
            [[nodiscard]] SockResult send(const void* data, const std::size_t size);
            [[nodiscard]] SockResult send_all(const void* data, const std::size_t size);
            [[nodiscard]] SockResult recv(void* data, const std::size_t size);
            [[nodiscard]] SockResult recv_all(void* data, const std::size_t size);
    };

    // TCPServer will return std::shared_ptr<TCPClient> from accept()
    class TCPServer final : public TCPSocket {
        public:
            TCPServer() = default;
            ~TCPServer() = default;

            TCPServer(const TCPServer&) = delete;
            TCPServer& operator=(const TCPServer&) = delete;
            TCPServer(TCPServer&&) noexcept = delete;
            TCPServer& operator=(TCPServer&&) noexcept = delete;

            [[nodiscard]] SockResult open_and_listen(uint16_t port, const char* ip = "0.0.0.0", std::int32_t backlog = 16);
            [[nodiscard]] std::pair<std::shared_ptr<TCPClient>, SockResult> accept();
            void request_stop() noexcept;

        private:
            [[nodiscard]] SockResult bind(uint16_t port, const char* ip = "0.0.0.0");
            [[nodiscard]] SockResult listen(int backlog);

    };
}
