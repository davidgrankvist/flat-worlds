#include "camera.h"
#include "libgame_math.h"

static int clientWidth = 0;
static int clientHeight = 0;
static Mat4 transform = {};
static Mat4 fallBackTransform = {};
static bool didSetCamera = false;
static float dummyAspectRatio = 0;

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

void SetCameraTransform3D(Camera3D* camera) {
    Mat4 view = Mat4ViewTransform(camera->target, camera->position, camera->up);

    float aspectRatio = camera->aspectRatio > dummyAspectRatio ? camera->aspectRatio : clientWidth / (float) clientHeight;
    Mat4 perspective = Mat4Perspective(camera->fieldOfViewY, aspectRatio, camera->nearPlane, camera->farPlane);

    Mat4 cameraTransform = Mat4Multiply(perspective, view);
    transform = cameraTransform;

    didSetCamera = true;
}

Mat4 GetCameraTransform() {
    if (didSetCamera) {
        return transform;
    }
    return fallBackTransform;
}

Camera3D GetDefaultCamera3D() {
    Camera3D camera = {0};

    camera.position = (Vec3) { 0, 0, -250 };
    camera.target = (Vec3) { 0, 0, 0 };
    camera.up = (Vec3) { 0, 1, 0};

    camera.fieldOfViewY = PI / 4;
    camera.nearPlane = 0.01;
    camera.farPlane = 1000;
    camera.aspectRatio = dummyAspectRatio;

    return camera;
}
