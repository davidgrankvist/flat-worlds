@echo off

mkdir bin > NUL 2>&1

call .\scripts\build_lib.bat

call .\scripts\build_game.bat
