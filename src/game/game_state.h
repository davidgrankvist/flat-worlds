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

#define MAX_PLANES 100

typedef enum {
    /*
     * Look around with mouse.
     * Move freely.
     */
    CameraFirstPerson,
    /*
     * Orbit around target.
     * Zoom in/out.
     */
    CameraOrbit,
    /*
     * Turn and zoom towards target.
     * Cycle through targets.
     */
    CameraFocus,
} CameraMode;

typedef struct {
    Camera3D camera;
    Camera3D startingCamera;
    CameraMode cameraMode;

    Quad planeQuads[MAX_PLANES];
    int planeCount;

    // camera focus animation
    int focusedPlane;
    Vec3 focusTargetStart;
    Vec3 focusTarget;
    Vec3 focusPositionStart;
    Vec3 focusPosition;
    float focusLerpT;
} GameState;

/*
 * Load both init and update dynamically, so that the game update code
 * owns both updates and resets.
 */
typedef void (*InitGameStateFunc)(GameState* gameState);
typedef void (*GameUpdateFunc)(float deltaTime, GameState* gameState);

#endif

