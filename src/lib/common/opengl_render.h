#ifndef opengl_render_h
#define opengl_render_h

#include "libgame.h"

// call before swapping buffers
void EndDrawGl();
// call at window resize
void SetResolutionGl(int width, int height);

// use these to set up PlatformRender
void ConfigureRenderGl(RenderSettings settings);
void ClearScreenGl(Color color);
void SetTransformGl(Mat4 mat);
void EndFrameGl();
void SetCamera2DGl(Camera2D* camera);
void SetCamera3DGl(Camera3D* camera);
void DrawTriangle2DGl(Vec2 a, Vec2 b, Vec2 c, Color color);
void DrawTriangle3DGl(Vec3 a, Vec3 b, Vec3 c, Color color);
void DrawQuad3DGl(Vec3 topLeft, Vec3 topRight, Vec3 bottomLeft, Vec3 bottomRight, Color color);

#endif
