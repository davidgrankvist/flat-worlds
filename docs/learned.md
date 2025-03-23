# Learned

Things I've learned.

## Platform Layer

### Main structure

Some platforms require a specific main function, like WinMain on Windows. To deal with this,
you can structure the code like this:

1. have a platform-specific main like `win32_main.c`
2. set up function pointers that invoke platform specific code
3. pass in the pointers into a GameMain that runs the game code

This way the platform owns the real entrypoint, but the game code is still able to call
platform APIs.

### Input

When you press a key, it will be reported to the OS using a platform-specific virtual key code.
You can map those to your own platform-independent key codes and then store input states
based on that. This allows you to share input state management between different platforms.

### OpenGL

OpenGL has two parts:

1. platform-specific initialization
2. platform-independent render backend calls (if the version is supported)

Each platform is responsible for things like setting up the OpenGL context. The render backend can be shared between some platforms, 
but it is not entirely platform-independent since different platforms support different versions of OpenGL.
For example OpenGL 3.3.0 is "desktop friendly", but not "mobile friendly".
