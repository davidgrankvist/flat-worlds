#include "game_update.h"

static void UpdateCamera(float deltaTime, GameState* gameState);
static void DrawGraphics(GameState* gameState, Render* render);

void GameUpdate(float deltaTime, GameState* gameState, Platform* platform) {
    UpdateCamera(deltaTime, gameState);
    DrawGraphics(gameState, &platform->render);
}

static Color black = { 0, 0, 0, 1 };
static Color red = { 1, 0, 0, 1 };
static Color green = { 0, 1, 0, 1 };
static Color blue = { 0, 0, 1, 1 };

static float movementSpeed = 400;
static float rotationSpeed = 1;

static void UpdateCamera(float deltaTime, GameState* gameState) {
    Camera3D* camera = &gameState->camera;

    float movementStep = movementSpeed * deltaTime;
    Vec3 offset = {0};

    float angleStep = rotationSpeed * deltaTime;
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    if (IsKeyPressed(KeyR)) {
        gameState->camera = gameState->startingCamera;
    }
    if (IsKeyDown(KeyW)) {
        offset.z += movementStep;
    }
    if (IsKeyDown(KeyS)) {
        offset.z -= movementStep;
    }
    if (IsKeyDown(KeyA)) {
        offset.x -= movementStep;
    }
    if (IsKeyDown(KeyD)) {
        offset.x += movementStep;
    }
    if (IsKeyDown(KeyJ)) {
        offset.y -= movementStep;
    }
    if (IsKeyDown(KeyK)) {
        offset.y += movementStep;
    }

    if (IsKeyDown(KeyLeft)) {
        yaw += angleStep;
    }
    if (IsKeyDown(KeyRight)) {
        yaw -= angleStep;
    }
    if (IsKeyDown(KeyUp)) {
        pitch += angleStep;
    }
    if (IsKeyDown(KeyDown)) {
        pitch -= angleStep;
    }
    if (IsKeyDown(KeyE)) {
        roll += angleStep;
    }
    if (IsKeyDown(KeyQ)) {
        roll -= angleStep;
    }

    if (IsKeyPressed(KeyO)) {
        gameState->orbitMode = !gameState->orbitMode;
    }

    if (gameState->orbitMode) {
        float azimuth = -yaw;
        float elevation = pitch;
        OrbitCameraAboutTarget(camera, azimuth, elevation);
        MoveCameraTowardsTarget(camera, offset.z);
    } else {
        RotateCameraFirstPerson(camera, yaw, pitch, roll);
        MoveCameraFirstPerson(camera, offset);
    }
}

static void DrawGraphics(GameState* gameState, Render* render) {
    render->ClearScreen(black);
    render->SetCamera3D(&gameState->camera);

    gameState->quads[0].color = red;
    gameState->quads[1].color = green;
    gameState->quads[2].color = blue;

    for (int i = 0; i < 3; i++) {
        Quad* quad = &gameState->quads[i];
        render->DrawQuad3D(quad->topLeft, quad->topRight, quad->bottomLeft, quad->bottomRight, quad->color);
    }

    render->MakeDrawCall();
    render->EndFrame();
}
