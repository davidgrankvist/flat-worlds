/*
 * These are some common definitions that are needed in both the platform
 * and game code.
 */

#ifndef common_h
#define common_h

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// -- Utils --

void Assert(bool b, const char* format, ...);
void AssertFail(const char* format, ...);

// -- Math --

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

// row major 4x4 matrix
typedef struct {
    float m[4][4];
} Mat4;

// -- Input --

typedef enum {
    // letters
    KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH,
    KeyI, KeyJ, KeyK, KeyL, KeyM, KeyN, KeyO, KeyP,
    KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX,
    KeyY, KeyZ,
    // numbers
    Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
    // modifiers
    KeyLeftShift, KeyRightShift,
    KeyLeftCtrl, KeyRightCtrl,
    KeyLeftAlt, KeyRightAlt,
    // fn
    KeyF1, KeyF2, KeyF3, KeyF4, KeyF5, KeyF6, KeyF7, KeyF8,
    KeyF9, KeyF10, KeyF11, KeyF12,
    // arrows
    KeyLeft, KeyUp, KeyRight, KeyDown,
    // special
    KeySpace, KeyEnter, KeyBackspace, KeyTab, KeyEsc,
    // fallback value / checking number of codes
    KeyUnknown,
} InputKey;

typedef enum {
    MouseLeft,
    MouseRight,
    MouseMiddle,
    // fallback value / checking number of codes
    MouseUnknown,
} InputMouseButton;

// -- Graphics --

// normalized colors (0 to 1)
typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;

typedef struct {
   Vec2 origin; // bottom left
} Camera2D;

typedef struct {
    Vec3 position;
    Vec3 target;
    Vec3 up;
} Camera3D;

#endif
