@echo off

mkdir bin > NUL 2>&1

cl src\lib\platform\win32_main.c src\lib\common\*.c src\game\*.c ^
    /Fe: bin\game.exe ^
    /Fo: bin\ ^
    /Fd: bin\ ^
    /Zi ^
    /I"src\lib\include" ^
    /I"src\lib\common" ^
    /I"src\game" ^
    /I"vendor\include" ^
    /link ^
        user32.lib ^
        winmm.lib ^
        gdi32.lib ^
        opengl32.lib ^
    /nologo
