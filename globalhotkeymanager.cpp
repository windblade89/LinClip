// --- 1. Include your Qt-facing headers FIRST ---
#include "globalhotkeymanager.h"
#include "hotkeyprivate.h"
#include <QThread>
#include <iostream>

// --- 2. Then include X11 ---
#include <X11/Xlib.h>
#include <X11/keysym.h>

// --- 3. Clean up all common conflicting X11 macros ---
#undef None
#undef Bool
#undef Status
#undef Success
#undef GrayScale
#undef Above
#undef Below

// --- 4. Implementation of GlobalHotkeyManager ---

GlobalHotkeyManager::GlobalHotkeyManager(QObject *parent)
    : QObject(parent), d(std::make_unique<HotkeyPrivate>())
{
}

GlobalHotkeyManager::~GlobalHotkeyManager() = default;

void GlobalHotkeyManager::run()
{
    if (!d->registerHotkey()) {
        std::cerr << "Hotkey registration failed. Thread will now exit." << std::endl;
        emit finished();
        return;
    }

    // This is a proper non-blocking event loop suitable for a Qt thread
    while (!QThread::currentThread()->isInterruptionRequested()) {
        if (XPending(d->display)) {
            XEvent ev;
            XNextEvent(d->display, &ev);
            if (ev.type == KeyPress) {
                // --- THE FIX ---
                // Use the correct signal name from the latest .h file
                emit hotkeyPressed();
            }
        } else {
            // Sleep briefly to avoid 100% CPU usage
            QThread::msleep(20);
        }
    }

    emit finished();
}

void GlobalHotkeyManager::stop()
{
    // The main window will call requestInterruption() on the thread,
    // which will cause the loop in run() to terminate.
    // This function is here to match the slot in the header.
}


// --- 5. Implementation of HotkeyPrivate ---

HotkeyPrivate::~HotkeyPrivate()
{
    if (display) {
        unregisterHotkey();
        XCloseDisplay(display);
    }
}

bool HotkeyPrivate::registerHotkey()
{
    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Error: Cannot open X display." << std::endl;
        return false;
    }

    rootWindow = DefaultRootWindow(display);
    keycode = XKeysymToKeycode(display, XK_V);
    unsigned int modifiers = ControlMask | Mod1Mask; // Ctrl + Alt

    // Robustly handle NumLock and CapsLock modifiers
    unsigned int numLockMask = 0;
    XModifierKeymap *modmap = XGetModifierMapping(display);
    KeyCode numLockKeyCode = XKeysymToKeycode(display, XK_Num_Lock);
    if (numLockKeyCode != 0) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < modmap->max_keypermod; j++) {
                if (modmap->modifiermap[i * modmap->max_keypermod + j] == numLockKeyCode) {
                    numLockMask = (1 << i);
                }
            }
        }
    }
    XFreeModifiermap(modmap);

    XGrabKey(display, keycode, modifiers, rootWindow, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, keycode, modifiers | LockMask, rootWindow, True, GrabModeAsync, GrabModeAsync);
    if (numLockMask != 0) {
        XGrabKey(display, keycode, modifiers | numLockMask, rootWindow, True, GrabModeAsync, GrabModeAsync);
        XGrabKey(display, keycode, modifiers | LockMask | numLockMask, rootWindow, True, GrabModeAsync, GrabModeAsync);
    }

    XSync(display, False);
    return true;
}

void HotkeyPrivate::unregisterHotkey()
{
    if (display) {
        XUngrabKey(display, keycode, AnyModifier, rootWindow);
    }
}
