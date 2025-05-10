/*
 * Internal camera related utilities to modify the camera state.
 *
 * This is to support the render backend.
 */

#ifndef camera_h
#define camera_h

#include "libgame.h"

void SetCameraClientArea(int clientWidth, int clientHeight);
void SetCameraTransform2D(Camera2D* camera);
void SetCameraTransform3D(Camera3D* camera);
// get the current transform, which may be for either a 2D or 3D camera depending on previously set settings
Mat4 GetCameraTransform();

#endif
