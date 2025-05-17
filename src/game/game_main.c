#include "libgame.h"
#include "libgame_platform_main.h"
#include "game_state.h"

static GameState InitGameState();

int main(int argc, char** argv) {
    Platform* platform = GetPlatform();
    Window window = platform->window;
    Input input = platform->input;
    LibraryLoader libLoader = platform->libLoader;

    GameState gameState = InitGameState();

    DynamicLibrary gameUpdateLib = {0};
    GameUpdateFunc gameUpdateFn;

    char gameUpdatePath[256];
    libLoader.ResolvePath("game_update", LibraryExtension, gameUpdatePath, sizeof(gameUpdatePath));
    libLoader.LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
    gameUpdateFn = (GameUpdateFunc)libLoader.LoadLibraryFunction("GameUpdate", &gameUpdateLib);

    window.InitWindow();

    ResetFpsTimer();
    SetTargetFps(60);
    uint64_t ticks = GetTicks();

    while (window.IsWindowOpen()) {
        bool didUpdate = libLoader.LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
        if (didUpdate) {
            gameUpdateFn = (GameUpdateFunc)libLoader.LoadLibraryFunction("GameUpdate", &gameUpdateLib);
        }

        input.ProcessInput();
        SleepUntilNextFrame();

        if (IsKeyPressed(KeyEsc)) {
            window.CloseCurrentWindow();
        }

        uint64_t ellapsed = GetTicks() - ticks;
        float deltaTime = TICKS_TO_SECONDS(ellapsed);

        gameUpdateFn(deltaTime, &gameState, platform);

        ticks = GetTicks();
    }

    return 0;
}

static GameState InitGameState() {
    float refSize = 50;

    GameState gameState = {0};

    // In XY plane (right, up)
    Quad quadXy;
    quadXy.topLeft = (Vec3) { 0, refSize, 0.0f };
    quadXy.topRight = (Vec3) { refSize, refSize, 0.0f };
    quadXy.bottomLeft = (Vec3) { 0, 0, 0 };
    quadXy.bottomRight = (Vec3) { refSize, 0, 0.0f };
    gameState.quads[0] = quadXy;

    // In XY plane (left, down)
    Quad quadXy2;
    quadXy2.topLeft = (Vec3) { -refSize, 0, 0.0f };
    quadXy2.topRight = (Vec3) { 0, 0, 0 };
    quadXy2.bottomLeft = (Vec3) { -refSize, -refSize, 0.0f };
    quadXy2.bottomRight = (Vec3) { 0, -refSize, 0.0f };
    gameState.quads[1] = quadXy2;

    // In XZ plane (left, forward)
    Quad quadXz;
    quadXz.topLeft = (Vec3) { -refSize, 0, refSize };
    quadXz.topRight = (Vec3) { 0, 0, refSize };
    quadXz.bottomLeft = (Vec3) { -refSize, 0, 0 };
    quadXz.bottomRight = (Vec3) { 0, 0, 0 };
    gameState.quads[2] = quadXz;

    Camera3D camera3D = GetDefaultCamera3D();
    Camera3D startingCamera = camera3D;

    gameState.camera = camera3D;
    gameState.startingCamera = startingCamera;

    return gameState;
}
