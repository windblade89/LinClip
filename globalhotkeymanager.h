#pragma once

#include <QObject>
#include <memory> // For std::unique_ptr

// This is the PIMPL pattern. The header is clean of any X11 includes.
class HotkeyPrivate;

class GlobalHotkeyManager : public QObject
{
    Q_OBJECT

public:
    explicit GlobalHotkeyManager(QObject *parent = nullptr);
    ~GlobalHotkeyManager();

public slots:
    void run();
    // --- ADDED: This slot will be connected to the application's aboutToQuit signal ---
    void stop();

signals:
    // --- ADDED: This signal is emitted when the hotkey is pressed ---
    void hotkeyPressed();
    void finished();

private:
    std::unique_ptr<HotkeyPrivate> d; // Pointer to implementation
    volatile bool m_stop = false;
};
