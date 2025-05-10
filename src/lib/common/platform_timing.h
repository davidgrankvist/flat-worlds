/*
 * Platform setup - timing
 *
 * Set up the platform specific timing utilities
 * before the public timing functions are called.
 */

#ifndef platform_timing_h
#define platform_timing_h

#include <stdint.h>

typedef struct {
    int64_t (*GetMicroTicks)();
    void (*MicroSleep)(int us);
} PlatformTiming;

void InitPlatformTiming(PlatformTiming timing);

#endif
