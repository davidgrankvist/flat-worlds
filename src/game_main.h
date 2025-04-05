/*
 * This header defines the platform API and game entrypoint.
 *
 * The platform specific "actual main" sets up the function pointers
 * and passes them into GameMain where the game code runs.
 */

#ifndef game_main_h
#define game_main_h

#include "common.h"

typedef struct {
    void (*InitWindow)();
    bool (*IsWindowOpen)();
    void (*CloseCurrentWindow)();
    int (*GetClientWidth)();
    int (*GetClientHeight)();
    void (*InitConsole)();
} Window;

typedef struct {
    // consume input and update key up/down states, etc.
    void (*ProcessInput)();
    /*
     * Down = currently held down
     * Pressed = changed from up to down
     * Released = changed from down to up
     */
    bool (*IsKeyDown)(InputKey key);
    bool (*IsKeyPressed)(InputKey key);
    bool (*IsKeyReleased)(InputKey key);
    bool (*IsMouseDown)(InputMouseButton btn);
    bool (*IsMousePressed)(InputMouseButton btn);
    bool (*IsMouseReleased)(InputMouseButton btn);
    // mouse position in screen coordinates
    int (*GetMouseInputX)();
    int (*GetMouseInputY)();
} Input;

/*
 * All render coordinates (including 2D calls) are in world coordinates (left-handed):
 * - origin = bottom left
 * - X = right
 * - Y = up
 * - Z = towards the screen
 */
typedef struct {
    void (*ClearScreen)(Color color);
    /*
     * Issues a draw call with all of the pending graphics.
     * Resets current transform.
     */
    void (*MakeDrawCall)();
    // paints to the screen and resets internal rendering state
    void (*EndFrame)();
    // sets transform to apply to all graphics in the next draw call
    void (*SetTransform)(Mat4 mat);
    void (*DrawTriangle)(Vec2 a, Vec2 b, Vec2 c, Color color);
} Render;

typedef struct {
    void (*SetTargetFps)(int fps);
    int (*GetFps)();
    void (*SleepUntilNextFrame)();
    void (*Reset)();
} FrameTimer;

typedef struct {
    Window window;
    Input input;
    Render render;
    FrameTimer timer;
} Platform;

int GameMain(Platform* platform);

#endif
