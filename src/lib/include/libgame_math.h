/*
 * Public header - math.
 */

#ifndef libgame_math_h
#define libgame_math_h

#include "libgame.h"

#define PI 3.14159265358979323846
#define DEGREES_TO_RADIANS (PI/180.0f)
#define RADIANS_TO_DEGREES (180.0f/PI)

// All angles are in radians and counterclockwise.

LIBGAME_EXPORT Mat4 Mat4Identity();
LIBGAME_EXPORT Mat4 Mat4Multiply(Mat4 first, Mat4 second);
LIBGAME_EXPORT Mat4 Mat4MultiplyAll(Mat4 ms[], int size); // multiply in given order
LIBGAME_EXPORT Mat4 Mat4MultiplyAllRev(Mat4 ms[], int size); // multiply in reverse order
LIBGAME_EXPORT Mat4 Mat4Add(Mat4 first, Mat4 second);
LIBGAME_EXPORT Mat4 Mat4Subtract(Mat4 first, Mat4 second);
LIBGAME_EXPORT Mat4 Mat4RotateX(float angle);
LIBGAME_EXPORT Mat4 Mat4RotateY(float angle);
LIBGAME_EXPORT Mat4 Mat4RotateZ(float angle);
LIBGAME_EXPORT Mat4 Mat4Translate(Vec3 offs);
LIBGAME_EXPORT Mat4 Mat4Translate2(Vec2 offs);
LIBGAME_EXPORT Mat4 Mat4Ortho(float left, float right,
        float bottom, float top,
        float near, float far);
LIBGAME_EXPORT Mat4 Mat4ViewTransform(Vec3 target, Vec3 position, Vec3 localUp);
LIBGAME_EXPORT Mat4 Mat4Perspective(float fieldOfViewY, float aspectRatio, float nearPlane, float farPlane);

LIBGAME_EXPORT Vec2 Vec2Scale(Vec2 vec, float scale);
LIBGAME_EXPORT Vec3 Vec3Scale(Vec3 vec, float scale);
LIBGAME_EXPORT Vec4 Vec4Scale(Vec4 vec, float scale);
LIBGAME_EXPORT Vec2 Vec2Transform(Vec2 vec, Mat4 transform);
LIBGAME_EXPORT Vec3 Vec3Transform(Vec3 vec, Mat4 transform);
LIBGAME_EXPORT Vec4 Vec4Transform(Vec4 vec, Mat4 transform);

LIBGAME_EXPORT float Vec3Magnitude(Vec3 vec);
LIBGAME_EXPORT Vec3 Vec3Normalize(Vec3 vec);
LIBGAME_EXPORT Vec3 Vec3Add(Vec3 a, Vec3 b);
LIBGAME_EXPORT Vec3 Vec3Sub(Vec3 a, Vec3 b);
LIBGAME_EXPORT Vec3 Vec3Cross(Vec3 a, Vec3 b);
LIBGAME_EXPORT float Vec3Dot(Vec3 a, Vec3 b);

LIBGAME_EXPORT Mat4 Mat4RotateAbout2(Vec2 center, float angle);
LIBGAME_EXPORT Mat4 Mat4RotateAbout(Vec3 center, float angle);
LIBGAME_EXPORT Vec2 Vec2RotateAbout(Vec2 vec, Vec2 center, float angle);
LIBGAME_EXPORT Vec3 Vec3RotateAbout(Vec3 vec, Vec3 center, float angle);

#endif
