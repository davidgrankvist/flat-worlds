#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <gl/gl.h>

LRESULT CALLBACK WindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HGLRC InitOpenGl(HDC windowHdc);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        PWSTR pCmdLine, int nCmdShow) {
    const wchar_t className[] = L"WindowClassName";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindProc;
    wc.hInstance = hInstance;
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
            hInstance,
            NULL);

    if (hwnd == NULL) {
        return 0;
    }

    HDC windowHdc = GetDC(hwnd);
    HGLRC hglrc = InitOpenGl(windowHdc);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SwapBuffers(windowHdc);
    }

    return 0;
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
