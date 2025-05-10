/*
 * This is the OpenGL rendering backend.
 *
 * The basic idea is to provide utilities like DrawTriangle and then
 * internally keep track of buffering and batching draw calls.
 */

#ifndef opengl_render_h
#define opengl_render_h

#include "platform_opengl_render.h"
#include "libgame.h"

// internal utilities for platform code
void EndDrawGl(); // call before swapping buffers
void SetResolutionGl(int width, int height); // call at window resize

// render API
void SetCamera2DGl(Camera2D* camera);
void SetCamera3DGl(Camera3D* camera);
void ClearScreenGl(Color color);
void DrawTriangle2DGl(Vec2 a, Vec2 b, Vec2 c, Color color);
void DrawTriangle3DGl(Vec3 a, Vec3 b, Vec3 c, Color color);
void SetTransformGl(Mat4 mat);
void EndFrameGl();

#endif
