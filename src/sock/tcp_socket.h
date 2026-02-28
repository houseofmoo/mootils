#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <cstdint>
#include "socket_result.h"

namespace sock {
    #if defined(MOO_WIN32) 
        using socket_handle = std::uintptr_t;
    #elif defined(MOO_LINUX)
        constexpr std::int32_t INVALID_SOCKET = -1;
        using socket_handle = int;
    #endif

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

            [[nodiscard]] SockResult open();
            void shutdown() noexcept;
            void close() noexcept;

            // shared implementation
            [[nodiscard]] bool is_connected() const noexcept { 
                return m_connected && handle_valid(); 
            }

            void adopt(socket_handle handle, bool connected = true) noexcept {
                close();
                m_handle = handle;
                m_connected = connected;
            }

            void disconnect() noexcept {
                shutdown();
                close();
            }


        protected:
            bool handle_valid() const noexcept;
    };

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
            TCPClient(TCPClient&&) = delete;
            TCPClient& operator=(TCPClient&&) = delete;

            [[nodiscard]] SockResult connect(const char* ip, uint16_t port);
            [[nodiscard]] SockResult send(const void* data, const size_t size);
            [[nodiscard]] SockResult send_all(const void* data, const size_t size);
            [[nodiscard]] SockResult recv(void* data, const size_t size);
            [[nodiscard]] SockResult recv_all(void* data, const size_t size);

            [[nodiscard]] SockResult open_and_connect(const char* ip, uint16_t port) {
                const sock::SockResult open_err = open();
                if (open_err.code != SockErr::None) {
                    return open_err;
                }
                return connect(ip, port);
            }
    };

    class TCPServer final : public TCPSocket {
        public:
            TCPServer() = default;
            ~TCPServer() = default;

            TCPServer(const TCPServer&) = delete;
            TCPServer& operator=(const TCPServer&) = delete;
            TCPServer(TCPServer&&) = default;
            TCPServer& operator=(TCPServer&&) = default;

            [[nodiscard]] SockResult bind(uint16_t port, const char* ip = "0.0.0.0");
            [[nodiscard]] SockResult listen(int backlog = 0);
            [[nodiscard]] std::pair<std::shared_ptr<TCPClient>, SockResult> accept();

            [[nodiscard]] SockResult open_and_listen(uint16_t port, const char* ip = "0.0.0.0") {
                sock::SockResult result = open();
                if (!result.ok()) return result;

                result = bind(port, ip);
                if (!result.ok()) return result;

                return listen(0);
            }
            
            void request_stop() noexcept { close(); }
    };
}
