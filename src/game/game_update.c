#include "game_update.h"

static void UpdateCamera(float deltaTime, Input input, Camera3D* camera3D, Camera3D startingCamera);
static void DrawTriangles(GameState* gameState, Platform* platform);

void GameUpdate(float deltaTime, GameState* gameState, Platform* platform) {
    UpdateCamera(deltaTime, platform->input, &gameState->camera, gameState->startingCamera);
    DrawTriangles(gameState, platform);
}

static float cameraSpeed = 400;
static Color black = { 0, 0, 0, 1 };
static Color red = { 1, 0, 0, 1 };
static Color green = { 0, 1, 0, 1 };
static Color blue = { 0, 0, 1, 1 };


static void UpdateCamera(float deltaTime, Input input, Camera3D* camera3D, Camera3D startingCamera) {

    float speed = cameraSpeed;
    float step = speed * deltaTime;

    if (input.IsKeyDown(KeyRight)) {
        camera3D->position.x += step;
    }
    if (input.IsKeyDown(KeyLeft)) {
        camera3D->position.x -= step;
    }
    if (input.IsKeyDown(KeyUp)) {
        camera3D->position.y += step;
    }
    if (input.IsKeyDown(KeyDown)) {
        camera3D->position.y -= step;
    }
    if (input.IsKeyDown(KeyW)) {
        camera3D->position.z += step;
    }
    if (input.IsKeyDown(KeyS)) {
        camera3D->position.z -= step;
    }
    if (input.IsKeyPressed(KeyR)) {
        *camera3D = startingCamera;
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
