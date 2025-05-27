#include <stdbool.h>
#include "libgame.h"
#include "platform_window.h"

static PlatformWindow platformWindow = {};
static bool shouldRun = true;
static int clientWidth = 0;
static int clientHeight = 0;

void InitPlatformWindow(PlatformWindow pw) {
    platformWindow = pw;
}

void SetResolution(int cw, int ch) {
    clientWidth = cw;
    clientHeight = ch;
}

void InitWindow(const char* title) {
    platformWindow.InitWindow(title);
}

void InitConsole() {
   platformWindow.InitConsole();
}

bool IsWindowOpen() {
   return shouldRun;
}

void CloseCurrentWindow() {
   shouldRun = false;
}

int GetClientWidth() {
    return clientWidth;
}

int GetClientHeight() {
   return clientHeight;
}
