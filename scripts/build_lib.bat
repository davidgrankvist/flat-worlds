@echo off

call .\libgame\scripts\build_win32.bat debug dynamic
copy /Y .\libgame\bin\libgame.dll .\bin\libgame.dll
