#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // By default, the app will keep running even if the window is closed,
    // because the system tray icon will still exist.
    a.setQuitOnLastWindowClosed(false);

    MainWindow w;

    // UPDATED: We no longer show the window on startup.
    // It will be shown only when the global hotkey is pressed.
    // w.show();

    return a.exec();
}
