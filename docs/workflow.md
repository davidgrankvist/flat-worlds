# Workflow

This is also inspired by Handmade Hero. 

Basically I have a text editor and a build script that runs the compiler. If I need to debug then I run the binary with a debugger attached. It's a simple setup to focus on the game programming instead of complex build systems and tools.

More concretely:
- Neovim for editing
- cl.exe for compiling
- Visual Studio for debugging
- RenderDoc for debugging graphics

## Neovim

There is a local .nvimrc to set up different :make commands. This is to be able to selectively build what's currently relevant.
For example, if you tune some game parameters, you may want :make to just rebuild the game update.
