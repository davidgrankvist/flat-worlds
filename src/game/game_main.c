#include "libgame.h"

static void UpdateCamera(Input input, Camera3D* camera, Camera3D startingCamera);

LIBGAME_EXPORT int GameMain(Platform* platform) {
    Window window = platform->window;
    Input input = platform->input;
    Render render = platform->render;
    FrameTimer timer = platform->timer;

    window.InitWindow();

    timer.Reset();
    timer.SetTargetFps(60);

    Color black = (Color) { 0, 0, 0, 1 };
    Color red = (Color) { 1, 0, 0, 1 };
    Color green = (Color) { 0, 1, 0, 1 };
    Color blue = (Color) { 0, 0, 1, 1 };
    Color purple = (Color) { 1, 0, 1, 1 };

    float refSize = 50;

    /* In XY plane
     *
     *     c
     *  a  b
     */
    Vec3 aRef = { 0, 0, 0 };
    Vec3 bRef = { refSize, 0, 0.0f };
    Vec3 cRef = { refSize, refSize, 0.0f };

    /* In XY plane
     *
     *  b  a
     *  c
     */
    Vec3 aRef2 = { 0, 0, 0 };
    Vec3 bRef2 = { -refSize, 0, 0.0f };
    Vec3 cRef2 = { -refSize, -refSize, 0.0f };

    /* In XZ plane
     *
     * b  c
     *    a
     */
    Vec3 aRef3 = { 0, 0, 0 };
    Vec3 bRef3 = { -refSize, 0, refSize };
    Vec3 cRef3 = { 0, 0, refSize };

    Camera3D camera3D = {0};
    camera3D.position = (Vec3) { 0, 0, -250 };
    camera3D.target = (Vec3) { 0, 0, 0 };
    camera3D.up = (Vec3) { 0, 1, 0};

    Camera3D startingCamera = camera3D;

    while (window.IsWindowOpen()) {
        input.ProcessInput();
        timer.SleepUntilNextFrame();

        if (input.IsKeyPressed(KeyEsc)) {
            window.CloseCurrentWindow();
        }

        UpdateCamera(input, &camera3D, startingCamera);

        render.ClearScreen(black);

        render.SetCamera3D(&camera3D);
        render.DrawTriangle3D(aRef, bRef, cRef, green);
        render.DrawTriangle3D(aRef2, bRef2, cRef2, blue);
        render.DrawTriangle3D(aRef3, bRef3, cRef3, red);

        render.MakeDrawCall();
        render.EndFrame();
    }

    return 0;
}

static void UpdateCamera(Input input, Camera3D* camera3D, Camera3D startingCamera) {
    float step = 5;
    bool didUpdate = false;
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
