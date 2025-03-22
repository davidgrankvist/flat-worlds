/*
 * This is the win32 entrypoint. It sets up win32 specific functions and passes them as function pointers to GameMain where the game code runs.
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <gl/gl.h>
#include "game_main.h"

// -- Platform layer API --

void InitWindow();
bool IsWindowOpen();
void CloseCurrentWindow();

void InitConsole();

void ProcessInput();
bool IsKeyDown(InputKey key);
bool IsKeyPressed(InputKey key);
bool IsKeyReleased(InputKey key);

void MakeDrawCall();
void ClearScreen(float r, float g, float b, float a);

// -- Internal constants (to make platform API simple for consumer) --
HINSTANCE windowHInstance;
int windowNCmdShow;
MSG msg = {};
bool shouldRun = true;
HDC windowHdc;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        PWSTR pCmdLine, int nCmdShow) {
    windowHInstance = hInstance;
    windowNCmdShow = nCmdShow;

    Platform platform = {};
    platform.InitWindow = InitWindow;
    platform.IsWindowOpen = IsWindowOpen;
    platform.CloseCurrentWindow = CloseCurrentWindow;

    platform.InitConsole = InitConsole;

    platform.ProcessInput = ProcessInput;
    platform.IsKeyDown = IsKeyDown;
    platform.IsKeyPressed = IsKeyPressed;
    platform.IsKeyReleased = IsKeyReleased;

    Render render = {};
    render.MakeDrawCall = MakeDrawCall;
    render.ClearScreen = ClearScreen;
    platform.render = &render;

    return GameMain(&platform);
}

// -- Window --

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HGLRC InitOpenGl(HDC windowHdc);

InputKey MapKeyCode(WPARAM wParam, LPARAM lParam);
void SetKeyDown(InputKey key);
void SetKeyUp(InputKey key);

void InitWindow() {
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

bool IsWindowOpen() {
    return shouldRun;
}

void CloseCurrentWindow() {
    shouldRun = false;
}

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            SetKeyDown(MapKeyCode(wParam, lParam));
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            SetKeyUp(MapKeyCode(wParam, lParam));
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HGLRC InitOpenGl(HDC windowHdc) {
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

    HGLRC hglrc = wglCreateContext(windowHdc);
    wglMakeCurrent(windowHdc, hglrc);

    return hglrc;
}

// -- Console --

void InitConsole() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "r", stdin);
}

// -- Input --

/*
 * The input buffer is packed into a u64 with one bit per key.
 * bit set in inputKeys[0] = currently down.
 * bit set in inputKeys[1] = previously down.
 *
 * This works as long as the InputKey enum has at most 64 values.
 */
uint64_t inputKeys[2];
#define KEY_TO_BIT(k) (1 << k)
#define IS_KEY_SET(k, input) (input & KEY_TO_BIT(k))
void UpdateInputBuffers();


void ProcessInput() {
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
InputKey MapKeyCode(WPARAM wParam, LPARAM lParam) {
    int vk = wParam;
    int scanCode = (lParam >> 16) & 0xFF;

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

void UpdateInputBuffers() {
    inputKeys[1] = inputKeys[0];
}

void SetKeyDown(InputKey key) {
    inputKeys[0] |= KEY_TO_BIT(key);
}

void SetKeyUp(InputKey key) {
    inputKeys[0] &= ~KEY_TO_BIT(key);
}

bool IsKeyDown(InputKey key) {
    return IS_KEY_SET(key, inputKeys[0]);
}

bool IsKeyPressed(InputKey key) {
    return IS_KEY_SET(key, inputKeys[0])
        && !IS_KEY_SET(key, inputKeys[1]);
}

bool IsKeyReleased(InputKey key) {
    return !IS_KEY_SET(key, inputKeys[0])
        && IS_KEY_SET(key, inputKeys[1]);
}

// -- Render --

void MakeDrawCall() {
    SwapBuffers(windowHdc);
}

void ClearScreen(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}
