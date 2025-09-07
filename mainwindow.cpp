#include "mainwindow.h"
#include "globalhotkeymanager.h"

// Qt headers
#include <QApplication>
#include <QClipboard>
#include <QGuiApplication>
#include <QListWidget>
#include <QMenu>
#include <QShortcut>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QThread>
#include <QCursor>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Clipboard History");
    setFixedSize(400, 500);

    listWidget = new QListWidget(this);
    setCentralWidget(listWidget);

    // --- NEW: Apply styling for padding between items ---
    listWidget->setStyleSheet(
        "QListWidget::item {"
        "  padding: 6px 8px;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #3377dd;" // A nice blue for selection
        "  color: white;"
        "}"
        );

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemActivated);

    clipboard = QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);

    // Shortcuts for when the window is active
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

    // Setup and start the global hotkey manager in a separate thread
    QThread* hotkeyThread = new QThread();
    GlobalHotkeyManager* hotkeyManager = new GlobalHotkeyManager();
    hotkeyManager->moveToThread(hotkeyThread);

    connect(hotkeyThread, &QThread::started, hotkeyManager, &GlobalHotkeyManager::run);
    connect(hotkeyManager, &GlobalHotkeyManager::hotkeyPressed, this, &MainWindow::toggleVisibility);
    connect(qApp, &QCoreApplication::aboutToQuit, hotkeyManager, &GlobalHotkeyManager::stop);
    connect(hotkeyManager, &GlobalHotkeyManager::finished, hotkeyThread, &QThread::quit);
    connect(hotkeyThread, &QThread::finished, hotkeyThread, &QThread::deleteLater);
    connect(hotkeyThread, &QThread::finished, hotkeyManager, &GlobalHotkeyManager::deleteLater);

    hotkeyThread->start();
}

MainWindow::~MainWindow()
{
    // The hotkey manager and its thread will be cleaned up automatically
    // via the aboutToQuit signal connections.
}

// ---------------------
// Slots
// ---------------------
void MainWindow::onItemActivated(QListWidgetItem *item)
{
    if (item) {
        // --- UPDATED: Get the original full text from history ---
        int rowIndex = listWidget->row(item);
        if (rowIndex >= 0 && rowIndex < history.size()) {
            clipboard->setText(history.at(rowIndex));
            hide(); // Hide after selection
        }
    }
}

void MainWindow::onClipboardChanged()
{
    QString newText = clipboard->text();
    if (!newText.isEmpty() && (history.empty() || history.front() != newText))
    {
        history.push_front(newText);
        if (history.size() > MAX_HISTORY_SIZE) {
            history.pop_back();
        }

        listWidget->clear();
        for (const QString &text : history) {
            // --- UPDATED: Only add the first line to the list view ---
            QString firstLine = text.split('\n').first().trimmed();
            listWidget->addItem(firstLine);
        }
    }
}

// ---------------------
// Tray Icon
// ---------------------
void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon::fromTheme("edit-copy"));
    trayIcon->setToolTip("LinClip Clipboard Manager");

    QMenu *menu = new QMenu(this);
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

// ---------------------
// Helpers
// ---------------------
void MainWindow::toggleVisibility()
{
    if (isVisible()) {
        hide();
    } else {
        // Refresh list content before showing
        listWidget->clear();
        for (const QString &text : history) {
            QString firstLine = text.split('\n').first().trimmed();
            listWidget->addItem(firstLine);
        }

        // Show at mouse position
        move(QCursor::pos());
        activateWindow();
        raise();
        show();
    }
}

void MainWindow::clearHistory()
{
    history.clear();
    listWidget->clear();
    statusBar()->showMessage("History cleared.", 2000);
}
