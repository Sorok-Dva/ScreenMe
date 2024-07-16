#ifndef SCREENSHOTDISPLAY_H
#define SCREENSHOTDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPen>
#include <QColorDialog>
#include "editor.h"

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
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    enum HandlePosition {
        None,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Top,
        Bottom,
        Left,
        Right
    };

    HandlePosition handleAtPoint(const QPoint& point);
    Qt::CursorShape cursorForHandle(HandlePosition handle);
    void resizeSelection(const QPoint& point);
    void drawHandles(QPainter& painter);
    void updateTooltip();
    void copySelectionToClipboard();
    void setToolActive(bool active);
    void deselectTool();
    void updateColorButton();

    QPixmap originalPixmap;
    QPixmap semiTransparentPixmap;
    QRect selectionRect;
    QPoint origin;
    bool selectionStarted;
    bool movingSelection;
    HandlePosition currentHandle;
    QPoint handleOffset;
    QPoint selectionOffset;

    Editor* editor;
    int currentTool;
    bool isToolActive;
    int borderSize;
    QColor currentColor;

    QPushButton* colorButton;
};

#endif // SCREENSHOTDISPLAY_H
