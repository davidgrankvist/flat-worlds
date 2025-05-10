#include "game_update.h"

static void UpdateCamera(float deltaTime, GameState* gameState, Platform* platform);
static void DrawTriangles(GameState* gameState, Platform* platform);

void GameUpdate(float deltaTime, GameState* gameState, Platform* platform) {
    UpdateCamera(deltaTime, gameState, platform);
    DrawTriangles(gameState, platform);
}

static Color black = { 0, 0, 0, 1 };
static Color red = { 1, 0, 0, 1 };
static Color green = { 0, 1, 0, 1 };
static Color blue = { 0, 0, 1, 1 };

static float movementSpeed = 400;
static float rotationSpeed = 1;

static void UpdateCamera(float deltaTime, GameState* gameState, Platform* platform) {
    Input input = platform->input;
    Render render = platform->render;
    Camera3D* camera = &gameState->camera;

    float movementStep = movementSpeed * deltaTime;
    Vec3 offset = {0};

    float angleStep = rotationSpeed * deltaTime;
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    if (input.IsKeyPressed(KeyR)) {
        gameState->camera = gameState->startingCamera;
    }
    if (input.IsKeyDown(KeyW)) {
        offset.z += movementStep;
    }
    if (input.IsKeyDown(KeyS)) {
        offset.z -= movementStep;
    }
    if (input.IsKeyDown(KeyA)) {
        offset.x -= movementStep;
    }
    if (input.IsKeyDown(KeyD)) {
        offset.x += movementStep;
    }
    if (input.IsKeyDown(KeyJ)) {
        offset.y -= movementStep;
    }
    if (input.IsKeyDown(KeyK)) {
        offset.y += movementStep;
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

    if (input.IsKeyPressed(KeyO)) {
        gameState->orbitMode = !gameState->orbitMode;
    }

    if (gameState->orbitMode) {
        float azimuth = -yaw;
        float elevation = pitch;
        render.OrbitCameraAboutTarget(camera, azimuth, elevation);
        render.MoveCameraTowardsTarget(camera, offset.z);
    } else {
        render.RotateCameraFirstPerson(camera, yaw, pitch, roll);
        render.MoveCameraFirstPerson(camera, offset);
    }
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
