#if defined(MOO_LINUX)
#include "sock/socket_context.h"
        #include <signal.h>

namespace sock {
    SocketContext::SocketContext() : m_ok(true) {
        // prevent terminanting processes when a send()
        // on closed socket occurs
        //signal(SIGPIPE, SIG_IGN);

        // disabled this to not pollute the application as a whole,
        // same result adding MSG_NOSIGNAL to send() calls
        // which we do for linux sockets
    }

    SocketContext::~SocketContext() {
        m_ok = false;
    }

    bool SocketContext::ok() const noexcept {
        return m_ok;
    }
}
#endif
