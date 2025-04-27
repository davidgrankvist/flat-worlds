@echo off

mkdir bin > NUL 2>&1

cl src\game\*.c ^
    /Fe: bin\game.exe ^
    /Fo: bin\ ^
    /Fd: bin\ ^
    /Zi ^
    /I"src\lib\include" ^
    /I"src\game" ^
    /link ^
        /LIBPATH:bin ^
        libgame.lib ^
        /SUBSYSTEM:WINDOWS ^
    /nologo
