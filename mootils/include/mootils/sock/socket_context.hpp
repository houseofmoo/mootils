#pragma once
#include "mootils/api.hpp"

namespace sock {
    // use RAII to control lifetime of socket initialization
    class SocketContext {
        private:
            bool m_ok = false;

        public:
            MOOTILS_API SocketContext();
            MOOTILS_API ~SocketContext();

            SocketContext(const SocketContext&) = delete;
            SocketContext& operator=(const SocketContext&) = delete;
            SocketContext(SocketContext&&) = delete;
            SocketContext& operator=(SocketContext&&) = delete;

            MOOTILS_API [[nodiscard]] bool ok() const noexcept;
    };
}