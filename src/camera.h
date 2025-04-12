/*
 * This header defines camera related utilities that are platform independent.
 * Each renderer is responsible for passing the transform into shaders.
 */

#ifndef camera_h
#define camera_h

#include "common.h"

void SetCameraClientArea(int clientWidth, int clientHeight);
void SetCameraTransform2D(Camera2D* camera);
// get the current transform, which may be for either a 2D or 3D camera depending on previously set settings
Mat4 GetCameraTransform();

#endif
