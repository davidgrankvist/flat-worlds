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

## Graphics

### Default shader program

For simple rendering, the client code may not need a custom shader. Instead, your render backend can provide high-level functions for drawing shapes
and pass those as input to a default shader program.

The default vertex shader can take in the vertex position, color, a custom transform and other projections.
This way, the client code can pass in a lot of different data but still only run the default shader program.

### Orthographic projection

In a game, there are different approaches to coordinate systems. A 2D game may use screen coordinates with an inverted Y axis.
A 3D game may use more abstract world coordinates, which has different conventions for the orientation.

Graphics APIs need a way to figure out where on the screen to draw things, regardless of 2D or 3D graphics.
This also needs to be consistent across resolutions, so they use something called Normalized Device Coordinates (NDC).
In OpenGL, this is in the -1 to 1 range. To convert to NDC, there is a useful transform called orthographic projection. 
It is flexible enough that you can decide things like Y axis direction.

The default shader program can apply orthographic projection so that even though vertex data
is in world coordinates everything is normalized. This simplifies both the client code and
the render backend, as the normalization happens in one place. The client code passes in
world coordinates, the render backend sends world coordinates to the GPU and the shader
program normalizes it.

### Batching draw calls

Each draw call comes with overhead, so ideally you want to make a few draw calls with lots of vertex data.

One simple approach to batching is to preallocate a buffer that is populated with vertex data each frame. This can for example
be vertices that should be drawn as triangles. When it's time to draw, you can check how much of the buffer was populated during
that frame, transmit all of that data at once and issue the draw call.

Sometimes you need multiple draw calls, for example when you want to change the value of a uniform or run different shader programs.
In that case, you can have a similar scheme but also keep track of a start index for the next draw call.
