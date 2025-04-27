/*
 * This is the OpenGL rendering backend.
 *
 * The basic idea is to provide utilities like DrawTriangle and then
 * internally keep track of buffering and batching draw calls.
 */

#ifndef opengl_render_h
#define opengl_render_h

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

// -- Utilities for the platform code --

#include "libgame.h"

// internal utilities for platform code
void InitGraphicsGl(OpenGlExt openglExt); // call at window creation
void EndDrawGl(); // call before swapping buffers
void SetResolutionGl(int width, int height); // call at window resize
void SetCamera2DGl(Camera2D* camera);
void SetCamera3DGl(Camera3D* camera);

// render API
void ClearScreenGl(Color color);
void DrawTriangle2DGl(Vec2 a, Vec2 b, Vec2 c, Color color);
void DrawTriangle3DGl(Vec3 a, Vec3 b, Vec3 c, Color color);
void SetTransformGl(Mat4 mat);
void EndFrameGl();

#endif
