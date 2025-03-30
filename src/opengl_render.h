/*
 * This is the OpenGL rendering backend. It relies on version 3.3.0.
 *
 * The basic idea is to provide utilities like DrawTriangle and then
 * internally keep track of buffering and batching draw calls.
 */

#ifndef opengl_render_h
#define opengl_render_h

// make sure windows.h is always included before OpenGL
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif
#include <gl/gl.h>

#define GL_VERSION_3_3 1
#define GL_GLEXT_PROTOTYPES
#include <gl/glext.h>

#include "common.h"

// OpenGL extensions that are dynamically loaded by the platform layer (assumes version 3.3)
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

// internal utilities for platform code
void InitGraphicsGl(OpenGlExt openglExt); // call at window creation
void EndDrawGl(); // call before swapping buffers
void SetResolutionGl(int width, int height); // call at window resize

// render API
void ClearScreenGl(Color color);
void DrawTriangleGl(Vec2 a, Vec2 b, Vec2 c, Color color);
void SetTransformGl(Mat4 mat);

#endif
