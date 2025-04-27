/*
 * This header defines/includes things that must be set up early in order for other headers
 * to work properly.
 *
 * For example, windows.h needs to be included before OpenGL headers.
 */
#ifndef platform_include_h
#define platform_include_h

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#endif
