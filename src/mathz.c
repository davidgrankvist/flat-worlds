#include <math.h>
#include "mathz.h"

static Vec4 Vec2ToVec4(Vec2 vec);
static Vec3 Vec2ToVec3(Vec2 vec);
static Vec4 Vec3ToVec4(Vec3 vec);
static Vec2 Vec4ToVec2(Vec4 vec);
static Vec3 Vec4ToVec3(Vec4 vec);

Mat4 Mat4Identity() {
    Mat4 transform =
    {{
         { 1.0f, 0, 0, 0 },
         { 0, 1.0f, 0, 0 },
         { 0, 0, 1.0f, 0 },
         { 0, 0, 0, 1.0f }
     }};
    return transform;
}

Mat4 Mat4Multiply(Mat4 first, Mat4 second) {
    Mat4 result = {{{0}}};

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            for (int i = 0; i < 4; i++) {
                result.m[y][x] += first.m[y][i] * second.m[i][x];
            }
        }
    }

    return result;
}

Mat4 Mat4MultiplyAll(Mat4 mats[], int size) {
    Mat4 result = mats[0];

    for (int i = 1; i < size; i++) {
        result = Mat4Multiply(result, mats[i]);
    }

    return result;
}

Mat4 Mat4MultiplyAllRev(Mat4 mats[], int size) {
    Mat4 result = mats[size - 1];

    for (int i = size - 2; i >= 0; i--) {
        result = Mat4Multiply(result, mats[i]);
    }

    return result;
}

static Mat4 Mat4AddInternal(Mat4 first, Mat4 second, int sign) {
    Mat4 result = {{{0}}};

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            result.m[y][x] = first.m[y][x] + sign * second.m[y][x];
        }
    }

    return result;
}

Mat4 Mat4Add(Mat4 first, Mat4 second) {
    return Mat4AddInternal(first, second, 1);
}

Mat4 Mat4Subtract(Mat4 first, Mat4 second) {
    return Mat4AddInternal(first, second, -1);
}

Mat4 Mat4RotateX(float angle) {
    Mat4 transform =
    {{
         { 1.0f, 0, 0, 0 },
         { 0, cosf(angle), -sinf(angle), 0 },
         { 0, sinf(angle), cosf(angle), 0 },
         { 0, 0, 0, 1.0f }
     }};
    return transform;
}

Mat4 Mat4RotateY(float angle) {
    Mat4 transform =
    {{
         { cosf(angle), 0, -sinf(angle), 0 },
         { 0, 1.0f, 0, 0 },
         { sinf(angle), 0, cosf(angle), 0 },
         { 0, 0, 0, 1.0f }
     }};
    return transform;
}

Mat4 Mat4RotateZ(float angle) {
    Mat4 transform =
    {{
         { cosf(angle), -sinf(angle), 0, 0 },
         { sinf(angle), cosf(angle), 0, 0 },
         { 0, 0, 1.0f, 0 },
         { 0, 0, 0, 1.0f }
     }};
    return transform;
}

Mat4 Mat4Translate(Vec3 offs) {
    Mat4 transform =
    {{
         { 1.0f, 0, 0, offs.x },
         { 0, 1.0f, 0, offs.y },
         { 0, 0, 1.0f, offs.z },
         { 0, 0, 0, 1.0f }
     }};
    return transform;
}

Mat4 Mat4Translate2(Vec2 offs) {
    return Mat4Translate(Vec2ToVec3(offs));
}

static Vec4 Vec2ToVec4(Vec2 vec) {
    Vec4 vec4 = { vec.x, vec.y, 0, 1 };
    return vec4;
}

static Vec3 Vec2ToVec3(Vec2 vec) {
    Vec3 vec3 = { vec.x, vec.y, 0, };
    return vec3;
}

static Vec2 Vec4ToVec2(Vec4 vec) {
    Vec2 vec2 = { vec.x, vec.y, };
    return vec2;
}

static Vec4 Vec3ToVec4(Vec3 vec) {
    Vec4 vec4 = { vec.x, vec.y, vec.z, 1 };
    return vec4;
}

static Vec3 Vec4ToVec3(Vec4 vec) {
    Vec3 vec3 = { vec.x, vec.y, vec.z };
    return vec3;
    
}

Vec2 Vec2Scale(Vec2 vec, float scale) {
    Vec2 vec2 = { scale * vec.x, scale * vec.y, };
    return vec2;
}

Vec3 Vec3Scale(Vec3 vec, float scale) {
    Vec3 vec3 = { scale * vec.x, scale * vec.y, scale * vec.z };
    return vec3;
}

Vec4 Vec4Scale(Vec4 vec, float scale) {
    Vec4 result = { vec.x * scale, vec.y * scale, vec.z * scale, 1 };
    return result;
}

Vec2 Vec2Transform(Vec2 vec, Mat4 transform) {
    Vec4 vec4 = Vec2ToVec4(vec);
    Vec4 result4 = Vec4Transform(vec4, transform);
    Vec2 result = Vec4ToVec2(result4);

    return result;
}

Vec3 Vec3Transform(Vec3 vec, Mat4 transform) {
    Vec4 vec4 = Vec3ToVec4(vec);
    Vec4 result4 = Vec4Transform(vec4, transform);
    Vec3 result = Vec4ToVec3(result4);

    return result;
}

Vec4 Vec4Transform(Vec4 vec, Mat4 transform) {
    float vecArr[4] = { vec.x, vec.y, vec.z, vec.w };
    float resultArr[4] = {0};

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            resultArr[y] += transform.m[y][x] * vecArr[x];
        }
    }

    Vec4 result = { resultArr[0], resultArr[1], resultArr[2], resultArr[3] };
    return result;
}

Mat4 Mat4RotateAbout2(Vec2 center, float angle) {
    Vec2 centerInverse = Vec2Scale(center, -1);
    Mat4 translate = Mat4Translate2(centerInverse);
    Mat4 rotate = Mat4RotateZ(angle);
    Mat4 translateBack = Mat4Translate2(center);

    Mat4 transforms[] = { translate, rotate, translateBack };
    Mat4 transform = Mat4MultiplyAllRev(transforms, 3);
    return transform;
}

Vec2 Vec2RotateAbout(Vec2 vec, Vec2 center, float angle) {
    Mat4 transform = Mat4RotateAbout2(center, angle);
    Vec2 result = Vec2Transform(vec, transform);
    return result;
}

Mat4 Mat4Ortho(float left, float right,
        float bottom, float top,
        float near, float far) {
    Mat4 transform =
    {{
         { 2 / (right - left), 0, 0, -(right + left) / (right - left) },
         { 0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom) },
         { 0, 0, -2 / (far - near), -(far + near) / (far - near) },
         { 0, 0, 0, 1.0f }
     }};
    return transform;
}
