#include "libgame.h"
#include "platform_setup.h"

PlatformLibraryLoader libraryLoader = {};

void SetPlatformLibraryLoader(PlatformLibraryLoader pll) {
    libraryLoader = pll;
}

void ResolvePath(char* fileBaseName, FileExtensionType extension, char* out, int outSize) {
    libraryLoader.ResolvePath(fileBaseName, extension, out, outSize);
}

bool LoadDynamicLibrary(char* libraryPath, DynamicLibrary* lib) {
    return libraryLoader.LoadDynamicLibrary(libraryPath, lib);
}

void* LoadLibraryFunction(char* functionName, DynamicLibrary* lib) {
    return libraryLoader.LoadLibraryFunction(functionName, lib);
}
