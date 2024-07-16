#pragma once

#include <QWidget>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QRect>
#include <QPixmap>

class ScreenshotDisplay : public QWidget {
    Q_OBJECT

public:
    explicit ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
//    void mouseMoveEvent(QMouseEvent* event) override;
//    void mouseReleaseEvent(QMouseEvent* event) override;
//    void keyPressEvent(QKeyEvent* event) override;
//    void paintEvent(QPaintEvent* event) override;

signals:
    void screenshotClosed();

private:
    //void copySelectionToClipboard();

    QPixmap originalPixmap;
    bool selectionStarted;
    QPoint origin;
    QRect selectionRect;
};

