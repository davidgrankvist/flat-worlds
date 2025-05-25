/*
 * Platform setup - input
 *
 * Initialize platform specifics before input utilities are called.
 */

#ifndef platform_input_h
#define platform_input_h

typedef struct {
    void (*ProcessInput)();
    void (*WarpMousePosition)(int x, int y);
} PlatformInput;

void InitPlatformInput(PlatformInput platformInput);

#endif
