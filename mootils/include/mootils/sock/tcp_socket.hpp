#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <span>
#include "socket_result.hpp"
#include "socket_defs.hpp"
#include "mootils/api.hpp"

namespace sock {
    // raw socket handle wrapper. Use TCPClient or TCPServer instead of TCPSocket directly
    class TCPSocket {
        protected:
            socket_handle m_handle;
            bool m_connected;

        public:
            MOOTILS_API TCPSocket();
            MOOTILS_API ~TCPSocket();
            TCPSocket(const TCPSocket&) = delete;
            TCPSocket& operator=(const TCPSocket&) = delete;
            MOOTILS_API TCPSocket(TCPSocket&&) noexcept;
            MOOTILS_API TCPSocket& operator=(TCPSocket&&) noexcept;

            MOOTILS_API void disconnect() noexcept;
            MOOTILS_API void adopt(socket_handle handle, bool connected = true) noexcept;
            MOOTILS_API [[nodiscard]] bool is_connected() const noexcept;

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
            MOOTILS_API TCPClient();
            MOOTILS_API ~TCPClient();
            TCPClient(const TCPClient&) = delete;
            TCPClient& operator=(const TCPClient&) = delete;
            TCPClient(TCPClient&&) noexcept = delete;
            TCPClient& operator=(TCPClient&&) noexcept = delete;

            MOOTILS_API [[nodiscard]] SockResult connect(const char* ip, uint16_t port);
            MOOTILS_API [[nodiscard]] SockResult send(const void* data, const std::size_t size);
            MOOTILS_API [[nodiscard]] SockResult send(const std::span<std::byte> blob);
            MOOTILS_API [[nodiscard]] SockResult send_all(const void* data, const std::size_t size);
            MOOTILS_API [[nodiscard]] SockResult send_all(const std::span<std::byte> blob);
            MOOTILS_API [[nodiscard]] SockResult recv(void* data, const std::size_t size);
            MOOTILS_API [[nodiscard]] SockResult recv(const std::span<std::byte> blob);
            MOOTILS_API [[nodiscard]] SockResult recv_all(void* data, const std::size_t size);
            MOOTILS_API [[nodiscard]] SockResult recv_all(const std::span<std::byte> blob);
    };

    // TCPServer will return std::shared_ptr<TCPClient> from accept()
    class TCPServer final : public TCPSocket {
        public:
            MOOTILS_API TCPServer();
            MOOTILS_API ~TCPServer();
            TCPServer(const TCPServer&) = delete;
            TCPServer& operator=(const TCPServer&) = delete;
            TCPServer(TCPServer&&) noexcept = delete;
            TCPServer& operator=(TCPServer&&) noexcept = delete;

            MOOTILS_API [[nodiscard]] SockResult open_and_listen(uint16_t port, const char* ip = "0.0.0.0", std::int32_t backlog = 16);
            MOOTILS_API [[nodiscard]] std::pair<std::shared_ptr<TCPClient>, SockResult> accept();
            MOOTILS_API void request_stop() noexcept;

        private:
            [[nodiscard]] SockResult bind(uint16_t port, const char* ip = "0.0.0.0");
            [[nodiscard]] SockResult listen(int backlog);

    };
}
