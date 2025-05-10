/*
 * Platform setup - input
 *
 * Update input buffers to support the public functions IsKeyDown etc.
 */

#ifndef platform_input_h
#define platform_input_h

#include "libgame.h"

void UpdateInputBuffers(); // call at beginning of ProcessInput
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);
void SetMousePosition(int x, int y);
void SetMouseDown(InputMouseButton btn);
void SetMouseUp(InputMouseButton btn);

#endif
