#pragma once

namespace sock {
    // use RAII to control lifetime of socket initialization
    class SocketContext {
        public:
            SocketContext();
            ~SocketContext();

            SocketContext(const SocketContext&) = delete;
            SocketContext& operator=(const SocketContext&) = delete;
            SocketContext(SocketContext&&) = delete;
            SocketContext& operator=(SocketContext&&) = delete;

            [[nodiscard]] bool ok() const noexcept;

        private:
            bool m_ok = false;
    };
}