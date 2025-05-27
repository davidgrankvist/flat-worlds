/*
 * Platform - Win32
 *
 * This file is responsible for:
 * 1. initializing things for lib/common
 * 2. responding to events (input etc.)
 * 3. setting up internal function pointers to support the public functions in libgame.h
 *
 * The WinMain is defined in libgame_platform_main.h and is not included in the library build.
 */

#ifndef UNICODE
#define UNICODE
#endif

#include "platform_include.h"

#include <timeapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "opengl_render.h"
#include <gl/wglext.h>

#include "platform_render.h"
#include "platform_timing.h"
#include "platform_window.h"
#include "platform_library_load.h"

#include "libgame.h"
#include "input.h"
#include "asserts.h"

// -- Forward declarations (public API) --

/*
 * Used in WinMain to set up platform function pointers.
 */
void InitMainWin32();

// -- Forward declarations (internal) --

static void InitPlatformWin32();

static void InitPlatformWindowWin32();
static void InitInputWin32();
static void InitRenderGlWin32();
static void InitTimingWin32();
static void InitLibraryLoaderWin32();

// -- State --

HINSTANCE windowHInstance;
int windowNCmdShow;
MSG msg = {};
HWND windowHwnd;
HDC windowHdc;

static void InitConsoleWin32();

void InitMainWin32() {
    if (getenv("DEBUG_CONSOLE")) {
        InitConsoleWin32();
    }
    windowHInstance = GetModuleHandle(NULL);
    windowNCmdShow = SW_SHOWDEFAULT;

    InitPlatformWin32();
}

static void InitPlatformWin32() {
    InitPlatformWindowWin32();
    InitInputWin32();
    InitRenderGlWin32();
    InitTimingWin32();
    InitLibraryLoaderWin32();
}

// -- Window --

static void InitWindowWin32(const char* windowTitle);

static void InitPlatformWindowWin32() {
    PlatformWindow platformWindow = {};
    platformWindow.InitWindow = InitWindowWin32;
    platformWindow.InitConsole = InitConsoleWin32;
    InitPlatformWindow(platformWindow);
}

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HGLRC InitOpenGl(HDC windowHdc);
wchar_t* ConvertToWide(const char* input);

static void InitWindowWin32(const char* windowTitle) {
    const wchar_t className[] = L"WindowClassName";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindProc;
    wc.hInstance = windowHInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);

    windowHwnd = CreateWindowEx(
            0,
            className,
            ConvertToWide(windowTitle),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,
            NULL,
            windowHInstance,
            NULL);

    windowHdc = GetDC(windowHwnd);
    HGLRC hglrc = InitOpenGl(windowHdc);

    ShowWindow(windowHwnd, windowNCmdShow);
}

wchar_t* ConvertToWide(const char* input) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
    wchar_t* wideStr = (wchar_t*)malloc(size_needed * sizeof(wchar_t));
    if (!wideStr) return NULL;

    MultiByteToWideChar(CP_UTF8, 0, input, -1, wideStr, size_needed);
    return wideStr;
}

static void MapAndSetResolution(LPARAM lParam);
static InputKey MapKeyCode(WPARAM wParam, LPARAM lParam);
static void OnMouseMove(HWND hwnd, LPARAM lParam);
static void OnMouseLeave();

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
            OnMouseMove(hwnd, lParam);
            return 0;
        }
        case WM_MOUSELEAVE: {
            OnMouseLeave();
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
    int clientWidth = EXTRACT_LOW16(lParam);
    int clientHeight = EXTRACT_HIGH16(lParam);

    SetResolution(clientWidth, clientHeight);
    SetResolutionGl(clientWidth, clientHeight);
}

// -- Console --

void InitConsoleWin32() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "r", stdin);
}

// -- Input --

static void ProcessInputWin32() {
    UpdateInputBuffers();

    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            CloseCurrentWindow();
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

static void TrackMouseLeave(HWND hwnd) {
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hwnd;
    tme.dwHoverTime = 0;
    TrackMouseEvent(&tme);
}

static bool didMouseLeave = true;

static void OnMouseMove(HWND hwnd, LPARAM lParam) {
    TrackMouseLeave(hwnd);
    MapAndSetMousePosition(lParam);

    if (didMouseLeave) {
        SetMouseEnteredWindow();
        didMouseLeave = false;
    }
}

static void OnMouseLeave() {
    didMouseLeave = true;
}

static void WarpMousePositionWin32(int x, int y) {
    POINT pt = { x, y };
    ClientToScreen(windowHwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

static void InitInputWin32() {
    PlatformInput platformInput = {};
    platformInput.ProcessInput = ProcessInputWin32;
    platformInput.WarpMousePosition = WarpMousePositionWin32;
    InitPlatformInput(platformInput);
}

// -- Render --

static void MakeDrawCallGl() {
    EndDrawGl();
}

static void EndFrameGlWin32() {
    EndFrameGl();
    SwapBuffers(windowHdc);
}

static void InitRenderGlWin32() {
    PlatformRender render = {};
    render.Configure = ConfigureRenderGl;
    render.MakeDrawCall = MakeDrawCallGl;
    render.ClearScreen = ClearScreenGl;
    render.DrawTriangle2D = DrawTriangle2DGl;
    render.DrawTriangle3D = DrawTriangle3DGl;
    render.DrawQuad3D = DrawQuad3DGl;
    render.SetTransform = SetTransformGl;
    render.EndFrame = EndFrameGlWin32;
    render.SetCamera2D = SetCamera2DGl;
    render.SetCamera3D = SetCamera3DGl;
    InitPlatformRender(render);
}

// -- Timing --

static int64_t usPerMs = 1000;
static int64_t usPerTick = 0; // Dummy value. This is set in InitTimingWin32.

static void MicroSleep(int us);

static inline int64_t GetTicksWin32() {
    LARGE_INTEGER ticks;
    int success = QueryPerformanceCounter(&ticks);
    Assert(success, "Failed to call QPC");
    return ticks.QuadPart;
}

static inline int64_t GetMicroTicksWin32() {
    return GetTicksWin32() / usPerTick;
}

// combine millisecond sleep and busy-wait for higher precision
static void MicroSleepWin32(int us) {
    int64_t usStart = GetMicroTicksWin32();

    if (us < usPerMs) {
        while(GetMicroTicksWin32() - usStart < us) {}
        return;
    }

    // win32 sleep may overshoot, so intentionally undershoot by 1ms
    int loweredUs = us - usPerMs;

    if (loweredUs / usPerMs > 0 ) {
        Sleep(loweredUs / usPerMs);
    }

    while(GetMicroTicksWin32() - usStart < us) {}
}

static void InitTimingWin32() {
    MMRESULT result = timeBeginPeriod(1);
    Assert(result == TIMERR_NOERROR, "Unable to set sleep resolution to 1 ms");

    LARGE_INTEGER lpFrequency;
    bool success = QueryPerformanceFrequency(&lpFrequency);
    Assert(success, "Unable to check QPC frequency");

    int64_t ticksPerSecond = (int64_t)lpFrequency.QuadPart;
    Assert(ticksPerSecond >= TICKS_PER_SECOND, "Too low QPC frequency. Unable to use microsecond sleep.");

    usPerTick = ticksPerSecond / TICKS_PER_SECOND;

    PlatformTiming platformTiming = {};
    platformTiming.GetMicroTicks = GetMicroTicksWin32;
    platformTiming.MicroSleep = MicroSleepWin32;

    InitPlatformTiming(platformTiming);
}

// -- Dynamic loading --

static void ResolvePathWin32(char* name, FileExtensionType extension, char* out, int outSize);
static bool LoadDynamicLibraryWin32(char* name, DynamicLibrary* lib);
static void* LoadLibraryFunctionWin32(char* name, DynamicLibrary* lib);

static void InitLibraryLoaderWin32() {
    PlatformLibraryLoader libraryLoader = {};
    libraryLoader.ResolvePath = ResolvePathWin32;
    libraryLoader.LoadDynamicLibrary = LoadDynamicLibraryWin32;
    libraryLoader.LoadLibraryFunction = LoadLibraryFunctionWin32;
    SetPlatformLibraryLoader(libraryLoader);
}

static const char* GetFileExtension(FileExtensionType extension) {
    switch(extension) {
        case LibraryExtension: return ".dll";
        default: AssertFail("Unsupported file extension %d", extension);
    }

    return NULL;
}

static void ResolvePathWin32(char* name, FileExtensionType extension, char* out, int outSize) {
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
static bool LoadDynamicLibraryWin32(char* path, DynamicLibrary* lib) {
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

static void* LoadLibraryFunctionWin32(char* name, DynamicLibrary* lib) {
    if (lib->handle == NULL) {
        return NULL;
    }
    return GetProcAddress(lib->handle, name);
}
