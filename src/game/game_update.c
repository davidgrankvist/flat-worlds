#include "game_update.h"

void InitGameState(GameState* gameState) {
    *gameState = (GameState){0};

    float refSize = 50;

    // In XY plane (right, up)
    Quad quadXy;
    quadXy.topLeft = (Vec3) { 0, refSize, 0.0f };
    quadXy.topRight = (Vec3) { refSize, refSize, 0.0f };
    quadXy.bottomLeft = (Vec3) { 0, 0, 0 };
    quadXy.bottomRight = (Vec3) { refSize, 0, 0.0f };
    gameState->quads[0] = quadXy;

    // In XY plane (left, down)
    Quad quadXy2;
    quadXy2.topLeft = (Vec3) { -refSize, 0, 0.0f };
    quadXy2.topRight = (Vec3) { 0, 0, 0 };
    quadXy2.bottomLeft = (Vec3) { -refSize, -refSize, 0.0f };
    quadXy2.bottomRight = (Vec3) { 0, -refSize, 0.0f };
    gameState->quads[1] = quadXy2;

    // In XZ plane (left, forward)
    Quad quadXz;
    quadXz.topLeft = (Vec3) { -refSize, 0, refSize };
    quadXz.topRight = (Vec3) { 0, 0, refSize };
    quadXz.bottomLeft = (Vec3) { -refSize, 0, 0 };
    quadXz.bottomRight = (Vec3) { 0, 0, 0 };
    gameState->quads[2] = quadXz;

    Camera3D camera3D = GetDefaultCamera3D();
    Camera3D startingCamera = camera3D;

    gameState->camera = camera3D;
    gameState->startingCamera = startingCamera;
}

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
    if (IsKeyPressed(KeyR)) {
        InitGameState(gameState);
    }

    Camera3D* camera = &gameState->camera;

    float movementStep = movementSpeed * deltaTime;
    Vec3 offset = {0};

    float angleStep = rotationSpeed * deltaTime;
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

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
