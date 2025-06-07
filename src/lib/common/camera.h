#ifndef camera_h
#define camera_h

#include "libgame.h"

// call from render backend SetResolution
void SetCameraClientArea(int clientWidth, int clientHeight);

// call these in the render backend to set/get the camera transform
void SetCameraTransform2D(Camera2D* camera);
void SetCameraTransform3D(Camera3D* camera);
Mat4 GetCameraTransform();

#endif
