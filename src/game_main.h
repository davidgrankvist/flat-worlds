/*
 * This header defines the platform API and game entrypoint.
 *
 * The platform specific "actual main" sets up the function pointers
 * and passes them into GameMain where the game code runs.
 */

#ifndef game_main_h
#define game_main_h

#include "common.h"

// render part of platform API
typedef struct {
    void (*MakeDrawCall)(); 
    void (*ClearScreen)(Color color);
    // draw a 2D triangle using screen coordinates
    void (*DrawTriangle)(Vec2 a, Vec2 b, Vec2 c, Color color);
} Render;

// platform API
typedef struct {
    void (*InitWindow)();
    bool (*IsWindowOpen)();
    void (*CloseCurrentWindow)();

    void (*InitConsole)();

    void (*ProcessInput)();
    bool (*IsKeyDown)(InputKey key);
    bool (*IsKeyPressed)(InputKey key);
    bool (*IsKeyReleased)(InputKey key);
    bool (*IsMouseDown)(InputMouseButton btn);
    bool (*IsMousePressed)(InputMouseButton btn);
    bool (*IsMouseReleased)(InputMouseButton btn);
    // mouse position in screen coordinates
    int (*GetMouseInputX)();
    int (*GetMouseInputY)();

    Render* render;
} Platform;

// game code entrypoint
int GameMain(Platform* platform);

#endif
