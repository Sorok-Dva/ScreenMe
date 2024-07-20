#include "include/hotkeyEventFilter.h"
#include <Windows.h>

HotkeyEventFilter::HotkeyEventFilter(QMainWindow* mainWindow)
    : QObject(mainWindow), mainWindow(mainWindow) {}

bool HotkeyEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) {
    Q_UNUSED(result);

    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            emit hotkeyPressed(msg->wParam);
            return true;
        }
    }

    return false;
}