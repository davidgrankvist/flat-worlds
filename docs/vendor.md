# Vendor

Summary of vendor/ directory contents.

## OpenGL

Even when OpenGL is supported by a platform, many functions are not declared in the core `<gl/gl.h>` header.
Those functions need to be loaded dynamically and called via function pointers.

There are some extension headers that define the types of the dynamically loaded functions. These are helpful for setting up the function pointers and figuring which functions are available in which OpenGL version.

### Includes

| Include | Purpose | Location | Source |
| ------- | ------- | -------- | ------ |
| `#include <gl/glext.h>` | Render backend calls. Platform independent, but varies for different OpenGL versions.| vendor/include/gl/glext.h | [OpenGL registry](https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/glext.h) |
| `#include <KHR/khrplatform.h>` | This is a dependency for glext.h.  | vendor/include/KHR/khrplatform.h | [Khronos registry](https://registry.khronos.org/EGL/api/KHR/khrplatform.h) |
| `#include <gl/wglext.h>` | WGL calls. This is for Windows specific OpenGL initialization.  | vendor/include/gl/wglext.h | [OpenGL registry](https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/wglext.h) |
