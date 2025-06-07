#include "libgame.h"
#include "platform_setup.h"
#include "asserts.h"

PlatformTiming platformTiming = {};
static int64_t targetFps = 60;
static int64_t ticksStart = 0;

void InitPlatformTiming(PlatformTiming pt) {
   platformTiming = pt;
}

uint64_t GetTicks() {
    return platformTiming.GetMicroTicks();
}

void SetTargetFps(int fps) {
    targetFps = fps;
}

int GetFps() {
    AssertFail("GetFps is not implemented yet");
    return 1337;
}

void SleepUntilNextFrame() {
    int64_t ellapsed = platformTiming.GetMicroTicks() - ticksStart;
    int64_t targetUsPerFrame = TICKS_PER_SECOND / targetFps;

    int64_t delta = targetUsPerFrame - ellapsed;

    if (delta > 0) {
        platformTiming.MicroSleep(delta);
    }

    ResetFpsTimer();
}

void ResetFpsTimer() {
    ticksStart = platformTiming.GetMicroTicks();
}
