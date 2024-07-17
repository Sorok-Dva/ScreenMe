#ifndef SCREENSHOTDISPLAY_H
#define SCREENSHOTDISPLAY_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QWheelEvent>
#include <QPainterPath>
#include <QGraphicsOpacityEffect>
#include "editor.h"
#include "config_manager.h"

class ScreenshotDisplay : public QWidget {
    Q_OBJECT
public:
    explicit ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent = nullptr, ConfigManager* configManager = nullptr);

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
    void onSaveRequested();
    void onPublishRequested();
    void onCloseRequested();
    void copySelectionToClipboard();

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

    ConfigManager* configManager;

    HandlePosition handleAtPoint(const QPoint& point);
    void resizeSelection(const QPoint& point);
    void drawHandles(QPainter& painter);
    void drawArrow(QPainter& painter, const QPoint& start, const QPoint& end);
    void updateTooltip();
    void updateEditorPosition();
    Qt::CursorShape cursorForHandle(HandlePosition handle);
    void drawBorderCircle(QPainter& painter, const QPoint& position);

    QPixmap originalPixmap;
    QPixmap drawingPixmap;

    QPoint origin;
    QPoint drawingEnd;

    QRect selectionRect;
    QRect currentShapeRect;

    bool selectionStarted;
    bool movingSelection;

    QVBoxLayout* actionLayout;

    Editor* editor;

    bool drawing;
    bool shapeDrawing;
    QPoint selectionOffset;
    QPoint lastPoint;
    HandlePosition currentHandle;
    QPoint handleOffset;
    QColor currentColor;
    Editor::Tool currentTool;
    int borderWidth;
    QPainterPath drawingPath;
    bool showBorderCircle;
    QPoint cursorPosition;
    QRect textBoundingRect;
    QString text;
    QFont currentFont;
};

#endif // SCREENSHOTDISPLAY_H
