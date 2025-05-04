/*
 * Public header - platform specific definitions.
 *
 * This is to make porting easier by having
 * platform specifics outside the libgame.h header.
 */
#ifndef libgame_platform_h
#define libgame_platform_h

#ifdef _WIN32
    // setup exports
    #define LIBGAME_EXPORT __declspec(dllexport)

    // setup main
    LIBGAME_EXPORT void InitMainWin32();
#else
    #error "Unsupported platform."
#endif

#endif
