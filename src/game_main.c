#include "game_main.h"
#include "mathz.h"

static Vec2 GetMouseWorldCoordinates(Platform* platform);

int GameMain(Platform* platform) {
    platform->InitWindow();
    Render* render = platform->render;

    platform->InitConsole();
    printf("Hello from GameMain!\n");

    Color black = (Color) { 0.0f, 0.0f, 0.0f, 1.0f };
    Color red = (Color) { 1.0f, 0.0f, 0.0f, 1.0f };
    Color green = (Color) { 0.0f, 1.0f, 0.0f, 1.0f };
    Color blue = (Color) { 0.0f, 0.0f, 1.0f, 1.0f };

    float size = 200.0f;

    float angle = 0;
    float angleStep = 0.001;

    Vec2 a2 = (Vec2) { size, 0.0f };
    Vec2 b2 = (Vec2) { size, size };
    Vec2 c2 = (Vec2) { 0.0f, size };
    Vec2 rotationCenter = b2;

    int offs = 100;
    Vec2 a3 = (Vec2) { size + offs, offs};
    Vec2 b3 = (Vec2) { size + offs, size + offs };
    Vec2 c3 = (Vec2) { offs, size + offs };
    Vec2 rotationCenter2 = b3;


    while (platform->IsWindowOpen()) {
        platform->ProcessInput();

        if (platform->IsKeyPressed(KeyEsc)) {
            platform->CloseCurrentWindow();
        }

        Vec2 vecMouse = GetMouseWorldCoordinates(platform);
        float posX = vecMouse.x;
        float posY = vecMouse.y;

        Vec2 a = (Vec2) { posX, posY };
        Vec2 b = (Vec2) { posX + size, posY };
        Vec2 c = (Vec2) { posX, posY + size };

        // CPU side transform
        angle += angleStep;
        Vec2 a2r = Vec2RotateAbout(a2, rotationCenter, angle);
        Vec2 b2r = Vec2RotateAbout(b2, rotationCenter, angle);
        Vec2 c2r = Vec2RotateAbout(c2, rotationCenter, angle);

        render->ClearScreen(black);

        render->DrawTriangle(a, b, c, red);
        render->DrawTriangle(a2, b2, c2, green);
        render->DrawTriangle(a2r, b2r, c2r, blue);
        render->MakeDrawCall();

        /*
         * This works on its own, but doesn't quite work combined with the other code.
         * MakeDrawCall resets the vertex count, but we don't want to overwrite
         * that data until we begin the next frame.
         */
        // GPU side transform
        //Mat4 transform = Mat4RotateAbout2(rotationCenter2, angle);
        //render->SetTransform(transform);
        //render->DrawTriangle(a3, b3, c3, blue);
        //render->MakeDrawCall();
    }

    return 0;
}

static Vec2 GetMouseWorldCoordinates(Platform* platform) {
    int posX = platform->GetMouseInputX();
    int posY = platform->GetMouseInputY();
    int height = platform->GetClientHeight();

    Vec2 vec = { posX, height - posY };
    return vec;
}
