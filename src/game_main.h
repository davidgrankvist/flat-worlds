#ifndef game_main_h
#define game_main_h

#include "common.h"

typedef struct {
   void (*MakeDrawCall)(); 
   void (*ClearScreen)(float r, float g, float b, float a);
} Render;

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
    // dummy
    KeyUnknown, // 64 codes, so they fit into a u64 for now
} InputKey;

typedef struct {
    void (*InitWindow)();
    bool (*IsWindowOpen)();
    void (*CloseCurrentWindow)();

    void (*InitConsole)();

    void (*ProcessInput)();
    bool (*IsKeyDown)(InputKey key);
    bool (*IsKeyPressed)(InputKey key);
    bool (*IsKeyReleased)(InputKey key);

    Render* render;
} Platform;

int GameMain(Platform* platform);

#endif
