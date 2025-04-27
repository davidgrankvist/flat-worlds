/*
 * Public header - platform specific definitions.
 *
 * This is to make porting easier by having
 * platform specifics outside the libgame.h header.
 */

#ifndef libgame_platform_h
#define libgame_platform_h

#ifdef _WIN32
    // setup exports
    #define LIBGAME_EXPORT __declspec(dllexport)

    // setup main
    LIBGAME_EXPORT void InitMainWin32();
#else
    #error "Unsupported platform."
#endif

/*
 * Add a platform specific main. This is skipped when the library is built,
 * but will be run when the game is built.
 */
#ifndef LIBGAME_SKIP_MAIN
#ifdef _WIN32
    #include <windows.h>

    /*
     * Use wWinMain as the actual main, but make it look like
     * the game code uses the regular main.
     */

    int main(int argc, char** argv);

    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            PWSTR pCmdLine, int nCmdShow) {
        InitMainWin32();
        return main(__argc, __argv);
    }
#else
    #error "No main method has been defined for this platform."
#endif
#endif


#endif
