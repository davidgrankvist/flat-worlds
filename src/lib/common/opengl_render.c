/*
 * The rendering backend has the following structure.
 *
 * BUFFERS
 *
 * Draw calls are batched using one big buffer of triangle vertices.
 * There is an additional vertex index buffer to avoid redundant
 * vertex data (for example when drawing quads).
 *
 * The two buffers are allocated ahead of time, with configurable sizes.
 *
 * The vertex count resets each frame. When a draw call is invoked,
 * the range of vertices specified by currentVertexCount and currentVertexStart
 * are included in the draw call. The vertex indices are handled similarly.
 * The start offsets are also reset at the end of the frame.
 *
 * SHADERS
 *
 * The default shader program does following:
 * - apply a user defined transform (defaults to the identity matrix)
 * - apply a camera transform (either 2D or 3D)
 * - pass through the given position and color
 */
#include <stdlib.h>
#include "platform_setup.h"
#include "opengl_render.h"
#include "libgame_math.h"
#include "camera.h"
#include "asserts.h"

// provided by platform layer
static OpenGlExt openGlExt;
static int clientWidth = 0;
static int clientHeight = 0;

static GLuint VAO, VBO;
static GLfloat* vertices = NULL;
static int maxVertices = LIBGAME_DEFAULT_MAX_VERTICES;
static int valuesPerVertex = 7; // 3 coordinates + 4 color channels
static int currentVertexCount = 0;
static int currentVertexStart = 0;

static GLuint EBO;
static GLuint* vertexIndices = NULL;
static int maxVertexIndices = LIBGAME_DEFAULT_MAX_INDICES;
static int currentVertexIndexCount = 0;
static int currentVertexIndexStart = 0;

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

void ConfigureRenderGl(RenderSettings settings) {
    Assert(vertices == NULL, "Unable to configure max vertices. "
            "The vertex buffer has already been initialized. "
            "This can happen if you have already created a window.");
    maxVertices = settings.maxVertices;
    maxVertexIndices = settings.maxVertexIndices;
}

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

static void AssertCountWithinBounds(int targetVertexCount, int targetVertexIndexCount) {
    Assert(targetVertexCount <= maxVertices, "Too many vertices (%d). Max is %d.", targetVertexCount, maxVertices);
    Assert(targetVertexIndexCount <= maxVertexIndices, "Too many vertex indices (%d). Max is %d.", targetVertexIndexCount, maxVertexIndices);
}

typedef struct {
   Vec3* positions;
   Color* colors;
   int vertexCount;
   int* indices;
   int indexCount;
} Mesh;

static void DrawMesh(Mesh mesh) {
    int targetVertexCount = currentVertexCount + mesh.vertexCount;
    int targetVertexIndexCount = currentVertexIndexCount + mesh.indexCount;
    AssertCountWithinBounds(targetVertexCount, targetVertexIndexCount);

    for (int i = 0; i < mesh.vertexCount; i++) {
        Vec3 pos = mesh.positions[i];
        Color color = mesh.colors[i];
        int iBuffer = (currentVertexCount + i) * valuesPerVertex;
        vertices[iBuffer] = pos.x;
        vertices[iBuffer + 1] = pos.y;
        vertices[iBuffer + 2] = pos.z;
        vertices[iBuffer + 3] = color.r;
        vertices[iBuffer + 4] = color.g;
        vertices[iBuffer + 5] = color.b;
        vertices[iBuffer + 6] = color.a;
    }

    for (int i = 0; i < mesh.indexCount; i++) {
        vertexIndices[currentVertexIndexCount + i] = mesh.indices[i] + currentVertexCount;
    }

    currentVertexCount = targetVertexCount;
    currentVertexIndexCount = targetVertexIndexCount;
}

void DrawTriangle3DGl(Vec3 a, Vec3 b, Vec3 c, Color color) {
    Vec3 positions[3] = { a, b, c };
    Color colors[3] = { color, color, color };
    int indices[3] = { 0, 1, 2 };

    Mesh mesh = {0};
    mesh.positions = positions;
    mesh.colors = colors;
    mesh.vertexCount = 3;
    mesh.indices = indices;
    mesh.indexCount = 3;

    DrawMesh(mesh);
}

void DrawTriangle2DGl(Vec2 a, Vec2 b, Vec2 c, Color color) {
    DrawTriangle3DGl((Vec3){ a.x, a.y, 0}, (Vec3){ b.x, b.y, 0}, (Vec3){ c.x, c.y, 0}, color);
}

void DrawQuad3DGl(Vec3 topLeft, Vec3 topRight, Vec3 bottomLeft, Vec3 bottomRight, Color color) {
    Vec3 positions[4] = { topLeft, topRight, bottomLeft, bottomRight };
    Color colors[4] = { color, color, color, color };
    int indices[6] = {
        0, 1, 2, // upper triangle
        2, 1, 3, // lower triangle
    };

    Mesh mesh = {0};
    mesh.positions = positions;
    mesh.colors = colors;
    mesh.vertexCount = 4;
    mesh.indices = indices;
    mesh.indexCount = 6;

    DrawMesh(mesh);
}
