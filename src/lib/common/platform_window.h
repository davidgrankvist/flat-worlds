/*
 * Platform setup - window.
 *
 * Set up platform specific window utilities
 * before calling public window functions.
 */
#ifndef platform_window_h
#define platform_window_h

typedef struct {
    void (*InitWindow)(const char* title);
    void (*InitConsole)();
} PlatformWindow;

void InitPlatformWindow(PlatformWindow platformWindow);

void SetResolution(int clientWidth, int clientHeight);

#endif
