/*
 * Platform setup - library loading.
 *
 * Set up platform specific library loading before calling
 * public library loading utilities.
 */

#ifndef platform_library_load_h
#define platform_library_load_h

#include "libgame.h"

typedef struct {
    void (*ResolvePath)(char* fileBaseName, FileExtensionType extension, char* out, int outSize);
    bool (*LoadDynamicLibrary)(char* libraryPath, DynamicLibrary* lib);
    void* (*LoadLibraryFunction)(char* functionName, DynamicLibrary* lib);
} PlatformLibraryLoader;

void SetPlatformLibraryLoader(PlatformLibraryLoader libraryLoader);

#endif
