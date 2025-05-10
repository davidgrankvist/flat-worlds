#include "camera.h"
#include "libgame_math.h"

static int clientWidth = 0;
static int clientHeight = 0;
static Mat4 transform = {};
static Mat4 fallBackTransform = {};
static bool didSetCamera = false;
static float dummyAspectRatio = 0;

// -- Common --

void SetCameraClientArea(int width, int height) {
    clientWidth = width;
    clientHeight = height;
    fallBackTransform = Mat4Ortho(0, clientWidth, 0, clientHeight, -1, 1);
}

Mat4 GetCameraTransform() {
    if (didSetCamera) {
        return transform;
    }
    return fallBackTransform;
}


// -- 2D --

void SetCameraTransform2D(Camera2D* camera) {
   Vec2 origin = camera->origin; 
   Mat4 ortho = Mat4Ortho(origin.x, clientWidth + origin.x, origin.y, clientHeight + origin.y, -1, 1);
   transform = ortho;

   didSetCamera = true;
}

// -- 3D --

void SetCameraTransform3D(Camera3D* camera) {
    Mat4 view = Mat4ViewTransform(camera->target, camera->position, camera->up);

    float aspectRatio = camera->aspectRatio > dummyAspectRatio ? camera->aspectRatio : clientWidth / (float) clientHeight;
    Mat4 perspective = Mat4Perspective(camera->fieldOfViewY, aspectRatio, camera->nearPlane, camera->farPlane);

    Mat4 cameraTransform = Mat4Multiply(perspective, view);
    transform = cameraTransform;

    didSetCamera = true;
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

void RotateCameraFirstPerson(Camera3D* camera, float yaw, float pitch, float roll) {
    // yaw - rotate forward about the initial up
    Vec3 forward = Vec3Sub(camera->target, camera->position);
    Vec3 forwardYaw = Vec3RotateAboutAxis(forward, camera->up, yaw); 

    // pitch - rotate the new forward about the new right
    Vec3 right = Vec3Normalize(Vec3Cross(camera->up, forwardYaw));
    Vec3 forwardPitch = Vec3RotateAboutAxis(forwardYaw, right, pitch);

    // roll - rotate the initial up about the final forward
    Vec3 up = Vec3Normalize(Vec3Cross(forwardPitch, right));
    Vec3 forwardPitchN = Vec3Normalize(forwardPitch);
    Vec3 upRoll = Vec3RotateAboutAxis(up, forwardPitchN, roll);

    Vec3 target = Vec3Add(forwardPitch, camera->position);

    camera->target = target;
    camera->up = upRoll;
}

void MoveCameraFirstPerson(Camera3D* camera, Vec3 relativeOffset) {
    // camera axes
    Vec3 forwardN = Vec3Normalize(Vec3Sub(camera->target, camera->position));
    Vec3 upN = camera->up;
    Vec3 rightN = Vec3Normalize(Vec3Cross(upN, forwardN));

    // offset along the axes
    Vec3 offsetRight = Vec3Scale(rightN, relativeOffset.x);
    Vec3 offsetUp = Vec3Scale(upN, relativeOffset.y);
    Vec3 offsetForward = Vec3Scale(forwardN, relativeOffset.z);
    Vec3 actualOffset = Vec3Add(Vec3Add(offsetRight, offsetUp), offsetForward);

    camera->position = Vec3Add(camera->position, actualOffset);
    camera->target = Vec3Add(camera->target, actualOffset);
}
