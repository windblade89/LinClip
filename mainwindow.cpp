#include "mainwindow.h"
#include "globalhotkeymanager.h" // Include the manager

// Standard Qt headers
#include <QGuiApplication>
#include <QMenu>
#include <QApplication>
#include <QCursor>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Clipboard History");
    setFixedSize(400, 500);

    // Set up the main list widget
    listWidget = new QListWidget(this);
    setCentralWidget(listWidget);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemActivated);

    // Monitor the system clipboard for changes
    clipboard = QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);

    // --- In-window Shortcuts (only active when window is focused) ---
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, [this]() {
        if (listWidget->currentItem()) {
            onItemActivated(listWidget->currentItem());
        }
    });

    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteShortcut, &QShortcut::activated, this, &MainWindow::clearHistory);

    QShortcut *quitShortcut = new QShortcut(QKeySequence(tr("Ctrl+Q", "Quit")), this);
    connect(quitShortcut, &QShortcut::activated, qApp, &QCoreApplication::quit);

    createTrayIcon();

    // --- Global Hotkey Setup (runs in a separate thread) ---
    hotkeyManager = new GlobalHotkeyManager();
    hotkeyManager->moveToThread(&hotkeyThread);

    // Connect signals and slots for thread-safe communication
    connect(&hotkeyThread, &QThread::started, hotkeyManager, &GlobalHotkeyManager::run);
    connect(hotkeyManager, &GlobalHotkeyManager::finished, &hotkeyThread, &QThread::quit);
    connect(hotkeyManager, &GlobalHotkeyManager::finished, hotkeyManager, &GlobalHotkeyManager::deleteLater);
    connect(&hotkeyThread, &QThread::finished, &hotkeyThread, &QThread::deleteLater);
    connect(hotkeyManager, &GlobalHotkeyManager::hotkeyActivated, this, &MainWindow::onHotkeyActivated);

    hotkeyThread.start();
}

MainWindow::~MainWindow()
{
    // Cleanly stop the hotkey thread when the main window is destroyed
    hotkeyThread.requestInterruption();
    hotkeyThread.quit();
    hotkeyThread.wait();
}

// This slot is triggered safely from the hotkey manager's thread
void MainWindow::onHotkeyActivated()
{
    if (isVisible()) {
        hide();
    } else {
        // Position the window at the current mouse cursor location
        move(QCursor::pos());
        // Bring the window to the front
        activateWindow();
        raise();
        show();
    }
}

void MainWindow::onItemActivated(QListWidgetItem *item)
{
    if (item) {
        clipboard->setText(item->text());
        hide(); // Hide the window after a selection is made
    }
}

void MainWindow::onClipboardChanged()
{
    QString newText = clipboard->text();
    // Add new, non-empty, and unique text to the history
    if (!newText.isEmpty() && (history.empty() || history.front() != newText))
    {
        history.push_front(newText);
        // Keep the history size limited to the max size
        if (history.size() > MAX_HISTORY_SIZE) {
            history.pop_back();
        }

        // Refresh the list widget with the updated history
        listWidget->clear();
        for (const QString &text : history) {
            listWidget->addItem(text);
        }
    }
}

void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    // Use a standard system icon for "copy"
    trayIcon->setIcon(QIcon::fromTheme("edit-copy"));
    trayIcon->setToolTip("Clipboard Manager");

    QMenu *menu = new QMenu(this);
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

void MainWindow::clearHistory()
{
    history.clear();
    listWidget->clear();
    statusBar()->showMessage("History cleared.", 2000); // Show a confirmation message
}
