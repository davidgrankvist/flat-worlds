/*
 * Platform setup - input
 *
 * Updates input buffers to support public functions like IsKeyDown.
 */

#ifndef platform_input_h
#define platform_input_h

#include "libgame.h"

typedef struct {
    void (*ProcessInput)();
    void (*WarpMousePosition)(int x, int y);
} PlatformInput;

void InitPlatformInput(PlatformInput platformInput);

void UpdateInputBuffers(); // call at beginning of ProcessInput
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);
void SetMousePosition(int x, int y);
void SetMouseDown(InputMouseButton btn);
void SetMouseUp(InputMouseButton btn);
void SetMouseEnteredWindow(); // call after mouse enters the window and the position has been set

#endif
