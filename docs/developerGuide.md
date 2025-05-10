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
3. setting up public function pointers used by the game code

#### Porting

To port, add a new library entrypoint to lib/platform. Then have a look at the following headers:
- Headers in lib/common with "platform" in their name define internal functions
that the platform specific code should use. For example, initializing OpenGL.
- Public headers under lib/include with "platform" in their name also have platform specific definitions.
- The primary library header lib/include/libgame.h defines structs with public function pointers to set up.

To target a specific OpenGL version, you need to define a macro at build time. See lib/common/platform_opengl_render.h for available versions.

### Game

- game/game_main.c - sets up the game loop and owns the game state
- game/game_update.c - per frame update code

The game update is loaded dynamically to support hot reloading while the game is running.
