/*
 * This header defines timing related utilites that are platform independent.
 * Each platform provides its own primitives like tick counting and sleep functions.
 */

#ifndef timing_h
#define timing_h

#include <stdint.h>

typedef struct {
    int64_t (*GetMicroTicks)();
    void (*MicroSleep)(int us);
} PlatformTiming;

void InitPlatformTiming(PlatformTiming timing);

void SetTargetFps(int fps);
int GetFps();
void SleepUntilNextFrame();
void ResetTimer();

#endif
