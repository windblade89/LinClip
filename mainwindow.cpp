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
#include <QVBoxLayout>
#include <QImage>
#include <QVariant>
#include <QMimeData>
#include <QUrl>        // ADDED: To handle file paths

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("LinClip History");
    setFixedSize(400, 500);

    // --- Window Padding Setup (no change) ---
    QWidget *centralContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralContainer);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget);
    setCentralWidget(centralContainer);

    // --- Styling (no change) ---
    listWidget->setStyleSheet(
        "QListWidget::item {"
        "  padding: 6px 8px;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #3377dd;"
        "  color: white;"
        "}"
        );

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemActivated);

    clipboard = QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);

    // --- Shortcuts (no change) ---
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, [this]() {
        if (listWidget->currentItem()) {
            onItemActivated(listWidget->currentItem());
        }
    });
    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteShortcut, &QShortcut::activated, this, &MainWindow::clearHistory);
    QShortcut *quitShortcut = new QShortcut(QKeySequence(tr("Ctrl+Q", "Quit")), this);
    connect(quitShortcut, &QShortcut::activated, qApp, &QApplication::quit);

    createTrayIcon();

    // --- Hotkey Thread Setup (no change) ---
    hotkeyThread = new QThread();
    hotkeyManager = new GlobalHotkeyManager();
    hotkeyManager->moveToThread(hotkeyThread);
    connect(hotkeyThread, &QThread::started, hotkeyManager, &GlobalHotkeyManager::run);
    connect(hotkeyManager, &GlobalHotkeyManager::hotkeyPressed, this, &MainWindow::toggleVisibility);
    connect(qApp, &QApplication::aboutToQuit, hotkeyManager, &GlobalHotkeyManager::stop);
    connect(hotkeyManager, &GlobalHotkeyManager::finished, hotkeyThread, &QThread::quit);
    connect(hotkeyThread, &QThread::finished, hotkeyThread, &QThread::deleteLater);
    connect(hotkeyThread, &QThread::finished, hotkeyManager, &GlobalHotkeyManager::deleteLater);
    hotkeyThread->start();
}

MainWindow::~MainWindow()
{
    // Cleanup is handled by signal connections
}

// ---------------------
// Slots
// ---------------------
void MainWindow::onItemActivated(QListWidgetItem *item)
{
    if (!item) return;

    QVariant data = item->data(Qt::UserRole);

    if (data.canConvert<QImage>()) {
        clipboard->setImage(data.value<QImage>());
    } else if (data.canConvert<QString>()) {
        clipboard->setText(data.value<QString>());
    }
    hide();
}

void MainWindow::onClipboardChanged()
{
    const QMimeData *mimeData = clipboard->mimeData();
    QVariant newContent;

    // --- MODIFIED: More robust content checking ---
    // 1. Prioritize raw image data (e.g., from screenshots, "Copy Image")
    if (mimeData->hasImage()) {
        newContent = clipboard->image();
    }
    // 2. Check for image file paths (e.g., from "Copy" in a file manager)
    else if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty() && urls.first().isLocalFile()) {
            // Try to load the file as an image
            QImage image(urls.first().toLocalFile());
            if (!image.isNull()) {
                newContent = image;
            }
        }
    }

    // 3. If we still haven't found an image, check for plain text.
    // This prevents grabbing the file path as text if the URL was an image.
    if (!newContent.isValid() && mimeData->hasText()) {
        newContent = clipboard->text();
    }

    // If after all checks, we still have no valid content, exit.
    if (!newContent.isValid()) return;

    // Prevent adding empty text or a null image
    if (newContent.canConvert<QString>() && newContent.toString().isEmpty()) return;
    if (newContent.canConvert<QImage>() && newContent.value<QImage>().isNull()) return;


    if (history.isEmpty() || history.front() != newContent) {
        history.push_front(newContent);
        if (history.size() > 20) {
            history.pop_back();
        }
        updateListWidget();
    }
}

// ---------------------
// Tray Icon (no structural changes)
// ---------------------
void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon::fromTheme("edit-copy"));
    trayIcon->setToolTip("LinClip Clipboard Manager");
    QMenu *menu = new QMenu(this);
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    menu->addAction(quitAction);
    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

// ---------------------
// Helpers
// ---------------------
void MainWindow::updateListWidget()
{
    listWidget->clear();
    for (const QVariant &itemData : history) {
        QListWidgetItem *listItem = new QListWidgetItem();

        if (itemData.canConvert<QImage>()) {
            QImage img = itemData.value<QImage>();
            QIcon icon(QPixmap::fromImage(img.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            listItem->setIcon(icon);
            listItem->setText(QString("[Image %1x%2]").arg(img.width()).arg(img.height()));
        } else if (itemData.canConvert<QString>()) {
            QString fullText = itemData.value<QString>();
            QString firstLine = fullText.split('\n').first().trimmed();
            listItem->setText(firstLine);
        }

        listItem->setData(Qt::UserRole, itemData);
        listWidget->addItem(listItem);
    }
}

void MainWindow::toggleVisibility()
{
    if (isVisible()) {
        hide();
    } else {
        updateListWidget();
        move(QCursor::pos());
        activateWindow();
        raise();
        show();
    }
}

void MainWindow::clearHistory()
{
    history.clear();
    updateListWidget();
    statusBar()->showMessage("History cleared.", 2000);
}
