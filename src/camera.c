#include "camera.h"
#include "mathz.h"

static int clientWidth = 0;
static int clientHeight = 0;
static Mat4 transform = {};
static Mat4 fallBackTransform = {};
static bool didSetCamera = false;

void SetCameraClientArea(int width, int height) {
    clientWidth = width;
    clientHeight = height;
    fallBackTransform = Mat4Ortho(0, clientWidth, 0, clientHeight, -1, 1);
}

void SetCameraTransform2D(Camera2D* camera) {
   Vec2 origin = camera->origin; 
   Mat4 ortho = Mat4Ortho(origin.x, clientWidth + origin.x, origin.y, clientHeight + origin.y, -1, 1);
   transform = ortho;
   didSetCamera = true;
}

Mat4 GetCameraTransform() {
    if (didSetCamera) {
        return transform;
    }
    return fallBackTransform;
}
