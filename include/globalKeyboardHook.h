#pragma once

#ifndef GLOBALKEYBOARDHOOK_H
#define GLOBALKEYBOARDHOOK_H

#include <QObject>
#include <QKeySequence>

#ifdef Q_OS_WIN
#include <Windows.h>
#elif defined(Q_OS_MAC)
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

class GlobalKeyboardHook : public QObject {
    Q_OBJECT

public:
    GlobalKeyboardHook(QObject* parent = nullptr);
    ~GlobalKeyboardHook();

    void start();
    void stop();

signals:
    void keyPressed(QKeySequence keySequence);

private:
#ifdef Q_OS_WIN
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK hHook;
#elif defined(Q_OS_MAC)
    static CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon);
    CFMachPortRef eventTap;
    CFRunLoopSourceRef runLoopSource;
#endif
    static GlobalKeyboardHook* instance;
    QKeySequence currentSequence;
    Qt::KeyboardModifiers modifiers;
};

#endif // GLOBALKEYBOARDHOOK_H
