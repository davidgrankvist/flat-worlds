/*
 * This header defines the platform API and game entrypoint.
 *
 * The platform specific "actual main" sets up the function pointers
 * and passes them into GameMain where the game code runs.
 */

#ifndef game_main_h
#define game_main_h

#include "common.h"

/*
 * Render part of platform API.
 *
 * All render coordinates (including 2D calls) are in world coordinates, left-handed XYZ:
 * - origin = bottom left
 * - X = right
 * - Y = upwards
 * - Z = away from the screen
 */
typedef struct {
    void (*ClearScreen)(Color color);
    /*
     * Issues a draw call with all of the pending graphics.
     * Resets current transform.
     */
    void (*MakeDrawCall)();
    /*
     * Paints to the screen and resets internal rendering state.
     * Should only be called once per frame.
     */
    void (*EndFrame)();
    /*
     * Sets transform to apply to all graphics in the next draw call.
     */
    void (*SetTransform)(Mat4 mat);
    void (*DrawTriangle)(Vec2 a, Vec2 b, Vec2 c, Color color);
} Render;

// platform API
typedef struct {
    // -- Window --

    void (*InitWindow)();
    bool (*IsWindowOpen)();
    void (*CloseCurrentWindow)();
    /*
     * Dimensions of window client area to render to.
     */
    int (*GetClientWidth)();
    int (*GetClientHeight)();

    void (*InitConsole)();

    // -- Input --

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

    // -- Graphics --

    Render* render;
} Platform;

// game code entrypoint
int GameMain(Platform* platform);

#endif
