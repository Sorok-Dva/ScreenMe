#include "include/screenshotdisplay.h"
#include "include/editor.h"
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#include <QKeyEvent>
#include <QToolTip>
#include <QCursor>
#include <QColorDialog>
#include <QDebug>

ScreenshotDisplay::ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent)
    : QWidget(parent), originalPixmap(pixmap), selectionStarted(false), movingSelection(false), currentHandle(None),
    currentTool(Editor::None), isToolActive(false), borderSize(2), currentColor(Qt::red) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowTitle("ScreenMe");
    setWindowIcon(QIcon("resources/icon.png"));
    setAttribute(Qt::WA_QuitOnClose, false);
    setGeometry(QApplication::primaryScreen()->geometry());
    QLabel* label = new QLabel(this);
    label->setPixmap(pixmap);
    label->setScaledContents(true);
    label->setGeometry(this->geometry());

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(label);
    effect->setOpacity(0.6);
    label->setGraphicsEffect(effect);

    semiTransparentPixmap = QPixmap(originalPixmap.size());
    semiTransparentPixmap.fill(Qt::transparent);

    QPainter painter(&semiTransparentPixmap);
    painter.setOpacity(0.6);
    painter.drawPixmap(0, 0, originalPixmap);
    painter.end();

    QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escapeShortcut, &QShortcut::activated, this, &ScreenshotDisplay::close);

    QShortcut* copyShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    connect(copyShortcut, &QShortcut::activated, this, &ScreenshotDisplay::copySelectionToClipboard);

    // Initialize color button
    colorButton = new QPushButton(this);
    colorButton->setFixedSize(20, 20);
    updateColorButton();
    connect(colorButton, &QPushButton::clicked, this, [this]() {
        QColor chosenColor = QColorDialog::getColor(currentColor, this, "Select Color");
        if (chosenColor.isValid()) {
            currentColor = chosenColor;
            updateColorButton();
        }
    });

    showFullScreen();
}

void ScreenshotDisplay::updateColorButton() {
    QPixmap pixmap(20, 20);
    pixmap.fill(currentColor);
    colorButton->setIcon(QIcon(pixmap));
}

void ScreenshotDisplay::closeEvent(QCloseEvent* event) {
    emit screenshotClosed();
    QWidget::closeEvent(event);
}

void ScreenshotDisplay::mousePressEvent(QMouseEvent* event) {
    HandlePosition handle = handleAtPoint(event->pos());
    if (handle != None) {
        currentHandle = handle;
        handleOffset = event->pos() - selectionRect.topLeft();
    }
    else if (selectionRect.contains(event->pos()) && !isToolActive) {
        movingSelection = true;
        selectionOffset = event->pos() - selectionRect.topLeft();
    }
    else if (isToolActive) {
        // Handle tool usage
        if (currentTool == Editor::Pen) {
            // Handle pen drawing logic
        }
        else if (currentTool == Editor::Text) {
            // Handle text input logic
        }
        // Handle other tools similarly
    }
    else {
        selectionStarted = true;
        origin = event->pos();
        selectionRect = QRect(origin, QSize());
        currentHandle = None;
        movingSelection = false;
    }
}

void ScreenshotDisplay::mouseMoveEvent(QMouseEvent* event) {
    if (selectionStarted) {
        selectionRect = QRect(origin, event->pos()).normalized();
        update();
        updateTooltip();
    }
    else if (currentHandle != None) {
        resizeSelection(event->pos());
        update();
        updateTooltip();
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
    else if (selectionRect.contains(event->pos()) && !isToolActive) {
        setCursor(Qt::SizeAllCursor);
    }
    else {
        setCursor(Qt::ArrowCursor);
    }
}

void ScreenshotDisplay::mouseReleaseEvent(QMouseEvent* event) {
    selectionStarted = false;
    movingSelection = false;
    currentHandle = None;
    updateTooltip();
}

void ScreenshotDisplay::wheelEvent(QWheelEvent* event) {
    if (isToolActive) {
        borderSize += event->angleDelta().y() / 120; // Assuming each wheel step changes border size by 1
        borderSize = qMax(1, borderSize); // Ensure border size is at least 1
        update();
    }
}

void ScreenshotDisplay::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (isToolActive) {
            deselectTool();
        }
        else {
            close();
        }
    }
    else if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
        copySelectionToClipboard();
        close();
    }
}

void ScreenshotDisplay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, semiTransparentPixmap);

    if (selectionRect.isValid()) {
        painter.drawPixmap(selectionRect, originalPixmap, selectionRect);

        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.drawRect(selectionRect);
        drawHandles(painter);

        if (isToolActive && currentTool == Editor::Pen) {
            // Handle pen drawing logic
        }
        // Handle other tools similarly
    }
}

void ScreenshotDisplay::copySelectionToClipboard() {
    if (selectionRect.isValid()) {
        QPixmap selectedPixmap = originalPixmap.copy(selectionRect);
        QApplication::clipboard()->setPixmap(selectedPixmap);
        close();
    }
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

void ScreenshotDisplay::setToolActive(bool active) {
    isToolActive = active;
}

void ScreenshotDisplay::deselectTool() {
    currentTool = Editor::None;
    setToolActive(false);
    setCursor(Qt::ArrowCursor);
    editor->deselectTools();
}
