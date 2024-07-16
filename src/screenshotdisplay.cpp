#include "include/screenshotdisplay.h"
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>

ScreenshotDisplay::ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent)
    : QWidget(parent), originalPixmap(pixmap), selectionStarted(false) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setGeometry(QApplication::primaryScreen()->geometry());
    QLabel* label = new QLabel(this);
    label->setPixmap(pixmap);
    label->setScaledContents(true);
    label->setGeometry(this->geometry());

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(label);
    effect->setOpacity(0.6);
    label->setGraphicsEffect(effect);

    // Ensure closing this window does not quit the app
    setAttribute(Qt::WA_QuitOnClose, false);

    connect(this, &ScreenshotDisplay::destroyed, [this]() {
        emit screenshotClosed();
    });

    showFullScreen();
}

void ScreenshotDisplay::mousePressEvent(QMouseEvent* event) {
    selectionStarted = true;
    origin = event->pos();
    selectionRect = QRect(origin, QSize());
}

void ScreenshotDisplay::mouseMoveEvent(QMouseEvent* event) {
    if (selectionStarted) {
        selectionRect = QRect(origin, event->pos()).normalized();
        update();
    }
}

void ScreenshotDisplay::mouseReleaseEvent(QMouseEvent* event) {
    selectionStarted = false;
    copySelectionToClipboard();
    close();
}

void ScreenshotDisplay::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    else if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
        copySelectionToClipboard();
        close();
    }
}

void ScreenshotDisplay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, originalPixmap);
    if (selectionStarted) {
        painter.setPen(Qt::red);
        painter.drawRect(selectionRect);
    }
}

void ScreenshotDisplay::copySelectionToClipboard() {
    if (selectionRect.isValid()) {
        QPixmap selectedPixmap = originalPixmap.copy(selectionRect);
        QApplication::clipboard()->setPixmap(selectedPixmap);
    }
}
