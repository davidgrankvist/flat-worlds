#include <stdint.h>
#include "libgame.h"
#include "platform_input.h"
#include "asserts.h"

// helpers to toggle 1-bit key/button states
#define KEY_TO_BIT(k) (1ULL << k)
#define IS_KEY_SET(k, input) (input & KEY_TO_BIT(k))

// current / previous key states are set with single bits (1 means down)
typedef struct {
    // keys
    uint64_t inputKeys[2];

    // mouse
    int mouseX;
    int mouseY;
    int mousePrevX;
    int mousePrevY;
    uint8_t inputMouseButtons[2];
} InputState;
InputState inputState = {};

void UpdateInputBuffers() {
    Assert(KeyUnknown <= 64, "Too many key codes to fit in a u64. Please update the input data structure.");
    Assert(MouseUnknown <= 8, "Too many mouse key codes to fit in a u8. Please update the input data structure.");

    inputState.inputKeys[1] = inputState.inputKeys[0];
    inputState.inputMouseButtons[1] = inputState.inputMouseButtons[0];
    inputState.mousePrevX = inputState.mouseX;
    inputState.mousePrevY = inputState.mouseY;
}

void SetKeyDown(InputKey key) {
    inputState.inputKeys[0] |= KEY_TO_BIT(key);
}

void SetKeyUp(InputKey key) {
    inputState.inputKeys[0] &= ~KEY_TO_BIT(key);
}

bool IsKeyDown(InputKey key) {
    return IS_KEY_SET(key, inputState.inputKeys[0]);
}

bool IsKeyPressed(InputKey key) {
    return IS_KEY_SET(key, inputState.inputKeys[0])
        && !IS_KEY_SET(key, inputState.inputKeys[1]);
}

bool IsKeyReleased(InputKey key) {
    return !IS_KEY_SET(key, inputState.inputKeys[0])
        && IS_KEY_SET(key, inputState.inputKeys[1]);
}

void SetMouseDown(InputMouseButton btn) {
    inputState.inputMouseButtons[0] |= KEY_TO_BIT(btn);
}

void SetMouseUp(InputMouseButton btn) {
    inputState.inputMouseButtons[0] &= ~KEY_TO_BIT(btn);
}

bool IsMouseDown(InputMouseButton btn) {
    return IS_KEY_SET(btn, inputState.inputMouseButtons[0]);
}

bool IsMousePressed(InputMouseButton btn) {
    return IS_KEY_SET(btn, inputState.inputMouseButtons[0])
        && !IS_KEY_SET(btn, inputState.inputMouseButtons[1]);
}

bool IsMouseReleased(InputMouseButton btn) {
    return !IS_KEY_SET(btn, inputState.inputMouseButtons[0])
        && IS_KEY_SET(btn, inputState.inputMouseButtons[1]);
}

void SetMousePosition(int x, int y) {
    inputState.mouseX = x;
    inputState.mouseY = y;
}

int GetMouseInputX() {
    return inputState.mouseX;
}

int GetMouseInputY() {
    return inputState.mouseY;
}

int GetMouseInputDeltaX() {
    return inputState.mouseX - inputState.mousePrevX;
}

int GetMouseInputDeltaY() {
    return inputState.mouseY - inputState.mousePrevY;
}
