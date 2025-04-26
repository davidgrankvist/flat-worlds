@echo off

mkdir bin > NUL 2>&1

cl src\platform\*.c src\game\*.c ^
    /Fe: bin\game.exe ^
    /Fo: bin\ ^
    /Fd: bin\ ^
    /Zi ^
    /I"src\platform" ^
    /I"src\game" ^
    /I"vendor\include" ^
    /link ^
        user32.lib ^
        winmm.lib ^
        gdi32.lib ^
        opengl32.lib ^
    /nologo
