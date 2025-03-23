# Vendor

Summary of vendor/ directory contents.

## OpenGL

Even when OpenGL is supported by a platform, many functions are not declared in the core `<gl/gl.h>` header.
Those functions need to be loaded dynamically and called via function pointers.

There are some extension headers that define the types of the dynamically loaded functions. These are helpful for setting up the function pointers and figuring which functions are available in which OpenGL version.

### Includes

Include: `#include <gl/glext.h>`
Purpose: Render backend calls. Platform independent, but varies for different OpenGL versions.
Location: vendor/include/gl/glext.h
Source: <https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/glext.h>

Include: `#include <KHR/khrplatform.h>`
Purpose: This is a dependency for glext.h.
Location: vendor/include/KHR/khrplatform.h
Source: <https://registry.khronos.org/EGL/api/KHR/khrplatform.h>

Include: `#include <gl/wglext.h>`
Purpose: WGL calls. This is for Windows specific OpenGL initialization.
Location: vendor/include/gl/wglext.h
Source: <https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/wglext.h>
