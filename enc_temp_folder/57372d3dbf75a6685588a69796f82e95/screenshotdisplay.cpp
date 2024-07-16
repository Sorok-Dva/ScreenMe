#include "include/screenshotdisplay.h"
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#include <QKeyEvent>
#include <QToolTip>
#include <QCursor>
#include <QDebug>
#include <QWheelEvent>

ScreenshotDisplay::ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent)
    : QWidget(parent), originalPixmap(pixmap), selectionStarted(false), movingSelection(false), currentHandle(None), editor(nullptr), drawing(false), shapeDrawing(false), currentColor(Qt::black), currentTool(Editor::None), borderWidth(2), drawingPixmap(pixmap.size()) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowTitle("ScreenMe");
    setWindowIcon(QIcon("resources/icon.png"));
    setAttribute(Qt::WA_QuitOnClose, false);
    setGeometry(QApplication::primaryScreen()->geometry());

    // Initialize drawing pixmap
    drawingPixmap.fill(Qt::transparent);

    QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escapeShortcut, &QShortcut::activated, [this]() {
        if (editor->getCurrentTool() != Editor::None) {
            editor->deselectTools();
            setCursor(Qt::ArrowCursor);
        }
        else {
            close();
        }
    });

    QShortcut* copyShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    connect(copyShortcut, &QShortcut::activated, this, &ScreenshotDisplay::copySelectionToClipboard);

    editor = new Editor(this);
    editor->hide();
    connect(editor, &Editor::toolChanged, this, &ScreenshotDisplay::onToolSelected);
    connect(editor, &Editor::colorChanged, this, &ScreenshotDisplay::onColorChanged);

    // Initialize Color Button
    colorButton = new QPushButton(this);
    colorButton->setFixedSize(24, 24);
    colorButton->setStyleSheet("background-color: black");
    connect(colorButton, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::black, this, "Select Color");
        if (color.isValid()) {
            editor->setCurrentColor(color);
            colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        }
    });

    showFullScreen();
}

void ScreenshotDisplay::closeEvent(QCloseEvent* event) {
    emit screenshotClosed();
    if (editor) {
        editor->hide();
    }
    QWidget::closeEvent(event);
}

void ScreenshotDisplay::mousePressEvent(QMouseEvent* event) {
    if (editor->getCurrentTool() == Editor::None) {
        HandlePosition handle = handleAtPoint(event->pos());
        if (handle != None) {
            currentHandle = handle;
            handleOffset = event->pos() - selectionRect.topLeft();
        }
        else if (selectionRect.contains(event->pos())) {
            movingSelection = true;
            selectionOffset = event->pos() - selectionRect.topLeft();
        }
        else {
            selectionStarted = true;
            origin = event->pos();
            selectionRect = QRect(origin, QSize());
            currentHandle = None;
            movingSelection = false;
        }
    }
    else {
        drawing = true;
        lastPoint = event->pos();
        if (editor->getCurrentTool() != Editor::Pen) {
            shapeDrawing = true;
            currentShapeRect = QRect(lastPoint, QSize());
        }
    }
}

void ScreenshotDisplay::mouseMoveEvent(QMouseEvent* event) {
    if (selectionStarted) {
        selectionRect = QRect(origin, event->pos()).normalized();
        update();
        updateTooltip();
        updateEditorPosition();
    }
    else if (drawing && editor->getCurrentTool() == Editor::Pen) {
        QPixmap tempPixmap = drawingPixmap.copy();
        QPainter painter(&tempPixmap);
        painter.setPen(QPen(currentColor, borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(lastPoint, event->pos());
        lastPoint = event->pos();
        drawingPixmap = tempPixmap;
        update();
    }
    else if (shapeDrawing) {
        currentShapeRect = QRect(lastPoint, event->pos()).normalized();
        update();
    }
    else if (movingSelection) {
        selectionRect.moveTopLeft(event->pos() - selectionOffset);
        update();
        updateTooltip();
    }

    HandlePosition handle = handleAtPoint(event->pos());
    if (handle != None) {
        setCursor(cursorForHandle(handle));
    }
    else if (selectionRect.contains(event->pos()) && editor->getCurrentTool() == Editor::None) {
        setCursor(Qt::SizeAllCursor);
    }
    else {
        setCursor(Qt::ArrowCursor);
    }

    if (selectionRect.isValid() && editor->isHidden()) {
        updateEditorPosition();
        editor->show();
    }

    // Update border circle position
    if (editor->getCurrentTool() != Editor::None) {
        update();
    }
}

void ScreenshotDisplay::mouseReleaseEvent(QMouseEvent* event) {
    selectionStarted = false;
    movingSelection = false;
    currentHandle = None;
    drawing = false;

    if (shapeDrawing) {
        QPixmap tempPixmap = drawingPixmap.copy();
        QPainter painter(&tempPixmap);
        painter.setPen(QPen(currentColor, borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        switch (editor->getCurrentTool()) {
        case Editor::Rectangle:
            painter.drawRect(currentShapeRect);
            break;
        case Editor::Ellipse:
            painter.drawEllipse(currentShapeRect);
            break;
        case Editor::Line:
            painter.drawLine(currentShapeRect.topLeft(), currentShapeRect.bottomRight());
            break;
        case Editor::Arrow:
            // Draw arrow
            painter.drawLine(currentShapeRect.topLeft(), currentShapeRect.bottomRight());
            // Add arrow head drawing logic here
            break;
        case Editor::Text:
            // Implement text drawing
            break;
        default:
            break;
        }

        drawingPixmap = tempPixmap;
        shapeDrawing = false;
        update();
    }

    updateTooltip();
}

void ScreenshotDisplay::keyPressEvent(QKeyEvent* event) {
    if (editor->getCurrentTool() != Editor::None && event->key() == Qt::Key_Escape) {
        editor->deselectTools();
        setCursor(Qt::ArrowCursor);
    }
    else if (event->key() == Qt::Key_Escape) {
        close();
    }
    else if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
        copySelectionToClipboard();
        close();
    }
}

void ScreenshotDisplay::wheelEvent(QWheelEvent* event) {
    if (editor->getCurrentTool() != Editor::None) {
        borderWidth += event->angleDelta().y() / 120;
        if (borderWidth < 1) borderWidth = 1;
        if (borderWidth > 20) borderWidth = 20;
        update();
    }
}

void ScreenshotDisplay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, originalPixmap);
    painter.drawPixmap(0, 0, drawingPixmap);

    if (selectionRect.isValid()) {
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.drawRect(selectionRect);
        drawHandles(painter);
    }

    if (shapeDrawing && editor->getCurrentTool() != Editor::Pen) {
        painter.setPen(QPen(currentColor, borderWidth, Qt::DashLine));
        switch (editor->getCurrentTool()) {
        case Editor::Rectangle:
            painter.drawRect(currentShapeRect);
            break;
        case Editor::Ellipse:
            painter.drawEllipse(currentShapeRect);
            break;
        case Editor::Line:
            painter.drawLine(currentShapeRect.topLeft(), currentShapeRect.bottomRight());
            break;
        case Editor::Arrow:
            painter.drawLine(currentShapeRect.topLeft(), currentShapeRect.bottomRight());
            // Add arrow head drawing logic here
            break;
        default:
            break;
        }
    }

    if (editor->getCurrentTool() != Editor::None) {
        drawBorderCircle(painter, mapFromGlobal(QCursor::pos()));
    }
}

void ScreenshotDisplay::copySelectionToClipboard() {
    QPixmap resultPixmap = originalPixmap;
    QPainter painter(&resultPixmap);
    painter.drawPixmap(0, 0, drawingPixmap);

    if (selectionRect.isValid()) {
        QPixmap selectedPixmap = resultPixmap.copy(selectionRect);
        QApplication::clipboard()->setPixmap(selectedPixmap);
    }
    else {
        QApplication::clipboard()->setPixmap(resultPixmap);
    }
    close();
}

void ScreenshotDisplay::updateTooltip() {
    if (selectionRect.isValid()) {
        QString tooltipText = QString("Size: %1 x %2").arg(selectionRect.width()).arg(selectionRect.height());
        QToolTip::showText(mapToGlobal(selectionRect.topRight()), tooltipText, this);
    }
}

void ScreenshotDisplay::drawHandles(QPainter& painter) {
    const int handleSize = 3;
    const QVector<QPoint> handlePoints = {
        selectionRect.topLeft(),
        selectionRect.topRight(),
        selectionRect.bottomLeft(),
        selectionRect.bottomRight(),
        selectionRect.topLeft() + QPoint(selectionRect.width() / 2, 0),
        selectionRect.bottomLeft() + QPoint(selectionRect.width() / 2, 0),
        selectionRect.topLeft() + QPoint(0, selectionRect.height() / 2),
        selectionRect.topRight() + QPoint(0, selectionRect.height() / 2)
    };
    painter.setBrush(Qt::red);
    for (const QPoint& point : handlePoints) {
        painter.drawRect(QRect(point - QPoint(handleSize / 2, handleSize / 2), QSize(handleSize * 2, handleSize * 2)));
    }
}

ScreenshotDisplay::HandlePosition ScreenshotDisplay::handleAtPoint(const QPoint& point) {
    const int handleSize = 20;
    const QRect handleRect(QPoint(0, 0), QSize(handleSize, handleSize));
    if (handleRect.translated(selectionRect.topLeft()).contains(point)) return TopLeft;
    if (handleRect.translated(selectionRect.topRight()).contains(point)) return TopRight;
    if (handleRect.translated(selectionRect.bottomLeft()).contains(point)) return BottomLeft;
    if (handleRect.translated(selectionRect.bottomRight()).contains(point)) return BottomRight;
    if (handleRect.translated(selectionRect.topLeft() + QPoint(selectionRect.width() / 2, 0)).contains(point)) return Top;
    if (handleRect.translated(selectionRect.bottomLeft() + QPoint(selectionRect.width() / 2, 0)).contains(point)) return Bottom;
    if (handleRect.translated(selectionRect.topLeft() + QPoint(0, selectionRect.height() / 2)).contains(point)) return Left;
    if (handleRect.translated(selectionRect.topRight() + QPoint(0, selectionRect.height() / 2)).contains(point)) return Right;
    return None;
}

void ScreenshotDisplay::resizeSelection(const QPoint& point) {
    switch (currentHandle) {
    case TopLeft:
        selectionRect.setTopLeft(point);
        break;
    case TopRight:
        selectionRect.setTopRight(point);
        break;
    case BottomLeft:
        selectionRect.setBottomLeft(point);
        break;
    case BottomRight:
        selectionRect.setBottomRight(point);
        break;
    case Top:
        selectionRect.setTop(point.y());
        break;
    case Bottom:
        selectionRect.setBottom(point.y());
        break;
    case Left:
        selectionRect.setLeft(point.x());
        break;
    case Right:
        selectionRect.setRight(point.x());
        break;
    default:
        break;
    }
    selectionRect = selectionRect.normalized();
}

Qt::CursorShape ScreenshotDisplay::cursorForHandle(HandlePosition handle) {
    switch (handle) {
    case TopLeft:
    case BottomRight:
        return Qt::SizeFDiagCursor;
    case TopRight:
    case BottomLeft:
        return Qt::SizeBDiagCursor;
    case Top:
    case Bottom:
        return Qt::SizeVerCursor;
    case Left:
    case Right:
        return Qt::SizeHorCursor;
    default:
        return Qt::ArrowCursor;
    }
}

void ScreenshotDisplay::onToolSelected(Editor::Tool tool) {
    currentTool = tool;
    if (tool == Editor::None) {
        setCursor(Qt::ArrowCursor);
    }
    else {
        setCursor(Qt::CrossCursor);
    }
}

void ScreenshotDisplay::onColorChanged(const QColor& color) {
    currentColor = color;
    colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
}

void ScreenshotDisplay::updateEditorPosition() {
    if (selectionRect.isValid()) {
        const int margin = 10;
        editor->move(selectionRect.bottomRight() + QPoint(margin, margin));
        colorButton->move(editor->pos() + QPoint(0, editor->height() + margin));
    }
}

void ScreenshotDisplay::drawBorderCircle(QPainter& painter, const QPoint& position) {
    painter.setPen(QPen(Qt::red , 2, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(position, borderWidth, borderWidth);
}
