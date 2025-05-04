@echo off

mkdir bin > NUL 2>&1

call .\scripts\build_game_update.bat

cl src\game\game_main.c ^
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
