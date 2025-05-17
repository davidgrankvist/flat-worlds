#ifndef asserts_h
#define asserts_h

#include <stdbool.h>

void AssertFn(bool b, const char* file, int line, const char* format, ...);

#define Assert(b, format, ...) \
    AssertFn((b), __FILE__, __LINE__, format, __VA_ARGS__)

#define AssertFail(format, ...) \
    AssertFn(false, __FILE__, __LINE__, format, __VA_ARGS__)

#endif
