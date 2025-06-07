#ifndef input_h
#define input_h

#include "libgame.h"

// call at beginning of platform ProcessInput implementation
void UpdateInputBuffers();
// call these when input events are received
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);
void SetMousePosition(int x, int y);
void SetMouseDown(InputMouseButton btn);
void SetMouseUp(InputMouseButton btn);
// call after mouse enters the window and the position has been set
void SetMouseEnteredWindow();

#endif
