#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <gl/gl.h>
#include "game_main.h"

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitWindow();
bool IsWindowOpen();
HGLRC InitOpenGl(HDC windowHdc);
void ProcessInput();
void MakeDrawCall();
void ClearScreen(float r, float g, float b, float a);

HINSTANCE windowHInstance;
int windowNCmdShow;
MSG msg = {};
bool shouldRun = true;
HDC windowHdc;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        PWSTR pCmdLine, int nCmdShow) {
    windowHInstance = hInstance;
    windowNCmdShow = nCmdShow;

    Render render = {};
    render.MakeDrawCall = MakeDrawCall;
    render.ClearScreen = ClearScreen;

    Platform platform = {};
    platform.InitWindow = InitWindow;
    platform.IsWindowOpen = IsWindowOpen;
    platform.ProcessInput = ProcessInput;
    platform.render = &render;

    return GameMain(&platform);
}

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

void ProcessInput() {
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            shouldRun = false;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void MakeDrawCall() {
    SwapBuffers(windowHdc);
}

void ClearScreen(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
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
