#ifndef game_state_h
#define game_state_h

#include "libgame.h"

typedef struct {
    Vec3 topLeft;
    Vec3 topRight;
    Vec3 bottomLeft;
    Vec3 bottomRight;
    Color color;
} Quad;

typedef struct {
    Camera3D camera;
    Camera3D startingCamera;
    bool orbitMode;

    Quad quads[3];
} GameState;

/*
 * Load both init and update dynamically, so that the game update code
 * owns both updates and resets.
 */
typedef void (*InitGameStateFunc)(GameState* gameState);
typedef void (*GameUpdateFunc)(float deltaTime, GameState* gameState, Platform* platform);

#endif

