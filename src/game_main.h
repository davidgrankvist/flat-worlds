#ifndef game_main_h
#define game_main_h

#include "common.h"

typedef struct {
   void (*MakeDrawCall)(); 
   void (*ClearScreen)(float r, float g, float b, float a);
} Render;

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
    int (*GetMouseInputX)();
    int (*GetMouseInputY)();

    Render* render;
} Platform;

int GameMain(Platform* platform);

#endif
