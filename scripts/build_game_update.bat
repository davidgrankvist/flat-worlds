@echo off
setlocal enabledelayedexpansion

REM Builds the game update. This DLL can be rebuilt while the game is running.
REM The PDB is assigned a random name each time, because the debugger will lock it.

set SRC_FILES_NO_MAIN=
for %%f in (src\game\*.c) do (
    if /I not "%%f" == "src\game\game_main.c" (
        set SRC_FILES_NO_MAIN=!SRC_FILES_NO_MAIN! %%f
    )
)

mkdir bin > NUL 2>&1

del /Q bin\game_update_temp_*.pdb > NUL 2>&1

cl %SRC_FILES_NO_MAIN% ^
    /LD ^
    /Fe: bin\game_update.dll ^
    /Fo: bin\ ^
    /Zi ^
    /Fd: bin\ ^
    /I"src\lib\include" ^
    /I"src\game" ^
    /link ^
        /LIBPATH:bin ^
        libgame.lib ^
        /DEBUG ^
        /PDB:bin\game_update_temp_%random%.pdb ^
        /INCREMENTAL:NO ^
    /nologo
