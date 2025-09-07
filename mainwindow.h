#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QClipboard>
#include <QSystemTrayIcon>
#include <QStringList>
#include <QListWidgetItem>
#include <QShortcut>

// Forward declaration to avoid including the header here
class GlobalHotkeyManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // --- ADDED: This slot will be triggered by the hotkey signal ---
    void toggleVisibility();

private slots:
    void onItemActivated(QListWidgetItem *item);
    void onClipboardChanged();
    void clearHistory();

private:
    void createTrayIcon();

    QListWidget *listWidget;
    QClipboard *clipboard;
    QSystemTrayIcon *trayIcon;

    // Use a deque or list for efficient front insertion
    QList<QString> history;
    const int MAX_HISTORY_SIZE = 20;
};
