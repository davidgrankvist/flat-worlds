# Vendor

Summary of vendor/ directory contents.

## OpenGL Extensions

Even when OpenGL is supported by a platform, many functions are not declared in the core `<gl/gl.h>` header.
These features are called OpenGL extensions and can be loaded dynamically with functions like wglGetProcAddress.

They don't require additionally linking, but the extension headers help with declaring a lot of types and grouping them by OpenGL version.

### Includes

Include: `#include <gl/glext.h>`
Purpose: Extensions for render backend calls. Platform independent, but varies for different OpenGL versions.
Location: vendor/include/gl/glext.h
Source: <https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/glext.h>

Include: `#include <KHR/khrplatform.h>`
Purpose: This is a dependency for glext.h.
Location: vendor/include/KHR/khrplatform.h
Source: <https://registry.khronos.org/EGL/api/KHR/khrplatform.h>

Include: `#include <gl/wglext.h>`
Purpose: Extensions WGL, Windows specific OpenGL initialization.
Location: vendor/include/gl/wglext.h
Source: <https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/wglext.h>
