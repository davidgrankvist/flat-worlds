#include <math.h>
#include "libgame_math.h"
#include "game_update.h"

static Color black = { 0, 0, 0, 1 };
static Color red = { 1, 0, 0, 1 };
static Color green = { 0, 1, 0, 1 };
static Color blue = { 0, 0, 1, 1 };

static float movementSpeed = 400;
static float rotationSpeed = 1;
static float lookAroundRotationSpeedFactor = 0.2; // angle delta = mouse delta * factor * delta time
static Color focusColor = { 0, 1, 1, 1 };

void InitGameState(GameState* gameState) {
    *gameState = (GameState){0};

    float side = 50;
    Quad quads[3];

    // In XY plane (right, up)
    Quad quadXy;
    quadXy.topLeft = (Vec3) { 0, side, 0.0f };
    quadXy.topRight = (Vec3) { side, side, 0.0f };
    quadXy.bottomLeft = (Vec3) { 0, 0, 0 };
    quadXy.bottomRight = (Vec3) { side, 0, 0.0f };
    quadXy.color = red;
    quads[0] = quadXy;

    // In XY plane (left, down)
    Quad quadXy2;
    quadXy2.topLeft = (Vec3) { -side, 0, 0.0f };
    quadXy2.topRight = (Vec3) { 0, 0, 0 };
    quadXy2.bottomLeft = (Vec3) { -side, -side, 0.0f };
    quadXy2.bottomRight = (Vec3) { 0, -side, 0.0f };
    quadXy2.color = green;
    quads[1] = quadXy2;

    // In XZ plane (left, forward)
    Quad quadXz;
    quadXz.topLeft = (Vec3) { -side, 0, side };
    quadXz.topRight = (Vec3) { 0, 0, side };
    quadXz.bottomLeft = (Vec3) { -side, 0, 0 };
    quadXz.bottomRight = (Vec3) { 0, 0, 0 };
    quadXz.color = blue;
    quads[2] = quadXz;

    for (int i = 0; i < 3; i++) {
        gameState->planeQuads[i] = quads[i];
    }
    gameState->planeCount = 3;
    gameState->focusedPlane = 0;
    gameState->cameraMode = CameraFirstPerson;

    Camera3D camera3D = GetDefaultCamera3D();
    Camera3D startingCamera = camera3D;

    gameState->camera = camera3D;
    gameState->startingCamera = startingCamera;
}

static void RunDevCommands(GameState* gameState);
static void UpdateCamera(float deltaTime, GameState* gameState);
static void DrawGraphics(GameState* gameState);

void GameUpdate(float deltaTime, GameState* gameState) {
    RunDevCommands(gameState);
    UpdateCamera(deltaTime, gameState);
    DrawGraphics(gameState);
}

static void RunDevCommands(GameState* gameState) {
    if (IsKeyPressed(KeyEsc)) {
        CloseCurrentWindow();
    }
    if (IsKeyDown(KeyLeftShift) && IsKeyPressed(KeyR)) {
        InitGameState(gameState);
    }
}

static void UpdateCameraFirstPerson(float deltaTime, GameState* gameState);
static void UpdateCameraOrbit(float deltaTime, GameState* gameState);
static void UpdateCameraFocus(float deltaTime, GameState* gameState);

static void UpdateCamera(float deltaTime, GameState* gameState) {
    // reset camera
    if (IsKeyPressed(KeyR)) {
        gameState->camera = gameState->startingCamera;
    }

    // switch camera modes
    if (IsKeyPressed(KeyP)) {
        gameState->cameraMode = CameraFirstPerson;
    }
    if (IsKeyPressed(KeyO)) {
        gameState->cameraMode = CameraOrbit;
    }
    if (IsKeyPressed(KeyF)) {
        gameState->cameraMode = CameraFocus;
    }

    switch(gameState->cameraMode) {
        case CameraFirstPerson: {
            UpdateCameraFirstPerson(deltaTime, gameState);
            break;
        }
        case CameraOrbit: {
            UpdateCameraOrbit(deltaTime, gameState);
            break;
        }
        case CameraFocus: {
            UpdateCameraFocus(deltaTime, gameState);
            break;
        }
        default: {
            break;
        }
    }

}

static void UpdateCameraFirstPerson(float deltaTime, GameState* gameState) {
    // -- First person movement --

    float movementStep = movementSpeed * deltaTime;
    Vec3 offset = {0};

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

    // -- First person look around / roll --

    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    float turnDx = GetMouseInputDeltaX();
    float turnDy = -GetMouseInputDeltaY();
    float yawStep = -turnDx * lookAroundRotationSpeedFactor * deltaTime;
    float pitchStep = turnDy * lookAroundRotationSpeedFactor * deltaTime;

    yaw += yawStep;
    pitch += pitchStep;

    float rollStep = rotationSpeed * deltaTime;
    if (IsKeyDown(KeyE)) {
        roll += rollStep;
    }
    if (IsKeyDown(KeyQ)) {
        roll -= rollStep;
    }

    Camera3D* camera = &gameState->camera;

    RotateCameraFirstPerson(camera, yaw, pitch, roll);
    MoveCameraFirstPerson(camera, offset);
}

static void UpdateCameraOrbit(float deltaTime, GameState* gameState) {
    // -- Movement towards target (relative Z) --
    float movementStep = movementSpeed * deltaTime;
    float offsetZ = 0;

    if (IsKeyDown(KeyW)) {
        offsetZ += movementStep;
    }
    if (IsKeyDown(KeyS)) {
        offsetZ -= movementStep;
    }

    // -- Orbit around target --

    float azimuth = 0;
    float elevation = 0;
    float angleStep = rotationSpeed * deltaTime;

    if (IsKeyDown(KeyLeft)) {
        azimuth -= angleStep;
    }
    if (IsKeyDown(KeyRight)) {
        azimuth += angleStep;
    }
    if (IsKeyDown(KeyUp)) {
        elevation += angleStep;
    }
    if (IsKeyDown(KeyDown)) {
        elevation -= angleStep;
    }

    Camera3D* camera = &gameState->camera;

    OrbitCameraAboutTarget(camera, azimuth, elevation);
    MoveCameraTowardsTarget(camera, offsetZ);
}

static void UpdateCameraFocus(float deltaTime, GameState* gameState) {
    // -- Focus - animate camera towards target --

    Camera3D* camera = &gameState->camera;

    if (IsKeyPressed(KeyTab)) {
        gameState->focusedPlane = (gameState->focusedPlane + 1) % (gameState->planeCount);
    }

    // begin animation if we just entered the focus mode or changed focus target
    if (IsKeyPressed(KeyF) || IsKeyPressed(KeyTab)) {
        Quad quad = gameState->planeQuads[gameState->focusedPlane];
        Vec3 quadCenter = Vec3Lerp(quad.topLeft, quad.bottomRight, 0.5);

        gameState->focusTargetStart = camera->target;
        gameState->focusTarget = quadCenter;
        gameState->focusLerpT = 0;

        Vec3 delta = Vec3Sub(camera->position, quadCenter);
        Vec3 deltaN = Vec3Normalize(delta);
        Vec3 targetPosition = Vec3Add(Vec3Scale(deltaN, 250), quadCenter);
        gameState->focusPositionStart = camera->position;
        gameState->focusPosition = targetPosition;
    }

    // end animation if we reached the target
    if (FloatEquals(gameState->focusLerpT, 1)) {
        gameState->camera.target = gameState->focusTarget;
        gameState->camera.position = gameState->focusPosition;
        return;
    }

    float focusLerpStep = deltaTime;
    gameState->focusLerpT = fmin(gameState->focusLerpT + focusLerpStep, 1);

    // turn and move towards target
    Vec3 currTarget = Vec3Lerp(gameState->focusTargetStart, gameState->focusTarget, gameState->focusLerpT);
    Vec3 currPosition = Vec3Lerp(gameState->focusPositionStart, gameState->focusPosition, gameState->focusLerpT);

    camera->target = currTarget;
    camera->position = currPosition;
}

static void DrawGraphics(GameState* gameState) {
    ClearScreen(black);
    SetCamera3D(&gameState->camera);

    for (int i = 0; i < gameState->planeCount; i++) {
        Quad* quad = &gameState->planeQuads[i];
        Color color = gameState->cameraMode == CameraFocus && gameState->focusedPlane == i ? focusColor : quad->color;
        DrawQuad3D(quad->topLeft, quad->topRight, quad->bottomLeft, quad->bottomRight, color);
    }

    MakeDrawCall();
    EndFrame();
}
