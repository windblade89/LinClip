#pragma once

#include <X11/Xlib.h>

// This class holds all X11-specific details.
// It is hidden from the rest of the project by only being included in the .cpp.
class HotkeyPrivate {
public:
    ~HotkeyPrivate();

    bool registerHotkey();
    void unregisterHotkey();

    Display *display = nullptr;
    Window rootWindow = 0;
    int keycode = 0;
};
