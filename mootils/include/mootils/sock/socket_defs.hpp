#pragma once

namespace sock {
    #if defined(MOO_WIN32) 
        using socket_handle = std::uintptr_t;
    #elif defined(MOO_LINUX)
        constexpr std::int32_t INVALID_SOCKET = -1;
        using socket_handle = int;
    #endif
}