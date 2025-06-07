# Developer Guide

## Code Structure

### Library

- lib/ - the engine code
- lib/include - public headers
- lib/common - platform independent shared utils
- lib/platform - platform specific library entrypoint

The platform entrypoint is responsible for:
1. initializing things for the lib/common code (setting up OpenGL, etc.)
2. calling internal lib/common functions at the right time in response to platform events (input, draw, etc.)

#### Porting

For the most part function pointers are used to avoid a ton of ifdefs.

To port, you need to:
- add a new library entrypoint in lib/platform
- set up the function pointers defined in lib/common/platform_setup.h
- update a couple of ifdefs in lib/include/libgame.h
- call platform independent utilities at the right time (for example responding to keyboard input)

To target a specific OpenGL version, you need to define a macro at build time. The macro is used in lib/common/platform_setup.h to define which OpenGL functions to load dynamically.

### Game

- game/game_main.c - sets up the game loop and owns the game state
- game/game_update.c - per frame update code

The game update is loaded dynamically to support hot reloading while the game is running.
