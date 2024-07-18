#include "include/screenshotdisplay.h"
#include "include/DraggableTextItem.h"
#include "include/config_manager.h"
#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#include <QKeyEvent>
#include <QToolTip>
#include <QCursor>
#include <QDebug>
#include <QWheelEvent>
#include <include/utils.h>

ScreenshotDisplay::ScreenshotDisplay(const QPixmap& pixmap, QWidget* parent, ConfigManager* configManager)
    : QWidget(parent), originalPixmap(pixmap), selectionStarted(false), movingSelection(false), currentHandle(None), editor(nullptr), configManager(configManager),
    drawing(false), shapeDrawing(false), currentColor(Qt::black), currentTool(Editor::None), borderWidth(2), 
    drawingPixmap(pixmap.size()), currentFont("Arial", 16), text("Editable Text") {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowTitle("ScreenMe");
    setWindowIcon(QIcon("resources/icon.png"));
    setAttribute(Qt::WA_QuitOnClose, false);
    setGeometry(QApplication::primaryScreen()->geometry());

    editor = new Editor(this);

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

    connect(editor, &Editor::toolChanged, this, [this](Editor::Tool tool) {
        if (tool == Editor::None) {
            setCursor(Qt::ArrowCursor);
        } else {
            setCursor(Qt::CrossCursor);
        }
    });
    connect(editor, &Editor::colorChanged, this, [this](const QColor& color) {
        update();
    });
    connect(editor, &Editor::saveRequested, this, &ScreenshotDisplay::onSaveRequested);
    connect(editor, &Editor::copyRequested, this, &ScreenshotDisplay::copySelectionToClipboard);
    connect(editor, &Editor::publishRequested, this, &ScreenshotDisplay::onPublishRequested);
    connect(editor, &Editor::closeRequested, this, &ScreenshotDisplay::onCloseRequested);

    QFontMetrics fm(currentFont);
    textBoundingRect = QRect(QPoint(100, 100), fm.size(0, text));

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
    } else if (editor->getCurrentTool() == Editor::Text) {
        if (textBoundingRect.contains(event->pos())) {
            bool ok;
            QString newText = QInputDialog::getText(this, "Input Text", "Enter your text:", QLineEdit::Normal, text, &ok);
            if (ok && !newText.isEmpty()) {
                text = newText;
                QFontMetrics fm(currentFont);
                textBoundingRect = QRect(event->pos(), fm.size(0, text));
                update();
            }
        }
        update();
    } else {
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
        painter.setPen(QPen(editor->getCurrentColor(), borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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
         updateEditorPosition();
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

    // Redraw selection rectangle if selection is valid
    if (selectionRect.isValid()) {
        update();
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
        painter.setPen(QPen(editor->getCurrentColor(), borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

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
    if (editor->getCurrentTool() != Editor::None && editor->getCurrentTool() != Editor::Text) {
        borderWidth += event->angleDelta().y() / 120;
        if (borderWidth < 1) borderWidth = 1;
        if (borderWidth > 20) borderWidth = 20;
        update();
    }
    if (editor->getCurrentTool() == Editor::Text) {
        int delta = event->angleDelta().y() / 120;
        int newSize = currentFont.pointSize() + delta;
        if (newSize > 0) {
            currentFont.setPointSize(newSize);
            QFontMetrics fm(currentFont);
            textBoundingRect = QRect(textBoundingRect.topLeft(), fm.size(0, text));
            update();
        }
        return;
    }

}

void ScreenshotDisplay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, originalPixmap);
    painter.drawPixmap(0, 0, drawingPixmap);

    if (selectionRect.isValid()) {
        //painter.drawPixmap(selectionRect, originalPixmap, selectionRect);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.drawRect(selectionRect);
        drawHandles(painter);
    }

    if (shapeDrawing) {
        painter.setPen(QPen(editor->getCurrentColor(), borderWidth, Qt::SolidLine));
        switch (editor->getCurrentTool()) {
        case Editor::Pen:
            painter.drawPath(drawingPath);
            break;
        case Editor::Rectangle:
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(currentShapeRect);
            break;
        case Editor::Ellipse:
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(currentShapeRect);
            break;
        case Editor::Line:
            painter.drawLine(origin, drawingEnd);
            break;
        case Editor::Arrow:
            drawArrow(painter, origin, drawingEnd);
            break;
        case Editor::Text:
            painter.setFont(currentFont);
            painter.drawText(textBoundingRect, Qt::AlignLeft, text);
            break;
        default:
            break;
        }
    }

    if (editor->getCurrentTool() != Editor::None) {
        drawBorderCircle(painter, mapFromGlobal(QCursor::pos()));
        painter.setBrush(QBrush(Qt::transparent));
        painter.drawEllipse(cursorPosition, borderWidth / 2, borderWidth / 2);
    }
}

void ScreenshotDisplay::onSaveRequested() {
    QJsonObject config = configManager->loadConfig();
    QString defaultSaveFolder = config["default_save_folder"].toString();
    QString fileExtension = config["file_extension"].toString();
    QString defaultFileName = getUniqueFilePath(defaultSaveFolder, "screenshot", fileExtension);

    QString fileFilter = "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;";
    if (fileExtension == "png") {
        fileFilter = "PNG Files (*.png);;";
    }
    else if (fileExtension == "jpg" || fileExtension == "jpeg") {
        fileFilter = "JPEG Files (*.jpg *.jpeg);;";
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Save As", defaultFileName, fileFilter);

    if (!filePath.isEmpty()) {
        originalPixmap.save(filePath);
        close();
    }
}

void ScreenshotDisplay::onPublishRequested() {
    // Implémentez votre logique de publication en ligne ici
}

void ScreenshotDisplay::onCloseRequested() {
    close();
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
        QPoint tooltipPosition = selectionRect.topRight() + QPoint(10, -20);
        QToolTip::showText(mapToGlobal(tooltipPosition), tooltipText, this);
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

void ScreenshotDisplay::updateEditorPosition() {
    if (selectionRect.isValid()) {
        const int margin = 10;
        editor->move(selectionRect.topRight() + QPoint(margin, margin));
    }
}

void ScreenshotDisplay::drawArrow(QPainter& painter, const QPoint& start, const QPoint& end) {
    painter.drawLine(start, end);

    double angle = std::atan2(end.y() - start.y(), end.x() - start.x());

    const double arrowHeadLength = 20.0;
    const double arrowHeadAngle = M_PI / 6;

    QPoint arrowP1 = end + QPoint(std::cos(angle + arrowHeadAngle) * arrowHeadLength,
        std::sin(angle + arrowHeadAngle) * arrowHeadLength);
    QPoint arrowP2 = end + QPoint(std::cos(angle - arrowHeadAngle) * arrowHeadLength,
        std::sin(angle - arrowHeadAngle) * arrowHeadLength);

    QPolygon arrowHead;
    arrowHead << end << arrowP1 << arrowP2;

    painter.drawPolygon(arrowHead);
}

void ScreenshotDisplay::drawBorderCircle(QPainter& painter, const QPoint& position) {
    painter.setPen(QPen(editor->getCurrentColor(), 2, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(position, borderWidth, borderWidth);
}
