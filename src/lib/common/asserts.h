#ifndef asserts_h
#define asserts_h

#include <stdbool.h>

/*
 * Usually use the macros instead of calling this function directly.
 *
 * If you need more control over the file/line capture, you can use
 * this to make a custom macro. This is useful if your assert requires
 * several steps and you want to refer to the caller line number.
 */
void AssertFn(bool b, const char* file, int line, const char* format, ...);

#define Assert(b, format, ...) \
    AssertFn((b), __FILE__, __LINE__, format, __VA_ARGS__)

#define AssertFail(format, ...) \
    AssertFn(false, __FILE__, __LINE__, format, __VA_ARGS__)

#endif
