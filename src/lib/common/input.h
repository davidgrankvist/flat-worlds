/*
 * Internal input related utilities to modify the input state.
 *
 * The platform specific code is responsible for mapping key codes
 * and updating input buffers at the right time.
 */

#ifndef input_h
#define input_h

#include "libgame.h"

void UpdateInputBuffers(); // call at beginning of ProcessInput
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);
void SetMousePosition(int x, int y);
void SetMouseDown(InputMouseButton btn);
void SetMouseUp(InputMouseButton btn);

#endif
