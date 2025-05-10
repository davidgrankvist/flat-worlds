/*
 * Internal utilities for timing.
 *
 * The platform specific code is responsible for setting up
 * a PlatformTiming before public timing functions are called.
 */

#ifndef timing_h
#define timing_h

#include <stdint.h>

typedef struct {
    int64_t (*GetMicroTicks)();
    void (*MicroSleep)(int us);
} PlatformTiming;

void InitPlatformTiming(PlatformTiming timing);

#endif
