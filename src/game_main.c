#include "game_main.h"

int GameMain(Platform* platform) {
    platform->InitWindow();
    Render* render = platform->render;

    platform->InitConsole();
    printf("Hello from GameMain!\n");

    Color clearColor = (Color) { 0.0f, 0.0f, 0.0f, 1.0f };

    while (platform->IsWindowOpen()) {
        platform->ProcessInput();

        if (platform->IsKeyPressed(KeyEsc)) {
            platform->CloseCurrentWindow();
        }

        int posX = platform->GetMouseInputX();
        int posY = platform->GetMouseInputY();

        float size = 200.0f;
        Vec2 a = (Vec2) { posX, posY };
        Vec2 b = (Vec2) { posX + size, posY };
        Vec2 c = (Vec2) { posX, posY + size };
        Color color = (Color) { 1.0f, 0.0f, 0.0f, 1.0f };

        Vec2 a2 = (Vec2) { size, 0.0f };
        Vec2 b2 = (Vec2) { size, size };
        Vec2 c2 = (Vec2) { 0.0f, size };
        Color color2 = (Color) { 0.0f, 1.0f, 0.0f, 1.0f };

        render->ClearScreen(clearColor);
        render->DrawTriangle(a, b, c, color);
        render->DrawTriangle(a2, b2, c2, color2);
        render->MakeDrawCall();
    }

    return 0;
}
