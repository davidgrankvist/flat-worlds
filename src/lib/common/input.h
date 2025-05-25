/*
 * Input utilities to set for example IsKeyDown states.
 */

#ifndef input_h
#define input_h

#include "libgame.h"
#include "platform_input.h"

void UpdateInputBuffers(); // call at beginning of ProcessInput
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);
void SetMousePosition(int x, int y);
void SetMouseDown(InputMouseButton btn);
void SetMouseUp(InputMouseButton btn);
void SetMouseEnteredWindow(); // call after mouse enters the window and the position has been set

#endif
