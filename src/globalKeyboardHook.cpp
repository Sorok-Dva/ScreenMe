#include "include/globalKeyboardHook.h"
#include <QDebug>

#ifdef Q_OS_WIN
HHOOK GlobalKeyboardHook::hHook = nullptr;
#endif

GlobalKeyboardHook* GlobalKeyboardHook::instance = nullptr;

GlobalKeyboardHook::GlobalKeyboardHook(QObject* parent) : QObject(parent) {
    instance = this;
}

GlobalKeyboardHook::~GlobalKeyboardHook() {
    stop();
    instance = nullptr;
}

void GlobalKeyboardHook::start() {
#ifdef Q_OS_WIN
    if (!hHook) {
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
        if (!hHook) {
            qWarning() << "Failed to install keyboard hook!";
        }
    }
#elif defined(Q_OS_MAC)
    CGEventMask eventMask = (1 << kCGEventKeyDown) | (1 << kCGEventFlagsChanged);
    eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, CGEventCallback, this);
    if (!eventTap) {
        qWarning("Failed to create event tap");
        return;
    }
    runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
#endif
}

void GlobalKeyboardHook::stop() {
#ifdef Q_OS_WIN
    if (hHook) {
        UnhookWindowsHookEx(hHook);
        hHook = nullptr;
    }
#elif defined(Q_OS_MAC)
    if (eventTap) {
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
        CFRelease(runLoopSource);
        CFMachPortInvalidate(eventTap);
        CFRelease(eventTap);
        eventTap = nullptr;
    }
#endif
}

#ifdef Q_OS_WIN
LRESULT CALLBACK GlobalKeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        int key = p->vkCode;
        instance->modifiers = Qt::NoModifier;

        if (key == VK_SNAPSHOT) {
            key = Qt::Key_Print; // Map VK_SNAPSHOT to Qt::Key_Print
        }

        if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) instance->modifiers |= Qt::ControlModifier;
        if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) instance->modifiers |= Qt::ControlModifier;
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) instance->modifiers |= Qt::ShiftModifier;
        if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) instance->modifiers |= Qt::ShiftModifier;
        if (GetAsyncKeyState(VK_LMENU) & 0x8000) instance->modifiers |= Qt::AltModifier;
        if (GetAsyncKeyState(VK_RMENU) & 0x8000) instance->modifiers |= Qt::AltModifier;
        if (GetAsyncKeyState(VK_LWIN) & 0x8000 || GetAsyncKeyState(VK_RWIN) & 0x8000) instance->modifiers |= Qt::MetaModifier;

        // Ignore modifier key presses by themselves
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
            if (!(key == VK_LCONTROL || key == VK_RCONTROL || key == VK_LSHIFT || key == VK_RSHIFT || key == VK_LMENU || key == VK_RMENU || key == VK_LWIN || key == VK_RWIN)) {
                QKeySequence keySequence = QKeySequence(instance->modifiers | key);
                instance->currentSequence = keySequence;
                emit instance->keyPressed(instance->currentSequence);
                return 1; // Block the event so Windows doesn't process it
            }
        }

        if (wParam == WM_SYSKEYUP || wParam == WM_KEYUP) {
            instance->modifiers = Qt::NoModifier;
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
#elif defined(Q_OS_MAC)
CGEventRef GlobalKeyboardHook::CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) {
    if (type == kCGEventKeyDown || type == kCGEventFlagsChanged) {
        GlobalKeyboardHook* hook = static_cast<GlobalKeyboardHook*>(refcon);
        CGKeyCode keyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        CGEventFlags flags = CGEventGetFlags(event);
        hook->modifiers = Qt::NoModifier;
        if (flags & kCGEventFlagMaskControl)
            hook->modifiers |= Qt::ControlModifier;
        if (flags & kCGEventFlagMaskShift)
            hook->modifiers |= Qt::ShiftModifier;
        if (flags & kCGEventFlagMaskAlternate)
            hook->modifiers |= Qt::AltModifier;
        if (flags & kCGEventFlagMaskCommand)
            hook->modifiers |= Qt::MetaModifier;

        if (type == kCGEventKeyDown) {
            QKeySequence keySequence = QKeySequence(keyCode | hook->modifiers);
            hook->currentSequence = keySequence;
            emit hook->keyPressed(hook->currentSequence);
        }

        if (type == kCGEventFlagsChanged) {
            hook->modifiers = Qt::NoModifier;
        }
    }
    return event;
}
#endif
