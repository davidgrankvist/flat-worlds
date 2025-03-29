#include "opengl_render.h"
#include "mathz.h"

// provided by platform layer
OpenGlExt openGlExt;
int screenWidth = 0;
int screenHeight = 0;

/*
 * For simplicity, there is just one big buffer of triangle vertices.
 *
 * The vertex count resets each frame. At the end of the frame, 
 * the first currentVertexCount vertices in the buffer are included
 * in the draw call.
 */
GLuint VAO, VBO;
GLfloat* vertices = NULL;
int maxVertices = 1000; // constant size for now
int valuesPerVertex = 7; // 3 coordinates + 4 color channels
int currentVertexCount = 0;

/*
 * The default shader program transforms the position
 * and passes through the color.
 *
 * By default the transform is just the identity matrix.
 */
const char* defaultVertexShaderSrc = "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec4 color;\n"
    "uniform mat4 transform;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    gl_Position = transform * vec4(position, 1.0);\n"
    "    fragColor = color;\n"
    "}";

const char* defaultFragmentShaderSrc = "#version 330 core\n"
    "in vec4 fragColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = fragColor;\n"
    "}";
GLuint defaultShaderProgram;
GLint transformLoc;

typedef struct {
    float m[16]; 
} RenderTransform;
RenderTransform transform = {{0}};

static inline int Mat4PosToIndex(int x, int y) {
    return x + y * 4;
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

static void AssertNoGlError() {
    GLenum err = glGetError();
    Assert(err == GL_NO_ERROR, "OpenGL error. glGetError returned code %x", err);
}

void SetResolutionGl(int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
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

    transformLoc = openGlExt.glGetUniformLocation(defaultShaderProgram, "transform");
    transform = Mat4ToRenderTransform(Mat4Identity());
    openGlExt.glUniformMatrix4fv(transformLoc, 1, false, transform.m);

    // -- Vertex buffer for triangles --

    openGlExt.glGenVertexArrays(1, &VAO);
    openGlExt.glGenBuffers(1, &VBO);
    openGlExt.glBindVertexArray(VAO);
    openGlExt.glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int maxVertexBufferSize = maxVertices * valuesPerVertex * sizeof(GLfloat);
    vertices = (GLfloat*)malloc(maxVertexBufferSize);
    Assert(vertices != NULL, "Failed to allocate vertex buffer");

    openGlExt.glBufferData(GL_ARRAY_BUFFER, maxVertexBufferSize, vertices, GL_DYNAMIC_DRAW);

    // position attribute
    openGlExt.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, valuesPerVertex * sizeof(GLfloat), (GLvoid*)0);
    openGlExt.glEnableVertexAttribArray(0);
    // color attribute
    openGlExt.glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, valuesPerVertex * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    openGlExt.glEnableVertexAttribArray(1);

    AssertNoGlError();
}

void EndDrawGl() {
    openGlExt.glBufferSubData(GL_ARRAY_BUFFER, 0, currentVertexCount * valuesPerVertex * sizeof(GLfloat), vertices);

    glDrawArrays(GL_TRIANGLES, 0, currentVertexCount);

    AssertNoGlError();
    currentVertexCount = 0;
}

void ClearScreenGl(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
 }

static inline float ScreenToClipX(float screenX) {
    return 2 * (screenX / screenWidth) - 1;
}

static inline float ScreenToClipY(float screenY) {
    return 1 - 2 * (screenY / screenHeight);
}

void DrawTriangleGl(Vec2 a, Vec2 b, Vec2 c, Color color) {
    Assert(currentVertexCount + 3 <= maxVertices, "Too many vertices. Fix this by increasing the max vertices or by adding automatic resizing.");

    GLfloat verticesToAdd[] = {
        ScreenToClipX(a.x), ScreenToClipY(a.y), 0.0f, 
            color.r, color.g, color.b, color.a,

        ScreenToClipX(b.x), ScreenToClipY(b.y), 0.0f, 
            color.r, color.g, color.b, color.a,

        ScreenToClipX(c.x), ScreenToClipY(c.y), 0.0f, 
            color.r, color.g, color.b, color.a
    };

    memcpy(&vertices[currentVertexCount * valuesPerVertex], verticesToAdd, sizeof(verticesToAdd));

    currentVertexCount += 3;
}
