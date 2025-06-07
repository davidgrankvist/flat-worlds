/*
 * Platform setup - prepares the platform layer.
 *
 * Set up a platform by passing in implementations via function pointers.
 */

#ifndef platform_setup_h
#define platform_setup_h

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include <stdint.h>
#include "libgame.h"

// -- Window --

typedef struct {
    void (*InitWindow)(const char* title);
    void (*InitConsole)();
} PlatformWindow;

void InitPlatformWindow(PlatformWindow platformWindow);

void SetResolution(int clientWidth, int clientHeight);

// -- Input --

typedef struct {
    void (*ProcessInput)();
    void (*WarpMousePosition)(int x, int y);
} PlatformInput;

void InitPlatformInput(PlatformInput platformInput);

// -- Timing --

typedef struct {
    int64_t (*GetMicroTicks)();
    void (*MicroSleep)(int us);
} PlatformTiming;

void InitPlatformTiming(PlatformTiming timing);

// -- Graphics --

typedef struct {
    void (*Configure)(RenderSettings settings);
    void (*ClearScreen)(Color color);
    void (*MakeDrawCall)();
    void (*EndFrame)();
    void (*SetTransform)(Mat4 mat);
    void (*SetCamera2D)(Camera2D* camera);
    void (*SetCamera3D)(Camera3D* camera);
    void (*DrawTriangle2D)(Vec2 a, Vec2 b, Vec2 c, Color color);
    void (*DrawTriangle3D)(Vec3 a, Vec3 b, Vec3 c, Color color);
    void (*DrawQuad3D)(Vec3 topLeft, Vec3 topRight, Vec3 bottomLeft, Vec3 bottomRight, Color color);
} PlatformRender;

void InitPlatformRender(PlatformRender platformRender);

// -- Dynamic library loading --

typedef struct {
    void (*ResolvePath)(char* fileBaseName, FileExtensionType extension, char* out, int outSize);
    bool (*LoadDynamicLibrary)(char* libraryPath, DynamicLibrary* lib);
    void* (*LoadLibraryFunction)(char* functionName, DynamicLibrary* lib);
} PlatformLibraryLoader;

void SetPlatformLibraryLoader(PlatformLibraryLoader libraryLoader);

// -- OpenGL initialization --

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
    // assume that OpenGL is the only render backend for now
    #error "No supported OpenGL version was enabled."
#endif

void InitGraphicsGl(OpenGlExt openglExt); // call at window creation

#endif
