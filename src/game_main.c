#include "game_main.h"
#include "mathz.h"

static Vec2 RotateAboutScreen(Vec2 vec, Vec2 center, float angle);

int GameMain(Platform* platform) {
    platform->InitWindow();
    Render* render = platform->render;

    platform->InitConsole();
    printf("Hello from GameMain!\n");

    Color clearColor = (Color) { 0.0f, 0.0f, 0.0f, 1.0f };
    float size = 200.0f;

    Vec2 a2 = (Vec2) { size, 0.0f };
    Vec2 b2 = (Vec2) { size, size };
    Vec2 c2 = (Vec2) { 0.0f, size };
    Color color2 = (Color) { 0.0f, 1.0f, 0.0f, 1.0f };
    float angle = 0;
    float angleStep = 0.001;
    Vec2 rotationCenter = b2;

    while (platform->IsWindowOpen()) {
        platform->ProcessInput();

        if (platform->IsKeyPressed(KeyEsc)) {
            platform->CloseCurrentWindow();
        }

        int posX = platform->GetMouseInputX();
        int posY = platform->GetMouseInputY();

        Vec2 a = (Vec2) { posX, posY };
        Vec2 b = (Vec2) { posX + size, posY };
        Vec2 c = (Vec2) { posX, posY + size };
        Color color = (Color) { 1.0f, 0.0f, 0.0f, 1.0f };

        angle += angleStep;
        Vec2 a2r = RotateAboutScreen(a2, rotationCenter, angle);
        Vec2 b2r = RotateAboutScreen(b2, rotationCenter, angle);
        Vec2 c2r = RotateAboutScreen(c2, rotationCenter, angle);

        render->ClearScreen(clearColor);
        render->DrawTriangle(a, b, c, color);
        render->DrawTriangle(a2r, b2r, c2r, color2);
        render->MakeDrawCall();
    }

    return 0;
}

static Vec2 ToScreenCoordinates(Vec2 vec) {
    return (Vec2) { vec.x, -vec.y };
}

static Vec2 FromScreenCoordinates(Vec2 vec) {
    return (Vec2) { vec.x, -vec.y };
}

static Vec2 RotateAboutScreen(Vec2 vecScreen, Vec2 centerScreen, float angle) {
    Vec2 vec = FromScreenCoordinates(vecScreen);
    Vec2 center = FromScreenCoordinates(centerScreen);

    Vec2 rotated = Vec2RotateAbout(vec, center, angle);
    Vec2 rotatedScreen = ToScreenCoordinates(rotated);

    return rotatedScreen;
}
