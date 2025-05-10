/*
 * This header defines camera related utilities that are platform independent.
 * Each renderer is responsible for passing the transform into shaders.
 */

#ifndef camera_h
#define camera_h

#include "libgame.h"

// -- Public API --

void SetCameraTransform2D(Camera2D* camera);
void SetCameraTransform3D(Camera3D* camera);
Camera3D GetDefaultCamera3D();
void RotateCameraFirstPerson(Camera3D* camera, float yaw, float pitch, float roll);
void MoveCameraFirstPerson(Camera3D* camera, Vec3 relativeOffset);

// -- Internal helpers --

// get the current transform, which may be for either a 2D or 3D camera depending on previously set settings
Mat4 GetCameraTransform();
void SetCameraClientArea(int clientWidth, int clientHeight);

#endif
