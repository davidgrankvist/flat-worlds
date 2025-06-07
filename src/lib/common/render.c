/*
 * Public API render functions.
 *
 * Delegates to the configured platform render functions.
 */

#include "platform_setup.h"

PlatformRender render = {};

void InitPlatformRender(PlatformRender pr) {
    render = pr; 
}

void ConfigureRender(RenderSettings settings) {
   render.Configure(settings);
}

void ClearScreen(Color color) {
   render.ClearScreen(color); 
}

void MakeDrawCall() {
   render.MakeDrawCall(); 
}

void EndFrame() {
   render.EndFrame(); 
}

void SetTransform(Mat4 mat) {
   render.SetTransform(mat); 
}

void SetCamera2D(Camera2D* camera) {
   render.SetCamera2D(camera); 
}

void SetCamera3D(Camera3D* camera) {
   render.SetCamera3D(camera); 
}

void DrawTriangle2D(Vec2 a, Vec2 b, Vec2 c, Color color) {
    render.DrawTriangle2D(a, b, c, color);    
}

void DrawTriangle3D(Vec3 a, Vec3 b, Vec3 c, Color color) {
    render.DrawTriangle3D(a, b, c, color); 
}

void DrawQuad3D(Vec3 topLeft, Vec3 topRight, Vec3 bottomLeft, Vec3 bottomRight, Color color) {
   render.DrawQuad3D(topLeft, topRight, bottomLeft, bottomRight, color); 
}
