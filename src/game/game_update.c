#include "game_update.h"

static void UpdateCamera(float deltaTime, Platform* platform, Camera3D* camera3D, Camera3D startingCamera);
static void DrawTriangles(GameState* gameState, Platform* platform);

void GameUpdate(float deltaTime, GameState* gameState, Platform* platform) {
    UpdateCamera(deltaTime, platform, &gameState->camera, gameState->startingCamera);
    DrawTriangles(gameState, platform);
}

static Color black = { 0, 0, 0, 1 };
static Color red = { 1, 0, 0, 1 };
static Color green = { 0, 1, 0, 1 };
static Color blue = { 0, 0, 1, 1 };

static float movementSpeed = 400;
static float rotationSpeed = 1;

static void UpdateCamera(float deltaTime, Platform* platform, Camera3D* camera, Camera3D startingCamera) {
    Input input = platform->input;
    Render render = platform->render;

    float movementStep = movementSpeed * deltaTime;
    float x = 0;
    float y = 0;
    float z = 0;

    float angleStep = rotationSpeed * deltaTime;
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    if (input.IsKeyPressed(KeyR)) {
        *camera = startingCamera;
    }
    if (input.IsKeyDown(KeyW)) {
        z += movementStep;
    }
    if (input.IsKeyDown(KeyS)) {
        z -= movementStep;
    }
    if (input.IsKeyDown(KeyA)) {
        x -= movementStep;
    }
    if (input.IsKeyDown(KeyD)) {
        x += movementStep;
    }
    if (input.IsKeyDown(KeyJ)) {
        y -= movementStep;
    }
    if (input.IsKeyDown(KeyK)) {
        y += movementStep;
    }

    if (input.IsKeyDown(KeyLeft)) {
        yaw += angleStep;
    }
    if (input.IsKeyDown(KeyRight)) {
        yaw -= angleStep;
    }
    if (input.IsKeyDown(KeyUp)) {
        pitch += angleStep;
    }
    if (input.IsKeyDown(KeyDown)) {
        pitch -= angleStep;
    }
    if (input.IsKeyDown(KeyE)) {
        roll += angleStep;
    }
    if (input.IsKeyDown(KeyQ)) {
        roll -= angleStep;
    }

    render.RotateCamera3D(camera, yaw, pitch, roll);

    // simplified movement for testing, not correct relative to rotation
    camera->position.x += x;
    camera->position.y += y;
    camera->position.z += z;
}

static void DrawTriangles(GameState* gameState, Platform* platform) {
    Render render = platform->render;

    render.ClearScreen(black);

    render.SetCamera3D(&gameState->camera);
    render.DrawTriangle3D(gameState->aRef, gameState->bRef, gameState->cRef, green);
    render.DrawTriangle3D(gameState->aRef2, gameState->bRef2, gameState->cRef2, blue);
    render.DrawTriangle3D(gameState->aRef3, gameState->bRef3, gameState->cRef3, red);

    render.MakeDrawCall();
    render.EndFrame();
}
