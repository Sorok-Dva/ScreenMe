#ifndef SCREENSHOTDISPLAY_H
#define SCREENSHOTDISPLAY_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QWheelEvent>
#include <QGraphicsOpacityEffect>
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
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onToolSelected(Editor::Tool tool);
    void onColorChanged(const QColor& color);

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
    void resizeSelection(const QPoint& point);
    void drawHandles(QPainter& painter);
    void copySelectionToClipboard();
    void updateTooltip();
    void updateEditorPosition();
    Qt::CursorShape cursorForHandle(HandlePosition handle);
    void drawBorderCircle(QPainter& painter, const QPoint& position);

    QPixmap originalPixmap;
    QPixmap drawingPixmap;
    QRect selectionRect;
    QRect currentShapeRect;
    QPoint origin;
    bool selectionStarted;
    bool movingSelection;
    bool drawing;
    bool shapeDrawing;
    QPoint selectionOffset;
    QPoint lastPoint;
    HandlePosition currentHandle;
    QPoint handleOffset;
    QColor currentColor;
    Editor::Tool currentTool;
    int borderWidth;

    Editor* editor;
    QPushButton* colorButton;
};

#endif // SCREENSHOTDISPLAY_H
