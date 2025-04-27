/*
 * Public header - platform specific definitions.
 *
 * This is to make porting easier by having
 * platform specifics outside the libgame.h header.
 */

#ifndef libgame_platform_h
#define libgame_platform_h

// -- GameMain marker --

#ifdef _WIN32
    #define LIBGAME_EXPORT __declspec(dllexport)
#else
    #error "LIBGAME_EXPORT has not been defined for this platform."
#endif

#endif
