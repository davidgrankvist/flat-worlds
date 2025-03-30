#ifndef mathz_h
#define mathz_h

#include "common.h"

// All angles are in radians and counterclockwise.

Mat4 Mat4Identity();
Mat4 Mat4Multiply(Mat4 first, Mat4 second);
Mat4 Mat4MultiplyAll(Mat4 ms[], int size);
Mat4 Mat4MultiplyAllRev(Mat4 ms[], int size);
Mat4 Mat4Add(Mat4 first, Mat4 second);
Mat4 Mat4Subtract(Mat4 first, Mat4 second);
Mat4 Mat4RotateX(float angle);
Mat4 Mat4RotateY(float angle);
Mat4 Mat4RotateZ(float angle);
Mat4 Mat4Translate(Vec3 offs);
Mat4 Mat4Translate2(Vec2 offs);
Mat4 Mat4Ortho(float left, float right,
        float bottom, float top,
        float near, float far);

Vec2 Vec2Scale(Vec2 vec, float scale);
Vec3 Vec3Scale(Vec3 vec, float scale);
Vec4 Vec4Scale(Vec4 vec, float scale);
Vec2 Vec2Transform(Vec2 vec, Mat4 transform);
Vec3 Vec3Transform(Vec3 vec, Mat4 transform);
Vec4 Vec4Transform(Vec4 vec, Mat4 transform);

Mat4 Mat4RotateAbout2(Vec2 center, float angle);
Vec2 Vec2RotateAbout(Vec2 vec, Vec2 center, float angle);

#endif
