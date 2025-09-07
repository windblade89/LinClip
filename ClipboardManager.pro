QT += core gui widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    globalhotkeymanager.cpp  # Add this line

HEADERS += \
    hotkeyprivate.h \
    mainwindow.h \
    globalhotkeymanager.h    # Add this line

# Link X11 libraries
LIBS += -lX11 -lxcb
