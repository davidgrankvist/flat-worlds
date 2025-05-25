/*
 * Platform setup - render
 *
 * Initialize platform specifics before render utilities are called.
 */
#ifndef platform_render_h
#define platform_render_h

#include "libgame.h"

typedef struct {
    void (*Configure)(RenderSettings settings);
    void (*ClearScreen)(Color color);
    void (*MakeDrawCall)();
    void (*EndFrame)();
    void (*SetTransform)(Mat4 mat);
    void (*SetCamera2D)(Camera2D* camera);
    void (*SetCamera3D)(Camera3D* camera);
    void (*DrawTriangle2D)(Vec2 a, Vec2 b, Vec2 c, Color color);
    void (*DrawTriangle3D)(Vec3 a, Vec3 b, Vec3 c, Color color);
    void (*DrawQuad3D)(Vec3 topLeft, Vec3 topRight, Vec3 bottomLeft, Vec3 bottomRight, Color color);
} PlatformRender;

void InitPlatformRender(PlatformRender platformRender);

#endif
