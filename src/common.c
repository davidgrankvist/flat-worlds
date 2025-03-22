#include <stdarg.h>
#include "common.h"

void Assert(bool b, const char* format, ...) {
    if (!b) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fputs("\n", stderr);

        exit(1);
    }
}
