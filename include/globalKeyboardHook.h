#ifndef GLOBALKEYBOARDHOOK_H
#define GLOBALKEYBOARDHOOK_H

#include <QObject>
#include <Windows.h>
#include <QKeySequence>

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
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK hHook;
    static GlobalKeyboardHook* instance;
    QKeySequence currentSequence;
    Qt::KeyboardModifiers modifiers;
};

#endif // GLOBALKEYBOARDHOOK_H
