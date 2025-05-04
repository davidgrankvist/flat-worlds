/*
 * Public header - platform specific main.
 *
 * Should only be included from the game entrypoint.
 */
#ifndef libgame_platform_main_h
#define libgame_platform_main_h

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
