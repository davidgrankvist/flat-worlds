@echo off

mkdir bin > NUL 2>&1

call .\scripts\build_game_update.bat

cl src\game_main.c ^
    /Fe: bin\game.exe ^
    /Fo: bin\ ^
    /Fd: bin\ ^
    /Zi ^
    /I"libgame\src\include" ^
    /I"src" ^
    /link ^
        /LIBPATH:bin ^
        /LIBPATH:"libgame\bin" ^
        libgamedll.lib ^
        /SUBSYSTEM:WINDOWS ^
    /nologo
