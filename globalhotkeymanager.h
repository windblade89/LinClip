#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H

#include <QObject>
#include <memory> // Required for std::unique_ptr

// Forward declare the private implementation class.
// This hides all X11 details from any file that includes this header.
class HotkeyPrivate;

class GlobalHotkeyManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalHotkeyManager(QObject *parent = nullptr);
    ~GlobalHotkeyManager();

public slots:
    void run(); // This will contain the listening loop

signals:
    void hotkeyActivated();
    void finished();

private:
    // A single, smart pointer to our private implementation.
    std::unique_ptr<HotkeyPrivate> d;
};

#endif // GLOBALHOTKEYMANAGER_H
