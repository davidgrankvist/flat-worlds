#ifndef game_state_h
#define game_state_h

#include "libgame.h"

typedef struct {
    Camera3D camera;
    Camera3D startingCamera;

    // -- Triangles --

    Vec3 aRef;
    Vec3 bRef;
    Vec3 cRef;

    Vec3 aRef2;
    Vec3 bRef2;
    Vec3 cRef2;

    Vec3 aRef3;
    Vec3 bRef3;
    Vec3 cRef3;
} GameState;

typedef void (*GameUpdateFunc)(float deltaTime, GameState* gameState, Platform* platform);

#endif

