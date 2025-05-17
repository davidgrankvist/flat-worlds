#include <stdlib.h>
#include "platform_opengl_render.h"
#include "opengl_render.h"
#include "libgame_math.h"
#include "camera.h"
#include "asserts.h"

// provided by platform layer
static OpenGlExt openGlExt;
static int clientWidth = 0;
static int clientHeight = 0;

/*
 * For simplicity, there is just one big buffer of triangle vertices.
 *
 * The vertex count resets each frame. When a draw call is invoked, 
 * the range of vertices specified by currentVertexCount and currentVertexStart 
 * are included in the draw call.
 */
static GLuint VAO, VBO;
static GLfloat* vertices = NULL;
static int maxVertices = 1000; // constant size for now
static int valuesPerVertex = 7; // 3 coordinates + 4 color channels
static int currentVertexCount = 0; // total vertex count in frame
static int currentVertexStart = 0; // start index for individual draw calls

static GLuint EBO;
static GLuint* vertexIndices = NULL;
static int maxVertexIndices = 1000;
static int currentVertexIndexCount = 0;
static int currentVertexIndexStart = 0;

/*
 * The default shader program does following:
 * - apply a user defined transform (defaults to the identity matrix)
 * - apply a camera transform (either 2D or 3D)
 * - pass through the given position and color
 */
static const char* defaultVertexShaderSrc = "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec4 color;\n"
    "uniform mat4 cameraTransform;\n"
    "uniform mat4 transform;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    gl_Position = cameraTransform * transform * vec4(position, 1.0);\n"
    "    fragColor = color;\n"
    "}";

static const char* defaultFragmentShaderSrc = "#version 330 core\n"
    "in vec4 fragColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = fragColor;\n"
    "}";
static GLuint defaultShaderProgram;
static GLint cameraTransformLoc;
static GLint transformLoc;

// OpenGL friendly flattened 4x4 matrix
typedef struct {
    float m[16]; 
} RenderTransform;
static RenderTransform defaultTransform = {0};

static RenderTransform Mat4ToRenderTransform(Mat4 mat);
static void ResetTransform();
static void UpdateCameraTransform();

static const char* MapOpenGlError(GLenum err) {
    switch(err) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:
            return "GL_CONTEXT_LOST";
        case GL_TABLE_TOO_LARGE:
            return "GL_TABLE_TOO_LARGE";
        default:
            return "Unknown";
    }
}

static void AssertNoGlErrorFn(char* msg, int line) {
    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        return;
    }
    const char* errStr = MapOpenGlError(err);
    AssertFn(false, __FILE__, line, "%s: OpenGL error. glGetError returned code %s (0x%.4x)", msg, errStr, err);
}

#define AssertNoGlError(msg) \
    AssertNoGlErrorFn(msg, __LINE__)

void SetResolutionGl(int width, int height) {
    glViewport(0, 0, width, height);
    clientWidth = width;
    clientHeight = height;
    SetCameraClientArea(width, height);
    UpdateCameraTransform();
}

void InitGraphicsGl(OpenGlExt ext) {
    openGlExt = ext;
    int success;

    // -- Default shaders --

    GLuint vertexShader = openGlExt.glCreateShader(GL_VERTEX_SHADER);
    openGlExt.glShaderSource(vertexShader, 1, &defaultVertexShaderSrc, NULL);
    openGlExt.glCompileShader(vertexShader);
    openGlExt.glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    Assert(success, "Failed to compile vertex shader");

    GLuint fragmentShader = openGlExt.glCreateShader(GL_FRAGMENT_SHADER);
    openGlExt.glShaderSource(fragmentShader, 1, &defaultFragmentShaderSrc, NULL);
    openGlExt.glCompileShader(fragmentShader);
    openGlExt.glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    Assert(success, "Failed to compile fragment shader");

    defaultShaderProgram = openGlExt.glCreateProgram();
    openGlExt.glAttachShader(defaultShaderProgram, vertexShader);
    openGlExt.glAttachShader(defaultShaderProgram, fragmentShader);
    openGlExt.glLinkProgram(defaultShaderProgram);
    openGlExt.glGetProgramiv(defaultShaderProgram, GL_LINK_STATUS, &success);
    Assert(success, "Failed to link shader program");

    openGlExt.glDeleteShader(vertexShader);
    openGlExt.glDeleteShader(fragmentShader);

    openGlExt.glUseProgram(defaultShaderProgram);

    cameraTransformLoc = openGlExt.glGetUniformLocation(defaultShaderProgram, "cameraTransform");
    UpdateCameraTransform();
    transformLoc = openGlExt.glGetUniformLocation(defaultShaderProgram, "transform");
    defaultTransform = Mat4ToRenderTransform(Mat4Identity());
    ResetTransform();

    // -- Vertex buffer for triangles --

    openGlExt.glGenVertexArrays(1, &VAO);

    // VBO
    openGlExt.glGenBuffers(1, &VBO);
    openGlExt.glBindVertexArray(VAO);
    openGlExt.glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int maxVertexBufferSize = maxVertices * valuesPerVertex * sizeof(GLfloat);
    vertices = (GLfloat*)malloc(maxVertexBufferSize);
    Assert(vertices != NULL, "Failed to allocate vertex buffer");

    openGlExt.glBufferData(GL_ARRAY_BUFFER, maxVertexBufferSize, vertices, GL_DYNAMIC_DRAW);

    // EBO
    openGlExt.glGenBuffers(1, &EBO);
    openGlExt.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    int maxVertexIndexBufferSize = maxVertexIndices * sizeof(GLuint);
    vertexIndices = (GLuint*)malloc(maxVertexIndexBufferSize);
    Assert(vertexIndices != NULL, "Failed to allocate vertex index buffer");

    openGlExt.glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxVertexIndexBufferSize, vertexIndices, GL_DYNAMIC_DRAW);

    // position attribute
    openGlExt.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, valuesPerVertex * sizeof(GLfloat), (GLvoid*)0);
    openGlExt.glEnableVertexAttribArray(0);
    // color attribute
    openGlExt.glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, valuesPerVertex * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    openGlExt.glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    AssertNoGlError("Failed to initialize OpenGL");
}

static inline int Mat4PosToIndex(int x, int y) {
    return x * 4 + y; // transpose, because OpenGL matrices are column-major
}

static RenderTransform Mat4ToRenderTransform(Mat4 mat) {
    RenderTransform trans = {{0}};
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            trans.m[Mat4PosToIndex(x, y)] = mat.m[y][x];
        }
    }

    return trans;
}

void SetTransformGl(Mat4 mat) {
    RenderTransform transform = Mat4ToRenderTransform(mat);
    openGlExt.glUniformMatrix4fv(transformLoc, 1, false, transform.m);
}

static void ResetTransform() {
    openGlExt.glUniformMatrix4fv(transformLoc, 1, false, defaultTransform.m);
}

void SetCamera2DGl(Camera2D* camera) {
    SetCameraTransform2D(camera);
    UpdateCameraTransform();
}

void SetCamera3DGl(Camera3D* camera) {
    SetCameraTransform3D(camera);
    UpdateCameraTransform();
}

static void UpdateCameraTransform() {
    Mat4 mat = GetCameraTransform();
    RenderTransform transform = Mat4ToRenderTransform(mat);
    openGlExt.glUniformMatrix4fv(cameraTransformLoc, 1, false, transform.m);
}

void EndDrawGl() {
    //  update vertices
    int length = currentVertexCount - currentVertexStart;
    int offset = currentVertexStart * valuesPerVertex * sizeof(GLfloat);
    int size = length * valuesPerVertex * sizeof(GLfloat);
    openGlExt.glBufferSubData(GL_ARRAY_BUFFER, offset, size, &vertices[currentVertexStart * valuesPerVertex]);

    // update indicies
    int indexLength = currentVertexIndexCount - currentVertexIndexStart;
    int indexOffset = currentVertexIndexStart * sizeof(GLuint);
    int indexSize = indexLength * sizeof(GLuint);
    openGlExt.glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexOffset, indexSize, &vertexIndices[currentVertexIndexStart]);

    glDrawElements(GL_TRIANGLES, indexLength, GL_UNSIGNED_INT, (void*)(uintptr_t)currentVertexIndexStart);

    AssertNoGlError("Failed to draw");
    ResetTransform();
    currentVertexStart = currentVertexCount;
    currentVertexIndexStart = currentVertexIndexCount;
}

void EndFrameGl() {
    currentVertexCount = 0;
    currentVertexStart = 0;
    currentVertexIndexCount = 0;
    currentVertexIndexStart = 0;
}

void ClearScreenGl(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 }

void DrawTriangle3DGl(Vec3 a, Vec3 b, Vec3 c, Color color) {
    int targetVertexCount = currentVertexCount + 3;
    int targetVertexIndexCount = currentVertexIndexCount + 3;
    Assert(targetVertexCount <= maxVertices, "Too many vertices (%d). Max is %d.", targetVertexCount, maxVertices);
    Assert(targetVertexIndexCount <= maxVertexIndices, "Too many vertex indices (%d). Max is %d.", targetVertexIndexCount, maxVertexIndices);

    GLfloat verticesToAdd[] = {
        a.x, a.y, a.z, color.r, color.g, color.b, color.a,
        b.x, b.y, b.z, color.r, color.g, color.b, color.a,
        c.x, c.y, c.z, color.r, color.g, color.b, color.a
    };
    GLuint vertexIndicesToAdd[] = {
        currentVertexIndexCount,
        currentVertexIndexCount + 1,
        currentVertexIndexCount + 2,
    };

    memcpy(&vertices[currentVertexCount * valuesPerVertex], verticesToAdd, sizeof(verticesToAdd));
    memcpy(&vertexIndices[currentVertexIndexCount], vertexIndicesToAdd, sizeof(vertexIndicesToAdd));

    currentVertexCount += 3;
    currentVertexIndexCount += 3;
}

void DrawTriangle2DGl(Vec2 a, Vec2 b, Vec2 c, Color color) {
    DrawTriangle3DGl((Vec3){ a.x, a.y, 0}, (Vec3){ b.x, b.y, 0}, (Vec3){ c.x, c.y, 0}, color);
}
