/*
 * This header defines input related utilities that are platform independent.
 * The idea is to support the platform layer implementations with some shared code.
 *
 * Each platform layer is responsible for mapping to the common key code format.
 */

#ifndef input_h
#define input_h

#include "libgame.h"

// helpers to set up input state from the platform-specific code
void UpdateInputBuffers(); // call at beginning of ProcessInput
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);
void SetMousePosition(int x, int y);
void SetMouseDown(InputMouseButton btn);
void SetMouseUp(InputMouseButton btn);

// platform layer API
bool IsKeyDown(InputKey key);
bool IsKeyPressed(InputKey key);
bool IsKeyReleased(InputKey key);
bool IsMouseDown(InputMouseButton btn);
bool IsMousePressed(InputMouseButton btn);
bool IsMouseReleased(InputMouseButton btn);
int GetMouseInputX();
int GetMouseInputY();

#endif
