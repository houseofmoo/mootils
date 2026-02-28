#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include "socket_result.h"

namespace sock {
    struct UdpMcastConfig {
        std::string group_ip = "239.255.0.1"; // admin scope...which may not work
        uint16_t port = 30001;
        std::string bind_ip = "0.0.0.0"; // INADDR_ANY
        int ttl = 1;
        bool loopback = true;
        bool reuse_addr = true;
    };

    #if defined(MOO_WIN32) 
        using socket_handle = std::uintptr_t;
    #elif defined(MOO_LINUX)
        constexpr std::int32_t INVALID_SOCKET = -1;
        using socket_handle = int;
    #endif

    class UDPMulticastSocket final {
        private:
            socket_handle m_handle;
            bool m_open;
            bool m_joined;
            UdpMcastConfig m_cfg{};

        public:
            UDPMulticastSocket();
            ~UDPMulticastSocket();

            UDPMulticastSocket(const UDPMulticastSocket&) = delete;
            UDPMulticastSocket& operator=(const UDPMulticastSocket&) = delete;
            UDPMulticastSocket(UDPMulticastSocket&&) noexcept;
            UDPMulticastSocket& operator=(UDPMulticastSocket&&) noexcept;

            [[nodiscard]] SockResult open_and_join(const UdpMcastConfig& cfg);
            [[nodiscard]] SockResult send_broadcast(const void* data, size_t size) noexcept;
            [[nodiscard]] SockResult recv_broadcast(void* data, size_t size) noexcept;
            void close() noexcept;

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
