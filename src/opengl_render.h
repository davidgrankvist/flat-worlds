#ifndef opengl_render_h
#define opengl_render_h

// make sure windows.h is always included before OpenGL
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif
#include <gl/gl.h>

void ClearScreenGl(float r, float g, float b, float a);

#endif
