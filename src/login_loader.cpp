#include "include/login_loader.h"
#include <QVBoxLayout>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>

LoginLoader::LoginLoader(QWidget* parent)
    : QWidget(parent), loadingBar(new QProgressBar(this)) {
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("ScreenMe - Waiting for login");
    setAttribute(Qt::WA_QuitOnClose, false);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(loadingBar);

    loadingBar->setRange(0, 0);
    loadingBar->setFixedHeight(30);

    int windowWidth = 250;
    int windowHeight = loadingBar->sizeHint().height();
    resize(windowWidth, windowHeight);

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    move(screenGeometry.right() - width(), screenGeometry.bottom() - height() - 50);
}
