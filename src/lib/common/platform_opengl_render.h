/*
 * Platform setup - OpenGL
 *
 * Initialize OpenGL before the public render backend functions are called.
 *
 */
#ifndef platform_opengl_render_h
#define platform_opengl_render_h

#include "platform_include.h"

// -- Version specific definitions --

#ifdef LIBGAME_OPENGL_RENDER_330
    #include <gl/gl.h>

    // set up glext.h
    #define GL_VERSION_3_3 1
    #define GL_GLEXT_PROTOTYPES

    #include <gl/glext.h>

    // OpenGL extensions that are dynamically loaded by the platform layer (assumes 3.3.0)
    typedef struct {
        PFNGLBINDBUFFERPROC glBindBuffer;
        PFNGLGENBUFFERSPROC glGenBuffers;
        PFNGLBUFFERDATAPROC glBufferData;
        PFNGLATTACHSHADERPROC glAttachShader;
        PFNGLCOMPILESHADERPROC glCompileShader;
        PFNGLCREATEPROGRAMPROC glCreateProgram;
        PFNGLCREATESHADERPROC glCreateShader;
        PFNGLDELETESHADERPROC glDeleteShader;
        PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
        PFNGLLINKPROGRAMPROC glLinkProgram;
        PFNGLSHADERSOURCEPROC glShaderSource;
        PFNGLUSEPROGRAMPROC glUseProgram;
        PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
        PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
        PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
        PFNGLGETSHADERIVPROC glGetShaderiv;
        PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
        PFNGLGETPROGRAMIVPROC glGetProgramiv;
        PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
        PFNGLBUFFERSUBDATAPROC glBufferSubData;
        PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
        PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
    } OpenGlExt;
#else
    #error "No supported OpenGL version was enabled."
#endif

// internal utilities for platform code
void InitGraphicsGl(OpenGlExt openglExt); // call at window creation

#endif
