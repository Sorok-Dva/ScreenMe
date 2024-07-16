#pragma once

#include <QWidget>
#include <QPixmap>
#include <QRect>
#include <QPoint>
#include <QGraphicsOpacityEffect>
#include <QLabel>

class ScreenshotDisplay : public QWidget {
    Q_OBJECT
public:
    explicit ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent = nullptr);

signals:
    void screenshotClosed();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void copySelectionToClipboard();

    QPixmap originalPixmap;
    bool selectionStarted;
    QPoint origin;
    QRect selectionRect;
};
