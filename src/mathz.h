#ifndef mathz_h
#define mathz_h

#include "common.h"

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

Vec2 Vec2Scale(Vec2 vec, float scale);
Vec3 Vec3Scale(Vec3 vec, float scale);
Vec4 Vec4Scale(Vec4 vec, float scale);
Vec2 Vec2Transform(Vec2 vec, Mat4 transform);
Vec3 Vec3Transform(Vec3 vec, Mat4 transform);
Vec4 Vec4Transform(Vec4 vec, Mat4 transform);

Vec2 Vec2RotateAbout(Vec2 vec, Vec2 center, float angle);

#endif
