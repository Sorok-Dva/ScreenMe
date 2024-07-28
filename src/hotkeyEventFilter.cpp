#include "include/hotkeyEventFilter.h"

#ifdef _WIN32
#include <Windows.h>
#endif

HotkeyEventFilter::HotkeyEventFilter(QMainWindow* mainWindow)
    : QObject(mainWindow), mainWindow(mainWindow) {}

bool HotkeyEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(result);

#ifdef _WIN32
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            emit hotkeyPressed(msg->wParam);
            return true;
        }
    }
#elif defined(Q_OS_MAC)
    qDebug() << "mac pressed";
#endif

    return false;
}
