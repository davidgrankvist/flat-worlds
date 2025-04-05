#include <stdarg.h>
#include "common.h"

// -- Utils --

void Assert(bool b, const char* format, ...) {
    if (!b) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\nat %s:%d\n", __FILE__, __LINE__);

        exit(1);
    }
}

void AssertFail(const char* format, ...) {
    va_list args;
    va_start(args, format);
    Assert(false, format, args);
    va_end(args);
}
