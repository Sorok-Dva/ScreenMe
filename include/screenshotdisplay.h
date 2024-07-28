#pragma once

#ifndef SCREENSHOTDISPLAY_H
#define SCREENSHOTDISPLAY_H

#include <stack> 
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QWheelEvent>
#include <QPainterPath>
#include <QGraphicsOpacityEffect>
#include <QTextEdit>
#include "editor.h"
#include "config_manager.h"
#include "customTextEdit.h"

class ScreenshotDisplay : public QWidget {
    Q_OBJECT
public:
    explicit ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent = nullptr, ConfigManager* configManager = nullptr);

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
    void undo();

private:
    void initializeEditor();
    void configureShortcuts();
    void updateTooltip();
    void updateEditorPosition();
    void drawHandles(QPainter& painter);
    void drawArrow(QPainter& painter, const QPoint& start, const QPoint& end);
    void drawBorderCircle(QPainter& painter, const QPoint& position);
    void saveStateForUndo();
    void finalizeTextEdit();
    void adjustTextEditSize();
    HandlePosition handleAtPoint(const QPoint& point);
    void resizeSelection(const QPoint& point);
    Qt::CursorShape cursorForHandle(HandlePosition handle);

    std::stack<QPixmap> undoStack;
    QPixmap originalPixmap;
    QPixmap drawingPixmap;
    QPoint origin;
    QPoint drawingEnd;
    QRect selectionRect;
    QRect currentShapeRect;
    QPoint selectionOffset;
    QPoint lastPoint;
    QPoint handleOffset;
    QPoint cursorPosition;
    QPoint textEditPosition;
    QRect textBoundingRect;

    bool selectionStarted;
    bool movingSelection;
    bool drawing;
    bool shapeDrawing;
    bool showBorderCircle;

    int borderWidth;

    QVBoxLayout* actionLayout;

    QColor currentColor;
    Editor::Tool currentTool;
    QFont currentFont;

    QString text;
    CustomTextEdit* textEdit;
    QScopedPointer<Editor> editor;
    ConfigManager* configManager;

    HandlePosition currentHandle;
    QPainterPath drawingPath;
};

#endif // SCREENSHOTDISPLAY_H
