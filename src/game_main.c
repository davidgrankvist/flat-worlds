#include "game_main.h"

int GameMain(Platform* platform) {
    platform->InitWindow();
    Render* render = platform->render;

    while (platform->IsWindowOpen()) {
        platform->ProcessInput();

        render->ClearScreen(0.0f, 0.0f, 0.0f, 1.0f);
        render->MakeDrawCall();
    }

    return 0;
}
