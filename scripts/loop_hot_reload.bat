@echo off

:loop
call .\scripts\build_game_update.bat
timeout 2
goto loop
