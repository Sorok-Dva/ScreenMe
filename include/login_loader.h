#pragma once

#ifndef LOGIN_LOADER_H
#define LOGIN_LOADER_H

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>

class LoginLoader : public QWidget {
    Q_OBJECT
public:
    explicit LoginLoader(QWidget* parent = nullptr);

private:
    QProgressBar* loadingBar;
};

#endif // LOGIN_LOADER_H
