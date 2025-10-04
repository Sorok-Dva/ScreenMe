#ifndef GLOBALKEYBOARDHOOK_H
#define GLOBALKEYBOARDHOOK_H

#include <QObject>
#include <QKeySequence>

#ifdef Q_OS_WIN
#include <Windows.h>

class GlobalKeyboardHook : public QObject {
    Q_OBJECT

public:
    explicit GlobalKeyboardHook(QObject* parent = nullptr);
    ~GlobalKeyboardHook();

    void start();
    void stop();

signals:
    void keyPressed(QKeySequence keySequence);

private:
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK hHook;
    static GlobalKeyboardHook* instance;
    QKeySequence currentSequence;
    Qt::KeyboardModifiers modifiers;
};
#else

class GlobalKeyboardHook : public QObject {
    Q_OBJECT

public:
    explicit GlobalKeyboardHook(QObject* parent = nullptr) : QObject(parent) {}
    ~GlobalKeyboardHook() override = default;

    void start() {}
    void stop() {}

signals:
    void keyPressed(QKeySequence keySequence);
};

#endif // Q_OS_WIN

#endif // GLOBALKEYBOARDHOOK_H
