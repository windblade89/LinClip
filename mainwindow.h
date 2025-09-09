#pragma once

#include <QMainWindow>
#include <QList>
#include <QVariant> // ADDED: For storing text or images

// Forward declarations to keep header clean
class QListWidget;
class QListWidgetItem;
class QClipboard;
class QSystemTrayIcon;
class QThread;
class GlobalHotkeyManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onItemActivated(QListWidgetItem *item);
    void onClipboardChanged();
    void toggleVisibility();
    void clearHistory();

private:
    void createTrayIcon();
    void updateListWidget(); // ADDED: Helper to refresh the UI

    QListWidget *listWidget;
    QClipboard *clipboard;
    QSystemTrayIcon *trayIcon;

    // --- MODIFIED: History now stores QVariant (text or image) ---
    QList<QVariant> history;

    // Hotkey manager members (no changes here)
    QThread* hotkeyThread;
    GlobalHotkeyManager* hotkeyManager;
};
