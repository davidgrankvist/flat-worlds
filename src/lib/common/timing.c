#include "timing.h"
#include "asserts.h"

PlatformTiming timing = {};
static int64_t usPerSecond = 1000000;
static int64_t targetFps = 60;
static int64_t ticksStart = 0;

void InitPlatformTiming(PlatformTiming pt) {
   timing = pt;
}

void SetTargetFps(int fps) {
    targetFps = fps;
}

int GetFps() {
    AssertFail("GetFps is not implemented yet");
    return 1337;
}

void SleepUntilNextFrame() {
    int64_t ellapsed = timing.GetMicroTicks() - ticksStart;
    int64_t targetUsPerFrame = usPerSecond / targetFps;

    int64_t delta = targetUsPerFrame - ellapsed;

    if (delta > 0) {
        timing.MicroSleep(delta);
    }

    ResetTimer();
}

void ResetTimer() {
    ticksStart = timing.GetMicroTicks();
}
