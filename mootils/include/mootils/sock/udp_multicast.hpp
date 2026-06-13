#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include "socket_result.hpp"
#include "socket_defs.hpp"
#include "mootils/api.hpp"

namespace sock {
    struct UdpMcastConfig {
        std::string group_ip = "239.255.0.1"; // admin scope...which may not work
        uint16_t port = 30001;
        std::string bind_ip = "0.0.0.0"; // INADDR_ANY
        int ttl = 1;
        bool loopback = true;
        bool reuse_addr = true;
    };

    class UDPMulticastSocket final {
        private:
            socket_handle m_handle;
            bool m_open;
            bool m_joined;
            UdpMcastConfig m_cfg{};

        public:
            MOOTILS_API UDPMulticastSocket();
            MOOTILS_API ~UDPMulticastSocket();
            UDPMulticastSocket(const UDPMulticastSocket&) = delete;
            UDPMulticastSocket& operator=(const UDPMulticastSocket&) = delete;
            MOOTILS_API UDPMulticastSocket(UDPMulticastSocket&&) noexcept;
            MOOTILS_API UDPMulticastSocket& operator=(UDPMulticastSocket&&) noexcept;

            MOOTILS_API [[nodiscard]] SockResult open_and_join(const UdpMcastConfig& cfg);
            MOOTILS_API [[nodiscard]] SockResult send_broadcast(const void* data,std::size_t size) noexcept;
            MOOTILS_API [[nodiscard]] SockResult recv_broadcast(void* data,std::size_t size) noexcept;
            MOOTILS_API void close() noexcept;

            // shared implementation
            [[nodiscard]] bool is_open() const noexcept {
                return m_open && handle_valid();
            }

            void request_stop() noexcept {
                close(); // breaks blocking recvfrom
            }

        private:
            bool handle_valid() const noexcept;
    };
}
