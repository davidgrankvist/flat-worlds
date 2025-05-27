/*
 * Public header - core.
 *
 * These are the primary utilities to use in the game, like window creation,
 * managing inputs and rendering.
 */

#ifndef libgame_h
#define libgame_h

#include <stdbool.h>
#include <stdint.h>

// -- Set up platform specifics --

#include "libgame_platform.h"

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

// consume input and update key up/down states, etc.
LIBGAME_EXPORT void ProcessInput();
/*
 * Down = currently held down
 * Pressed = changed from up to down
 * Released = changed from down to up
 */
LIBGAME_EXPORT bool IsKeyDown(InputKey key);
LIBGAME_EXPORT bool IsKeyPressed(InputKey key);
LIBGAME_EXPORT bool IsKeyReleased(InputKey key);
LIBGAME_EXPORT bool IsMouseDown(InputMouseButton btn);
LIBGAME_EXPORT bool IsMousePressed(InputMouseButton btn);
LIBGAME_EXPORT bool IsMouseReleased(InputMouseButton btn);
// mouse position in screen coordinates
LIBGAME_EXPORT int GetMouseInputX();
LIBGAME_EXPORT int GetMouseInputY();
LIBGAME_EXPORT int GetMouseInputDeltaX();
LIBGAME_EXPORT int GetMouseInputDeltaY();
LIBGAME_EXPORT void WarpMousePosition(int x, int y);

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

#define LIBGAME_DEFAULT_MAX_VERTICES 10000;
#define LIBGAME_DEFAULT_MAX_INDICES 10000;

typedef struct {
    int maxVertices;
    int maxVertexIndices;
} RenderSettings;

// see libgame_math.h for details about coordinate system conventions

LIBGAME_EXPORT void ConfigureRender(RenderSettings settings);
LIBGAME_EXPORT void ClearScreen(Color color);
/*
 * Issues a draw call with all of the pending graphics.
 * Resets the current custom transform. The camera transform is not reset.
 */
LIBGAME_EXPORT void MakeDrawCall();
LIBGAME_EXPORT void EndFrame();
// sets a custom transform to apply to all graphics in the next draw call
LIBGAME_EXPORT void SetTransform(Mat4 mat);
// set a camera to be active across draw calls
LIBGAME_EXPORT void SetCamera2D(Camera2D* camera);
LIBGAME_EXPORT void SetCamera3D(Camera3D* camera);
// shapes
LIBGAME_EXPORT void DrawTriangle2D(Vec2 a, Vec2 b, Vec2 c, Color color);
LIBGAME_EXPORT void DrawTriangle3D(Vec3 a, Vec3 b, Vec3 c, Color color);
LIBGAME_EXPORT void DrawQuad3D(Vec3 topLeft, Vec3 topRight, Vec3 bottomLeft, Vec3 bottomRight, Color color);

LIBGAME_EXPORT Camera3D GetDefaultCamera3D();
LIBGAME_EXPORT void RotateCameraFirstPerson(Camera3D* camera, float yaw, float pitch, float roll);
LIBGAME_EXPORT void MoveCameraFirstPerson(Camera3D* camera, Vec3 relativeOffset);
LIBGAME_EXPORT void OrbitCameraAboutTarget(Camera3D* camera, float azimuth, float elevation);
LIBGAME_EXPORT void MoveCameraTowardsTarget(Camera3D* camera, float distanceOffset);

// -- Window --

LIBGAME_EXPORT void InitWindow(const char* title);
LIBGAME_EXPORT bool IsWindowOpen();
LIBGAME_EXPORT void CloseCurrentWindow();
LIBGAME_EXPORT int GetClientWidth();
LIBGAME_EXPORT int GetClientHeight();
LIBGAME_EXPORT void InitConsole();

// -- Timing --

#define TICKS_PER_SECOND 1000000
#define TICKS_TO_SECONDS(t) ((float)(t) / (float)(TICKS_PER_SECOND))

LIBGAME_EXPORT uint64_t GetTicks(); // microseconds
LIBGAME_EXPORT void SetTargetFps(int fps);
LIBGAME_EXPORT int GetFps();
LIBGAME_EXPORT void SleepUntilNextFrame();
LIBGAME_EXPORT void ResetFpsTimer();

// -- Dynamic library loading --

// File handle wrapper to manage load/reload. Zero this struct before first usage.
typedef struct {
    // internal fields - do not edit manually
    void* handle;
    uint64_t lastWrite;
} DynamicLibrary;

typedef enum {
    LibraryExtension,
} FileExtensionType;

/*
 * Takes a path relative to the executable and resolves it to an absolute path.
 * The file extension should be omitted from the string and set via the FileExtensionType instead.
 */
LIBGAME_EXPORT void ResolvePath(char* fileBaseName, FileExtensionType extension, char* out, int outSize);
/*
 * Load or reload a library if it has changed. Returns true if a loading or reload was done.
 * Resolve to an absolute path before calling this.
 */
LIBGAME_EXPORT bool LoadDynamicLibrary(char* libraryPath, DynamicLibrary* lib);
LIBGAME_EXPORT void* LoadLibraryFunction(char* functionName, DynamicLibrary* lib);

#endif
