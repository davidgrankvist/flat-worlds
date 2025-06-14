@echo off

call .\libgame\scripts\build_win32_debug.bat
copy /Y .\libgame\bin\libgame.dll .\bin\libgame.dll
