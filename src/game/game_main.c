#include "libgame.h"
#include "libgame_platform_main.h"
#include "game_state.h"

static GameState InitGameState(Platform* platform);

int main(int argc, char** argv) {
    Platform* platform = GetPlatform();
    Window window = platform->window;
    Input input = platform->input;
    Timer timer = platform->timer;
    LibraryLoader libLoader = platform->libLoader;

    GameState gameState = InitGameState(platform);

    DynamicLibrary gameUpdateLib = {0};
    GameUpdateFunc gameUpdateFn;

    char gameUpdatePath[256];
    libLoader.ResolvePath("game_update", LibraryExtension, gameUpdatePath, sizeof(gameUpdatePath));
    libLoader.LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
    gameUpdateFn = (GameUpdateFunc)libLoader.LoadLibraryFunction("GameUpdate", &gameUpdateLib);

    window.InitWindow();

    timer.Reset();
    timer.SetTargetFps(60);
    uint64_t ticks = timer.GetTicks();

    while (window.IsWindowOpen()) {
        bool didUpdate = libLoader.LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
        if (didUpdate) {
            gameUpdateFn = (GameUpdateFunc)libLoader.LoadLibraryFunction("GameUpdate", &gameUpdateLib);
        }

        input.ProcessInput();
        timer.SleepUntilNextFrame();

        if (input.IsKeyPressed(KeyEsc)) {
            window.CloseCurrentWindow();
        }

        uint64_t ellapsed = timer.GetTicks() - ticks;
        float deltaTime = TICKS_TO_SECONDS(ellapsed);

        gameUpdateFn(deltaTime, &gameState, platform);

        ticks = timer.GetTicks();
    }

    return 0;
}

static GameState InitGameState(Platform* platform) {
    float refSize = 50;

    GameState gameState = {0};

    /* In XY plane
     *
     *     c
     *  a  b
     */
    gameState.aRef = (Vec3) { 0, 0, 0 };
    gameState.bRef = (Vec3) { refSize, 0, 0.0f };
    gameState.cRef = (Vec3) { refSize, refSize, 0.0f };

    /* In XY plane
     *
     *  b  a
     *  c
     */
    gameState.aRef2 = (Vec3) { 0, 0, 0 };
    gameState.bRef2 = (Vec3) { -refSize, 0, 0.0f };
    gameState.cRef2 = (Vec3) { -refSize, -refSize, 0.0f };

    /* In XZ plane
     *
     * b  c
     *    a
     */
    gameState.aRef3 = (Vec3) { 0, 0, 0 };
    gameState.bRef3 = (Vec3) { -refSize, 0, refSize };
    gameState.cRef3 = (Vec3) { 0, 0, refSize };

    Camera3D camera3D = platform->render.GetDefaultCamera3D();
    Camera3D startingCamera = camera3D;

    gameState.camera = camera3D;
    gameState.startingCamera = startingCamera;

    return gameState;
}
