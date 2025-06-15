#define LIBGAME_MAIN
#include "libgame.h"
#include "game_state.h"

int main(int argc, char** argv) {
    DynamicLibrary gameUpdateLib = {0};
    GameUpdateFunc gameUpdateFn;
    InitGameStateFunc initGameStateFn;

    char gameUpdatePath[256];
    ResolvePath("game_update", LibraryExtension, gameUpdatePath, sizeof(gameUpdatePath));
    LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
    initGameStateFn = (InitGameStateFunc)LoadLibraryFunction("InitGameState", &gameUpdateLib);
    gameUpdateFn = (GameUpdateFunc)LoadLibraryFunction("GameUpdate", &gameUpdateLib);

    GameState gameState = {0};
    initGameStateFn(&gameState);

    InitWindow("Flat Worlds");

    SetTargetFps(60);
    uint64_t ticks = GetTicks();

    while (IsWindowOpen()) {
        bool didUpdate = LoadDynamicLibrary(gameUpdatePath, &gameUpdateLib);
        if (didUpdate) {
            gameUpdateFn = (GameUpdateFunc)LoadLibraryFunction("GameUpdate", &gameUpdateLib);
        }

        ProcessInput();
        SleepUntilNextFrame();

        uint64_t ellapsed = GetTicks() - ticks;
        float deltaTime = TICKS_TO_SECONDS(ellapsed);

        gameUpdateFn(deltaTime, &gameState);

        ticks = GetTicks();
    }

    return 0;
}
