#include <stdarg.h>
#include <stdio.h>
#include "asserts.h"

void AssertFn(bool b, const char* file, int line, const char* format, ...) {
    if (!b) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\nat %s:%d\n", file, line);

        exit(1);
    }
}
