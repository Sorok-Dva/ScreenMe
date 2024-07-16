#ifndef SCREENSHOTDISPLAY_H
#define SCREENSHOTDISPLAY_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QGraphicsOpacityEffect>

class ScreenshotDisplay : public QWidget {
    Q_OBJECT

public:
    explicit ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent = nullptr);

signals:
    void screenshotClosed();

protected:
    void closeEvent(QCloseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    enum HandlePosition { None, TopLeft, TopRight, BottomLeft, BottomRight, Top, Bottom, Left, Right };
    HandlePosition handleAtPoint(const QPoint& point);
    void resizeSelection(const QPoint& point);
    Qt::CursorShape cursorForHandle(HandlePosition handle);
    void copySelectionToClipboard();
    void updateTooltip();
    void drawHandles(QPainter& painter);

    QPixmap originalPixmap;
    QPixmap semiTransparentPixmap;
    QRect selectionRect;
    QPoint origin;
    bool selectionStarted;
    bool movingSelection;
    HandlePosition currentHandle;
    QPoint selectionOffset;
    QPoint handleOffset;
};

#endif // SCREENSHOTDISPLAY_H
