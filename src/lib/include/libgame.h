/*
 * Public header - core.
 *
 * These are the primary utilities to use in the game, like window creation,
 * managing inputs and rendering.
 *
 * The Platform struct is a container for function pointers to those utilities.
 */

#ifndef libgame_h
#define libgame_h

#include <stdbool.h>
#include <stdint.h>

// -- Math --
// Just the types here. See libgame_math.h for math functions.

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

// row major 4x4 matrix
typedef struct {
    float m[4][4];
} Mat4;

// -- Input --

typedef enum {
    // letters
    KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH,
    KeyI, KeyJ, KeyK, KeyL, KeyM, KeyN, KeyO, KeyP,
    KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX,
    KeyY, KeyZ,
    // numbers
    Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
    // modifiers
    KeyLeftShift, KeyRightShift,
    KeyLeftCtrl, KeyRightCtrl,
    KeyLeftAlt, KeyRightAlt,
    // fn
    KeyF1, KeyF2, KeyF3, KeyF4, KeyF5, KeyF6, KeyF7, KeyF8,
    KeyF9, KeyF10, KeyF11, KeyF12,
    // arrows
    KeyLeft, KeyUp, KeyRight, KeyDown,
    // special
    KeySpace, KeyEnter, KeyBackspace, KeyTab, KeyEsc,
    // fallback value / checking number of codes
    KeyUnknown,
} InputKey;

typedef enum {
    MouseLeft,
    MouseRight,
    MouseMiddle,
    // fallback value / checking number of codes
    MouseUnknown,
} InputMouseButton;

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

// -- Graphics --

// normalized colors (0 to 1)
typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;

typedef struct {
   Vec2 origin; // bottom left
} Camera2D;

typedef struct {
    // position / orientation
    Vec3 position;
    Vec3 target;
    Vec3 up;
    // frustum
    float fieldOfViewY;
    float nearPlane;
    float farPlane;
    float aspectRatio; // set to 0 to use the entire client area
} Camera3D;

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
     * Resets the current custom transform. The camera transform is not reset.
     */
    void (*MakeDrawCall)();
    // paints to the screen and resets internal rendering state
    void (*EndFrame)();
    // sets a custom transform to apply to all graphics in the next draw call
    void (*SetTransform)(Mat4 mat);
    void (*DrawTriangle2D)(Vec2 a, Vec2 b, Vec2 c, Color color);
    void (*DrawTriangle3D)(Vec3 a, Vec3 b, Vec3 c, Color color);
    void (*SetCamera2D)(Camera2D* camera);
    void (*SetCamera3D)(Camera3D* camera);
    Camera3D (*GetDefaultCamera3D)();
} Render;

// -- Window --

typedef struct {
    void (*InitWindow)();
    bool (*IsWindowOpen)();
    void (*CloseCurrentWindow)();
    int (*GetClientWidth)();
    int (*GetClientHeight)();
    void (*InitConsole)();
} Window;

// -- Timing --

#define TICKS_PER_SECOND 1000000
#define TICKS_TO_SECONDS(t) ((float)(t) / (float)(TICKS_PER_SECOND))

typedef struct {
    void (*SetTargetFps)(int fps);
    int (*GetFps)();
    void (*SleepUntilNextFrame)();
    void (*Reset)();
    uint64_t (*GetTicks)(); // microseconds
} Timer;

// -- Dynamic loading --

// File handle wrapper to manage load/reload. Zero this struct before first usage.
typedef struct {
    // internal fields - do not edit manually
    void* handle;
    uint64_t lastWrite;
} DynamicLibrary;

typedef enum {
    LibraryExtension,
} FileExtensionType;

typedef struct {
    /*
     * Takes a path relative to the executable and resolves it to an absolute path.
     * The file extension should be omitted from the string and set via the FileExtensionType instead.
     */
    void (*ResolvePath)(char* fileBaseName, FileExtensionType extension, char* out, int outSize);
    /*
     * Load or reload a library if it has changed. Returns true if a loading or reload was done.
     * Resolve to an absolute path before calling this.
     */
    bool (*LoadDynamicLibrary)(char* libraryPath, DynamicLibrary* lib);
    void* (*LoadLibraryFunction)(char* functionName, DynamicLibrary* lib);
} LibraryLoader;

// -- Platform API combined struct --

typedef struct {
    Window window;
    Input input;
    Render render;
    Timer timer;
    LibraryLoader libLoader;
} Platform;

// -- Set up platform specifics --

#include "libgame_platform.h"

LIBGAME_EXPORT Platform* GetPlatform();

#endif
