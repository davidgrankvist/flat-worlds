#include "libgame.h"
#include "libgame_platform_main.h"
#include "game_state.h"

int main(int argc, char** argv) {
    Platform* platform = GetPlatform();
    Window window = platform->window;
    Input input = platform->input;
    LibraryLoader libLoader = platform->libLoader;

    DynamicLibrary gameUpdateLib = {0};
    GameUpdateFunc gameUpdateFn;
    InitGameStateFunc initGameStateFn;

    char gameUpdatePath[256];
    libLoader.ResolvePath("game_update", LibraryExtension, gameUpdatePath, sizeof(gameUpdatePath));
    libLoader.LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
    initGameStateFn = (InitGameStateFunc)libLoader.LoadLibraryFunction("InitGameState", &gameUpdateLib);
    gameUpdateFn = (GameUpdateFunc)libLoader.LoadLibraryFunction("GameUpdate", &gameUpdateLib);

    GameState gameState = {0};
    initGameStateFn(&gameState);

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

        uint64_t ellapsed = GetTicks() - ticks;
        float deltaTime = TICKS_TO_SECONDS(ellapsed);

        gameUpdateFn(deltaTime, &gameState, platform);

        ticks = GetTicks();
    }

    return 0;
}
