#pragma once

#define CPP_2_C_EXPORT extern "C"

// if compiling a shared library on windows with msvc, export symbols
#if defined(_WIN32) && MOOTILS_AS_SHARED_LIB && defined(_MSC_VER)
    // #pragma message("MOOTILS_AS_WINDOWS_SHARED_LIB: true")
    #define MOOTILS_EXPORT __declspec(dllexport)
    #define MOOTILS_IMPORT __declspec(dllimport)
#else
    // #pragma message("MOOTILS_AS_WINDOWS_SHARED_LIB: false")
    #define MOOTILS_EXPORT
    #define MOOTILS_IMPORT
#endif

#if defined(MOOTILS_LIB_BUILD)
    // #pragma message("MOOTILS_EXPORT")
    #define MOOTILS_API MOOTILS_EXPORT
#else
    // #pragma message("MOOTILS_IMPORT")
    #define MOOTILS_API MOOTILS_IMPORT
#endif