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
    void (*ProcessInput)();

    Render* render;
} Platform;

int GameMain(Platform* platform);

#endif
