/*
 * This file is the library entrypoint when targeting win32. It sets up function pointers
 * for the Platform struct that's used in the game code.
 *
 * The WinMain is defined in libgame_platform_main.h and is not included in the library build.
 */

#ifndef UNICODE
#define UNICODE
#endif

#include "platform_include.h"

#include <timeapi.h>
#include <stdio.h>

#include "opengl_render.h"
#include <gl/wglext.h>

#include "libgame.h"
#include "input.h"
#include "timing.h"
#include "asserts.h"
#include "camera.h"

// -- Forward declarations (public API) --

/*
 * Used in WinMain to set up platform function pointers.
 */
void InitMainWin32();

/*
 * Used by the game code to access the platform API
 * after the function pointers have been set up.
 */
Platform* GetPlatform();

// -- Forward declarations (internal) --

static Platform InitPlatformWin32();

static void InitWindow();
static bool IsWindowOpen();
static void CloseCurrentWindow();
static int GetClientWidth();
static int GetClientHeight();
static void InitConsole();
static void ProcessInput();
static void SetTransform(Mat4 mat);
static void EndFrame();

static void MakeDrawCallGl();
static void InitTiming();
static inline int64_t GetMicroTicks();
static void ResolvePath(char* name, FileExtensionType extension, char* out, int outSize);
static bool LoadLibraryInternal(char* name, DynamicLibrary* lib);
static void* LoadLibraryFunction(char* name, DynamicLibrary* lib);

// -- State --

HINSTANCE windowHInstance;
int windowNCmdShow;
MSG msg = {};
bool shouldRun = true;
HDC windowHdc;

static int clientWidth = 0;
static int clientHeight = 0;

Platform win32Platform = {};

Platform* GetPlatform() {
    return &win32Platform;
}

void InitMainWin32() {
    if (getenv("DEBUG_CONSOLE")) {
        InitConsole();
    }
    Assert(KeyUnknown <= 64, "Too many key codes to fit in a u64. Please update the input data structure.");
    Assert(MouseUnknown <= 8, "Too many mouse key codes to fit in a u8. Please update the input data structure.");

    windowHInstance = GetModuleHandle(NULL);
    windowNCmdShow = SW_SHOWDEFAULT;

    win32Platform = InitPlatformWin32();
}

static Platform InitPlatformWin32() {
    Platform platform = {};

    Window window = {};
    window.InitWindow = InitWindow;
    window.IsWindowOpen = IsWindowOpen;
    window.CloseCurrentWindow = CloseCurrentWindow;
    window.GetClientWidth = GetClientWidth;
    window.GetClientHeight = GetClientHeight;
    window.InitConsole = InitConsole;
    platform.window = window;

    Input input = {};
    input.ProcessInput = ProcessInput;
    input.IsKeyDown = IsKeyDown;
    input.IsKeyPressed = IsKeyPressed;
    input.IsKeyReleased = IsKeyReleased;
    input.IsMouseDown = IsMouseDown;
    input.IsMousePressed = IsMousePressed;
    input.IsMouseReleased = IsMouseReleased;
    input.GetMouseInputX = GetMouseInputX;
    input.GetMouseInputY = GetMouseInputY;
    platform.input = input;

    Render render = {};
    render.MakeDrawCall = MakeDrawCallGl;
    render.ClearScreen = ClearScreenGl;
    render.DrawTriangle2D = DrawTriangle2DGl;
    render.DrawTriangle3D = DrawTriangle3DGl;
    render.SetTransform = SetTransformGl;
    render.EndFrame = EndFrame;
    render.SetCamera2D = SetCamera2DGl;
    render.SetCamera3D = SetCamera3DGl;
    render.GetDefaultCamera3D = GetDefaultCamera3D;
    render.RotateCamera3D = RotateCamera3D;
    platform.render = render;

    InitTiming();
    Timer timer = {};
    timer.SetTargetFps = SetTargetFps;
    timer.GetFps = GetFps;
    timer.SleepUntilNextFrame = SleepUntilNextFrame;
    timer.Reset = ResetTimer;
    timer.GetTicks = GetMicroTicks;
    platform.timer = timer;

    LibraryLoader libLoader = {};
    libLoader.ResolvePath = ResolvePath;
    libLoader.LoadDynamicLibrary = LoadLibraryInternal;
    libLoader.LoadLibraryFunction = LoadLibraryFunction;
    platform.libLoader = libLoader;

    return platform;
}

// -- Window --

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HGLRC InitOpenGl(HDC windowHdc);

static void InitWindow() {
    const wchar_t className[] = L"WindowClassName";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindProc;
    wc.hInstance = windowHInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
            0,
            className,
            L"Flat Worlds",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,
            NULL,
            windowHInstance,
            NULL);

    windowHdc = GetDC(hwnd);
    HGLRC hglrc = InitOpenGl(windowHdc);

    ShowWindow(hwnd, windowNCmdShow);
}

static bool IsWindowOpen() {
    return shouldRun;
}

static void CloseCurrentWindow() {
    shouldRun = false;
}

void MapAndSetResolution(LPARAM lParam);
InputKey MapKeyCode(WPARAM wParam, LPARAM lParam);
void MapAndSetMousePosition(LPARAM lParam);

// for reading lParam things
#define EXTRACT_LOW16(n) (n & 0x0000FFFF);
#define EXTRACT_HIGH16(n) (n >> 16)

static LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            MapAndSetResolution(lParam);
            return 0;
        // keys
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            SetKeyDown(MapKeyCode(wParam, lParam));
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            SetKeyUp(MapKeyCode(wParam, lParam));
            return 0;
        // mouse
        case WM_LBUTTONDOWN:
            SetMouseDown(MouseLeft);
            return 0;
        case WM_LBUTTONUP:
            SetMouseUp(MouseLeft);
            return 0;
        case WM_RBUTTONDOWN:
            SetMouseDown(MouseRight);
            return 0;
        case WM_RBUTTONUP:
            SetMouseUp(MouseRight);
            return 0;
        case WM_MBUTTONDOWN:
            SetMouseDown(MouseMiddle);
            return 0;
        case WM_MBUTTONUP:
            SetMouseUp(MouseMiddle);
            return 0;
        case WM_MOUSEMOVE: {
            MapAndSetMousePosition(lParam);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

typedef struct {
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
} WglExt;
WglExt wglExt = {};

#define LOAD_OPENGL_EXTENSION_INTO(name, type, target) \
    do { \
        target = (type)wglGetProcAddress(#name); \
        Assert(target != NULL, "Unable to load OpenGL extension %s", #name); \
    } while(false)

#define LOAD_OPENGL_EXTENSION(name, type) LOAD_OPENGL_EXTENSION_INTO(name, type, openGlExt->name)
#define LOAD_WGL_EXTENSION(name, type) LOAD_OPENGL_EXTENSION_INTO(name, type, wglExt.name)

static void LoadOpenGlExtensions(OpenGlExt* openGlExt) {
    LOAD_OPENGL_EXTENSION(glBindBuffer, PFNGLBINDBUFFERPROC);
    LOAD_OPENGL_EXTENSION(glGenBuffers, PFNGLGENBUFFERSPROC);
    LOAD_OPENGL_EXTENSION(glBufferData, PFNGLBUFFERDATAPROC);
    LOAD_OPENGL_EXTENSION(glAttachShader, PFNGLATTACHSHADERPROC);
    LOAD_OPENGL_EXTENSION(glCompileShader, PFNGLCOMPILESHADERPROC);
    LOAD_OPENGL_EXTENSION(glCreateProgram, PFNGLCREATEPROGRAMPROC);
    LOAD_OPENGL_EXTENSION(glCreateShader, PFNGLCREATESHADERPROC);
    LOAD_OPENGL_EXTENSION(glDeleteShader, PFNGLDELETESHADERPROC);
    LOAD_OPENGL_EXTENSION(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
    LOAD_OPENGL_EXTENSION(glLinkProgram, PFNGLLINKPROGRAMPROC);
    LOAD_OPENGL_EXTENSION(glShaderSource, PFNGLSHADERSOURCEPROC);
    LOAD_OPENGL_EXTENSION(glUseProgram, PFNGLUSEPROGRAMPROC);
    LOAD_OPENGL_EXTENSION(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
    LOAD_OPENGL_EXTENSION(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
    LOAD_OPENGL_EXTENSION(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
    LOAD_OPENGL_EXTENSION(glGetShaderiv, PFNGLGETSHADERIVPROC);
    LOAD_OPENGL_EXTENSION(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
    LOAD_OPENGL_EXTENSION(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
    LOAD_OPENGL_EXTENSION(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
    LOAD_OPENGL_EXTENSION(glBufferSubData, PFNGLBUFFERSUBDATAPROC);
    LOAD_OPENGL_EXTENSION(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
    LOAD_OPENGL_EXTENSION(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
}

static void LoadWglExtensions() {
    LOAD_WGL_EXTENSION(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC);
}

static HGLRC InitOpenGl(HDC windowHdc) {
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;

    int pixelFormat = ChoosePixelFormat(windowHdc, &pfd);
    SetPixelFormat(windowHdc, pixelFormat, &pfd);

    PIXELFORMATDESCRIPTOR actualPfd = {};
    DescribePixelFormat(windowHdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &actualPfd);

    // -- Create a context for OpenGL 3.3 --

    // set up dummy context to be able to load WGL extensions
    HGLRC dummyContext = wglCreateContext(windowHdc);
    wglMakeCurrent(windowHdc, dummyContext);

    LoadWglExtensions();

    int attributes[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC actualContext = wglExt.wglCreateContextAttribsARB(windowHdc, dummyContext, attributes);
    wglMakeCurrent(windowHdc, actualContext);
    wglDeleteContext(dummyContext);

    // -- Load the OpenGL 3.3 extensions --

    OpenGlExt openGlExt = {};
    LoadOpenGlExtensions(&openGlExt);
    InitGraphicsGl(openGlExt);

    return actualContext;
}

static void MapAndSetResolution(LPARAM lParam) {
    clientWidth = EXTRACT_LOW16(lParam);
    clientHeight = EXTRACT_HIGH16(lParam);

    SetResolutionGl(clientWidth, clientHeight);
}

static int GetClientWidth() {
    return clientWidth;
}

static int GetClientHeight() {
    return clientHeight;
}

// -- Console --

void InitConsole() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "r", stdin);
}

// -- Input --

static void ProcessInput() {
    UpdateInputBuffers();

    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            shouldRun = false;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/*
 * Some virtual keycodes are sequential, so we can leverage that
 * by ordering the enum values similarly.
 */
static InputKey MapKeyCodeInRange(WPARAM vk, WPARAM vkStart, InputKey keyStart) {
    int offset = vk - vkStart;
    InputKey result = keyStart + offset;
    return result;
}

// see https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
static InputKey MapKeyCode(WPARAM wParam, LPARAM lParam) {
    int vk = wParam;
    int scanCode = EXTRACT_HIGH16(lParam) & 0xFF;

    // letters
    if (vk >= 0x41 && vk <= 0x5A) {
        return MapKeyCodeInRange(vk, 0x41, KeyA);
    }
    // numbers
    if (vk >= 0x30 && vk <= 0x39) {
        return MapKeyCodeInRange(vk, 0x30, Key0);
    }
    // modifiers (generic shift/ctrl/alt)
    if (vk >= 0x10 && vk <= 0x12) {
        // check for left/right variant
        int actualVk = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
        if (actualVk >= 0xA0 && actualVk <= 0xA5) {
            return MapKeyCodeInRange(actualVk, 0xA0, KeyLeftShift);
        }
    }
    // fn
    if (vk >= 0x70 && vk <= 0x7B) {
        return MapKeyCodeInRange(vk, 0x70, KeyF1);
    }
    // arrows
    if (vk >= 0x25 && vk <= 0x28) {
        return MapKeyCodeInRange(vk, 0x25, KeyLeft);
    }
    // other
    switch(vk) {
        case 0x20: return KeySpace;
        case 0x0D: return KeyEnter;
        case 0x08: return KeyBackspace;
        case 0x09: return KeyTab;
        case 0x1B: return KeyEsc;
        default: return KeyUnknown;
    }
}

// see https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove
static void MapAndSetMousePosition(LPARAM lParam) {
    int x = EXTRACT_LOW16(lParam);
    int y = EXTRACT_HIGH16(lParam);

    SetMousePosition(x, y);
}

// -- Render --

static void MakeDrawCallGl() {
    EndDrawGl();
}

static void EndFrame() {
    EndFrameGl();
    SwapBuffers(windowHdc);
}

// -- Timing --

static int64_t usPerSecond = 1000000;
static int64_t usPerMs = 1000;
static int64_t usPerTick = 0; // Dummy value. This is set in InitTiming.

static void MicroSleep(int us);

static void InitTiming() {
    MMRESULT result = timeBeginPeriod(1);
    Assert(result == TIMERR_NOERROR, "Unable to set sleep resolution to 1 ms");

    LARGE_INTEGER lpFrequency;
    bool success = QueryPerformanceFrequency(&lpFrequency);
    Assert(success, "Unable to check QPC frequency");

    int64_t ticksPerSecond = (int64_t)lpFrequency.QuadPart;
    Assert(ticksPerSecond >= usPerSecond, "Too low QPC frequency. Unable to use microsecond sleep.");

    usPerTick = ticksPerSecond / usPerSecond;

    PlatformTiming platformTiming = {};
    platformTiming.GetMicroTicks = GetMicroTicks;
    platformTiming.MicroSleep = MicroSleep;

    InitPlatformTiming(platformTiming);
}

static inline int64_t GetTicks() {
    LARGE_INTEGER ticks;
    int success = QueryPerformanceCounter(&ticks);
    Assert(success, "Failed to call QPC");
    return ticks.QuadPart;
}

static inline int64_t GetMicroTicks() {
    return GetTicks() / usPerTick;
}

// combine millisecond sleep and busy-wait for higher precision
static void MicroSleep(int us) {
    int64_t usStart = GetMicroTicks();

    if (us < usPerMs) {
        while(GetMicroTicks() - usStart < us) {}
        return;
    }

    // win32 sleep may overshoot, so intentionally undershoot by 1ms
    int loweredUs = us - usPerMs;

    if (loweredUs / usPerMs > 0 ) {
        Sleep(loweredUs / usPerMs);
    }

    while(GetMicroTicks() - usStart < us) {}
}

// -- Dynamic loading --

static const char* GetFileExtension(FileExtensionType extension) {
    switch(extension) {
        case LibraryExtension: return ".dll";
        default: AssertFail("Unsupported file extension %d", extension);
    }

    return NULL;
}

static void ResolvePath(char* name, FileExtensionType extension, char* out, int outSize) {
    const char* ext = GetFileExtension(extension);

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, sizeof(exePath));

    char* lastSlash = strrchr(exePath, '\\');
    *(lastSlash + 1) = '\0';

    strcat(exePath, name);
    strcat(exePath, ext);
    strncpy(out, exePath, outSize);
}

static uint64_t LastFileWrite(char* path) {
    WIN32_FIND_DATAA findData = {};
    HANDLE fileHandle = FindFirstFileA(path, &findData);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        return 0; // assume that it's locked and signal with a dummy timestamp
    }

    FILETIME lastWriteFt = findData.ftLastWriteTime;
    ULARGE_INTEGER ull;
    ull.LowPart = lastWriteFt.dwLowDateTime;
    ull.HighPart = lastWriteFt.dwHighDateTime;
    uint64_t lastWrite = ull.QuadPart;

    return lastWrite;
}

static bool IsFileLocked(char* path) {
    HANDLE hFile = CreateFileA(
        path,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return true; // assume that it's locked
    }

    OVERLAPPED overlapped = { 0 };
    if (LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, 0, 0, &overlapped)) {
        CloseHandle(hFile);
        return false;
    } else {
        CloseHandle(hFile);
        return true;
    }
}

/*
 * Loads or reloads a library if it has changed.
 *
 * To make reloading work without running into file locks,
 * the DLL is copied to a temp file and that temp file
 * is the DLL actually being loaded.
 *
 * Reloading is mainly for debug builds and quick iteration,
 * but for now the copy step is always enabled.
 */
static bool LoadLibraryInternal(char* path, DynamicLibrary* lib) {
    char tempName[MAX_PATH];

    bool isFirstLoad = lib->lastWrite == 0;

    uint64_t lastWrite = LastFileWrite(path);
    if (lastWrite == 0 || lib->lastWrite == lastWrite) {
        return false;
    }

    if (IsFileLocked(path)) {
        return false;
    }

    if (!isFirstLoad) {
        bool didFree = FreeLibrary(lib->handle);
        Assert(didFree, "Unable to free DLL when reloading");
    }

    int len = strlen(path);
    strncpy(tempName, path, len - 4);
    tempName[len - 4] = '\0';
    strcat(tempName, "_temp.dll");

    bool didCopy = CopyFileA(path, tempName, false);
    if (!didCopy) {
        DWORD err = GetLastError();
        AssertFail("Unable to copy DLL\nsource = %s\ndest = %s\nerror = %d", path, tempName, err);
    }

    HMODULE handle = LoadLibraryA(tempName);
    Assert(handle != NULL, "Unable to load DLL %s", tempName);

    lib->handle = handle;
    lib->lastWrite = lastWrite;

    return true;
}

static void* LoadLibraryFunction(char* name, DynamicLibrary* lib) {
    if (lib->handle == NULL) {
        return NULL;
    }
    return GetProcAddress(lib->handle, name);
}
