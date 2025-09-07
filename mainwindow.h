#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QClipboard>
#include <QSystemTrayIcon>
#include <QShortcut>
#include <QList>
#include <QThread>

class GlobalHotkeyManager; // Forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onItemActivated(QListWidgetItem *item);
    void onClipboardChanged();
    void onHotkeyActivated(); // The slot that listens for the hotkey
    void clearHistory();

private:
    void createTrayIcon();

    // UI elements
    QListWidget *listWidget;
    QSystemTrayIcon *trayIcon;

    // Data
    QClipboard *clipboard;
    QList<QString> history;
    static const int MAX_HISTORY_SIZE = 20;

    // Threading for the hotkey manager
    QThread hotkeyThread;
    GlobalHotkeyManager *hotkeyManager;
};
