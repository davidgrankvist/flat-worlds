@echo off

mkdir bin > NUL 2>&1

cl src\*.c ^
    /Fe: bin\game.exe ^
    /Fo: bin\ ^
    /Fd: bin\ ^
    /Zi ^
    /I"vendor\include" ^
    /link ^
        user32.lib ^
        winmm.lib ^
        gdi32.lib ^
        opengl32.lib
