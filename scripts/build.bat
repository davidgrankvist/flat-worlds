@echo off

mkdir bin > NUL 2>&1

cl src\win32_main.c ^
    /Fe: bin\ ^
    /Fo: bin\ ^
    /Fd: bin\ ^
    /Zi ^
    /link ^
        user32.lib ^
        gdi32.lib ^
        opengl32.lib
