#ifndef HOTKEYEVENTFILTER_H
#define HOTKEYEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QMainWindow>

class HotkeyEventFilter : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    explicit HotkeyEventFilter(QMainWindow* mainWindow);

    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

signals:
    void hotkeyPressed(quint32 id);

private:
    QMainWindow* mainWindow;
};

#endif // HOTKEYEVENTFILTER_H
