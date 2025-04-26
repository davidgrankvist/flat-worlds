#ifndef asserts_h
#define asserts_h

#include <stdbool.h>

void Assert(bool b, const char* format, ...);
void AssertFail(const char* format, ...);

#endif
