#include "../include/screenshotdisplay.h"
#include "../include/config_manager.h"
#include "../include/utils.h"
#include <QApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpMultiPart>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
#include <QShortcut>
#include <QToolTip>
#include <QCursor>
#include <QCheckBox>
#include <QWheelEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <cmath>
#include <algorithm>

ScreenshotDisplay::ScreenshotDisplay(const QPixmap& pixmap, const QRect& geometry, QWidget* parent, ConfigManager* configManager)
    : QWidget(parent),
    originalPixmap(pixmap),
    drawingPixmap(pixmap.size()),
    selectionRect(),
    currentShapeRect(),
    currentHandle(None),
    selectionStarted(false),
    movingSelection(false),
    drawing(false),
    shapeDrawing(false),
    showBorderCircle(false),
    borderWidth(5),
    pixmapDeviceRatio(qFuzzyIsNull(pixmap.devicePixelRatio()) ? 1.0 : pixmap.devicePixelRatio()),
    currentColor(Qt::black),
    currentTool(Editor::None),
    currentFont("Arial", 16),
    text("Editable Text"),
    textEdit(nullptr),
    editor(nullptr),
    configManager(configManager) {

    originalPixmap.setDevicePixelRatio(1.0);
    drawingPixmap.setDevicePixelRatio(1.0);
    pixmapDeviceRatio = 1.0;

    desktopGeometry = geometry.isValid() ? geometry : QRect(QPoint(0, 0), originalPixmap.size());

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowTitle("ScreenMe");
    setWindowIcon(QIcon("resources/icon.png"));
    setAttribute(Qt::WA_QuitOnClose, false);
    setGeometry(desktopGeometry);

    drawingPixmap.setDevicePixelRatio(pixmapDeviceRatio);
    drawingPixmap.fill(Qt::transparent);

    initializeEditor();
    configureShortcuts();

    QFontMetrics fm(currentFont);
    textBoundingRect = QRect(QPoint(100, 100), fm.size(0, text));
    show();
}

void ScreenshotDisplay::initializeEditor() {
    editor.reset(new Editor(this));
    connect(editor.get(), &Editor::toolChanged, this, &ScreenshotDisplay::onToolSelected);
    connect(editor.get(), &Editor::colorChanged, this, [this](const QColor& color) {
        currentColor = color;
        if (textEdit) {
            textEdit->setTextColor(currentColor);
        }
        update();
    });
    connect(editor.get(), &Editor::saveRequested, this, &ScreenshotDisplay::onSaveRequested);
    connect(editor.get(), &Editor::copyRequested, this, &ScreenshotDisplay::copySelectionToClipboard);
    connect(editor.get(), &Editor::publishRequested, this, [this]() {
        onPublishRequested(false);
    });
    connect(editor.get(), &Editor::printRequested, this, &ScreenshotDisplay::onPrintRequested);
    connect(editor.get(), &Editor::searchRequested, this, [this]() {
        onPublishRequested(true);
        });
    connect(editor.get(), &Editor::closeRequested, this, &ScreenshotDisplay::onCloseRequested);
}

void ScreenshotDisplay::configureShortcuts() {
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

    QShortcut* undoShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    connect(undoShortcut, &QShortcut::activated, this, &ScreenshotDisplay::undo);

    QShortcut* copyShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    connect(copyShortcut, &QShortcut::activated, this, &ScreenshotDisplay::copySelectionToClipboard);
}

void ScreenshotDisplay::closeEvent(QCloseEvent* event) {
    emit screenshotClosed();
    if (editor) {
        editor->hide();
    }
    if (textEdit) {
        textEdit->deleteLater();
        textEdit = nullptr;
    }

    while (!undoStack.empty()) {
        undoStack.pop();
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
    else if (editor->getCurrentTool() == Editor::Text) {
        if (!textEdit) {
            textEdit = new CustomTextEdit(this);
            textEdit->setFont(currentFont);
            textEdit->setTextColor(currentColor);
            textEdit->setStyleSheet("background: transparent;");
            textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            textEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            textEdit->move(event->pos());
            textEdit->show();
            textEdit->setFocus();
            textEditPosition = event->pos();
            connect(textEdit, &CustomTextEdit::focusOut, this, &ScreenshotDisplay::finalizeTextEdit);
            connect(textEdit, &QTextEdit::textChanged, this, &ScreenshotDisplay::adjustTextEditSize);
        }
        else {
            finalizeTextEdit();
        }
    }
    else {
        saveStateForUndo();
        drawing = true;
        lastPoint = event->pos();
        origin = event->pos();
        if (editor->getCurrentTool() != Editor::Pen) {
            shapeDrawing = true;
            currentShapeRect = QRect(lastPoint, QSize());
        }
    }
}

void ScreenshotDisplay::mouseMoveEvent(QMouseEvent* event) {
    if (selectionRect.isValid() && editor->isHidden()) {
        updateEditorPosition();
        editor->show();
    }
    if (selectionRect.isValid()) {
        update();
    }
    if (selectionStarted) {
        QRect newRect = QRect(origin, event->pos()).normalized();
        QRect bounds(QPoint(0, 0), size());
        selectionRect = newRect.intersected(bounds);
        update();
        updateTooltip();
        updateEditorPosition();
    }
    else if (drawing && editor->getCurrentTool() == Editor::Pen) {
        QPixmap tempPixmap = originalPixmap.copy();
        QPainter painter(&tempPixmap);
        painter.setPen(QPen(editor->getCurrentColor(), borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(lastPoint, event->pos());
        lastPoint = event->pos();
        originalPixmap = tempPixmap;
        update();
    }
    else if (shapeDrawing) {
        currentShapeRect = QRect(lastPoint, event->pos()).normalized();
        drawingEnd = event->pos();
        update();
    }
    else if (movingSelection) {
        QPoint topLeft = event->pos() - selectionOffset;
        QRect bounds(QPoint(0, 0), size());
        const int maxX = std::max(bounds.left(), bounds.width() - selectionRect.width());
        const int maxY = std::max(bounds.top(), bounds.height() - selectionRect.height());
        topLeft.setX(std::clamp(topLeft.x(), bounds.left(), maxX));
        topLeft.setY(std::clamp(topLeft.y(), bounds.top(), maxY));
        selectionRect.moveTopLeft(topLeft);
        update();
        updateTooltip();
        updateEditorPosition();
    }
    else if (currentHandle != None) {
        resizeSelection(event->pos());
        update();
        updateTooltip();
        updateEditorPosition();
    }

    HandlePosition handle = handleAtPoint(event->pos());
    setCursor(cursorForHandle(handle));
}

void ScreenshotDisplay::mouseReleaseEvent(QMouseEvent* event) {
    selectionStarted = false;
    movingSelection = false;
    currentHandle = None;
    drawing = false;
    endPoint = QPoint(std::lround(event->pos().x() * pixmapDeviceRatio),
                         std::lround(event->pos().y() * pixmapDeviceRatio));

    if (shapeDrawing) {
        saveStateForUndo();
        QPixmap tempPixmap = originalPixmap.copy();
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
            painter.drawLine(lastPoint, drawingEnd);
            break;
        case Editor::Arrow:
            drawArrow(painter, lastPoint, drawingEnd);
            break;
        default:
            break;
        }

        originalPixmap = tempPixmap;
        shapeDrawing = false;
        update();
    }

    update();
    updateTooltip();
}

void ScreenshotDisplay::keyPressEvent(QKeyEvent* event) {
    if (editor->getCurrentTool() != Editor::None && event->key() == Qt::Key_Escape) {
        if (editor->getCurrentTool() == Editor::Text && textEdit) {
            finalizeTextEdit();
        }
        else {
            editor->deselectTools();
            setCursor(Qt::ArrowCursor);
        }
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
        borderWidth = std::clamp(borderWidth, 1, 20);
        update();
    }
    if (editor->getCurrentTool() == Editor::Text && textEdit) {
        int delta = event->angleDelta().y() / 120;
        int newSize = currentFont.pointSize() + delta;
        if (newSize > 0) {
            currentFont.setPointSize(newSize);
            textEdit->setFont(currentFont);
            adjustTextEditSize();
            update();
        }
    }
}

void ScreenshotDisplay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QSize targetSize(originalPixmap.width() / pixmapDeviceRatio, originalPixmap.height() / pixmapDeviceRatio);
    const QRect targetRect(QPoint(0, 0), targetSize);

    painter.drawPixmap(targetRect, originalPixmap);
    painter.drawPixmap(targetRect, drawingPixmap);

    QPainterPath shadePath;
    shadePath.addRect(rect());
    if (selectionRect.isValid()) {
        QPainterPath selectionPath;
        selectionPath.addRect(selectionRect);
        shadePath = shadePath.subtracted(selectionPath);
    }

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.fillPath(shadePath, QColor(0, 0, 0, 140));
    painter.restore();

    if (selectionRect.isValid()) {
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
            drawArrow(painter, lastPoint, drawingEnd);
            break;
        case Editor::Text:
            painter.setFont(currentFont);
            painter.setPen(QPen(editor->getCurrentColor()));
            painter.drawText(textBoundingRect, Qt::AlignLeft, text);
            break;
        default:
            break;
        }
    }

    if (editor->getCurrentTool() != Editor::None) {
        cursorPosition = mapFromGlobal(QCursor::pos());
        drawBorderCircle(painter, cursorPosition);
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
    QRect captureRect = selectionRect.isValid() ? toPixmapRect(selectionRect) : originalPixmap.rect();

    if (!filePath.isEmpty()) {
        QPixmap selectedPixmap = originalPixmap.copy(captureRect);
        selectedPixmap.save(filePath);
        close();
    }
}

void ScreenshotDisplay::onPublishRequested(bool searchImage) {
    if (textEdit) {
        finalizeTextEdit();
    }
    QPixmap resultPixmap = originalPixmap;
    QPainter painter(&resultPixmap);
    painter.drawPixmap(0, 0, drawingPixmap);

    editor->hide();

    QRect captureRect = selectionRect.isValid() ? toPixmapRect(selectionRect) : originalPixmap.rect();

    QJsonObject config = configManager->loadConfig();
    QString defaultSaveFolder = config["default_save_folder"].toString();
    QString fileExtension = config["file_extension"].toString();

    if (selectionRect.isValid()) {
        ScreenshotDisplay::hide();
        QPixmap selectedPixmap = resultPixmap.copy(captureRect);
        QApplication::clipboard()->setPixmap(selectedPixmap);

        QString tempFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/screenshot.png";
        selectedPixmap.save(tempFilePath);

        QString savePath = getUniqueFilePath(defaultSaveFolder, "screenshot", fileExtension);
        selectedPixmap.save(savePath);
        qDebug() << "Saving screenshot to:" << savePath;
        QString jsonStr = loadLoginInfo();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
        QJsonObject loginInfo = jsonDoc.object();

        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QUrl url(SCREEN_ME_HOST + "/api/screenshot");
        QNetworkRequest request(url);

        request.setRawHeader("Authorization", "Bearer " + loginInfo["token"].toString().toUtf8());

        QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart imagePart;
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"screenshot\"; filename=\"screenshot.png\""));

        QFile* file = new QFile(tempFilePath);
        if (!file->open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "Error", "Failed to open the image file for upload.");
            return;
        }
        imagePart.setBodyDevice(file);
        file->setParent(multiPart);

        multiPart->append(imagePart);

        QProgressDialog* progressDialog = new QProgressDialog("Publishing screenshot", "Cancel", 0, 100, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setAutoClose(false);
        progressDialog->setAutoReset(false);
        progressDialog->show();

        // Position the progress dialog at the bottom right of the screen
        QRect screenGeometry = desktopGeometry;
        QSize progressDialogSize = progressDialog->sizeHint();
        progressDialog->move(screenGeometry.bottomRight() - QPoint(progressDialogSize.width() + 10, progressDialogSize.height() + 100));

        QNetworkReply* reply = manager->post(request, multiPart);
        multiPart->setParent(reply);

        connect(progressDialog, &QProgressDialog::canceled, reply, &QNetworkReply::abort);

        connect(reply, &QNetworkReply::uploadProgress, this, [progressDialog](qint64 bytesSent, qint64 bytesTotal) {
            if (bytesTotal > 0) {
                progressDialog->setMaximum(bytesTotal);
                progressDialog->setValue(bytesSent);
            }
        });

        connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this, [reply]() {
            qDebug() << "Network Error:" << reply->errorString();
        });

        connect(reply, &QNetworkReply::finished, this, [reply, file, tempFilePath, this, progressDialog, searchImage, screenGeometry, loginInfo]() {
            progressDialog->close();

            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
                QJsonObject jsonObject = jsonResponse.object();
                QString url = jsonObject["url"].toString();
                QString id = QString::number(jsonObject["id"].toInt());
                QString link = SCREEN_ME_HOST + "/" + url;
                QString imageUrl = jsonObject["imageUrl"].toString();

                if (!searchImage) {
                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle("Screenshot Uploaded");
                    msgBox.setText("Screenshot uploaded successfully ! Link: " + link);
                    QPushButton* copyButton = msgBox.addButton(tr("Copy"), QMessageBox::ActionRole);
                    QPushButton* openButton = msgBox.addButton(tr("Open"), QMessageBox::ActionRole);
                    msgBox.addButton(QMessageBox::Ok);

                    QCheckBox* privateCheckBox = nullptr;
                    if (!loginInfo["token"].toString().isEmpty()) {
                        privateCheckBox = new QCheckBox("Private", &msgBox);
                        msgBox.setCheckBox(privateCheckBox);

                        connect(privateCheckBox, &QCheckBox::toggled, this, [id, loginInfo](bool checked) {
                            QNetworkAccessManager* manager = new QNetworkAccessManager();
                            QUrl url(SCREEN_ME_HOST + "/api/screenshot/" + id);
                            QNetworkRequest request(url);

                            request.setRawHeader("Authorization", "Bearer " + loginInfo["token"].toString().toUtf8());
                            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                            QJsonObject json;
                            json["privacy"] = checked ? "private" : "public";
                            QJsonDocument doc(json);
                            QByteArray data = doc.toJson();

                            QNetworkReply* reply = manager->sendCustomRequest(request, "PATCH", data);
                            connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
                            });
                    }

                    connect(copyButton, &QPushButton::clicked, [link]() {
                        QClipboard* clipboard = QGuiApplication::clipboard();
                        clipboard->setText(link);
                        });

                    connect(openButton, &QPushButton::clicked, [link]() {
                        QDesktopServices::openUrl(QUrl(link));
                        });

                    // Position the message box at the bottom right of the screen
                    msgBox.show();
                    QSize msgBoxSize = msgBox.sizeHint();
                    msgBox.move(screenGeometry.bottomRight() - QPoint(msgBoxSize.width() + 10, msgBoxSize.height() + 100));
                    msgBox.exec();
                }
                else {
                    QDesktopServices::openUrl(QUrl("https://tineye.com/search?url=" + imageUrl));
                }
                
            }
            else {
                QString errorString = reply->errorString();
                qDebug() << "Upload Failed:" << errorString;
                QString serverReply = errorString.section("server replied: ", 1, 1);
                if (serverReply.contains("Forbidden")) {
                    QMessageBox::critical(this, "Upload Failed", "Failed to upload screenshot: " + serverReply + "\nPlease try to log in again.");
                }
                else {
                    QMessageBox::critical(this, "Upload Failed", "Failed to upload screenshot: " + serverReply);
                }
            }
            reply->deleteLater();
            file->deleteLater();
            QFile::remove(tempFilePath);
            delete progressDialog;

            emit screenshotClosed();
        });
    }
}


void ScreenshotDisplay::onCloseRequested() {
    close();
}

void ScreenshotDisplay::onPrintRequested() {
    if (textEdit) {
        finalizeTextEdit();
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setDocName(tr("ScreenMe Capture"));

    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Screenshot"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QPixmap resultPixmap = originalPixmap;
    QPainter painter(&resultPixmap);
    painter.drawPixmap(0, 0, drawingPixmap);

    const QRect captureRect = selectionRect.isValid() ? toPixmapRect(selectionRect) : resultPixmap.rect();
    QPixmap printPixmap = resultPixmap.copy(captureRect);

    if (printPixmap.isNull()) {
        return;
    }

    QPainter printPainter(&printer);
    printPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
    QPixmap scaled = printPixmap.scaled(pageRect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const qreal offsetX = (pageRect.width() - scaled.width()) / 2.0;
    const qreal offsetY = (pageRect.height() - scaled.height()) / 2.0;
    printPainter.drawPixmap(QPointF(pageRect.left() + offsetX, pageRect.top() + offsetY), scaled);
    printPainter.end();
}

void ScreenshotDisplay::copySelectionToClipboard() {
    QPixmap resultPixmap = originalPixmap;
    QPainter painter(&resultPixmap);
    painter.drawPixmap(0, 0, drawingPixmap);

    if (selectionRect.isValid()) {
        const QRect captureRect = toPixmapRect(selectionRect);
        QApplication::clipboard()->setPixmap(resultPixmap.copy(captureRect));
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
    QRect bounds(QPoint(0, 0), size());
    QRect newRect = selectionRect;

    switch (currentHandle) {
    case TopLeft:
        newRect.setTopLeft(point);
        break;
    case TopRight:
        newRect.setTopRight(point);
        break;
    case BottomLeft:
        newRect.setBottomLeft(point);
        break;
    case BottomRight:
        newRect.setBottomRight(point);
        break;
    case Top:
        newRect.setTop(point.y());
        break;
    case Bottom:
        newRect.setBottom(point.y());
        break;
    case Left:
        newRect.setLeft(point.x());
        break;
    case Right:
        newRect.setRight(point.x());
        break;
    default:
        break;
    }

    newRect = newRect.normalized().intersected(bounds);

    selectionRect = newRect;
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
    setCursor(tool == Editor::None ? Qt::ArrowCursor : Qt::CrossCursor);
}

void ScreenshotDisplay::updateEditorPosition() {
    if (selectionRect.isValid()) {
        const int margin = 10;
        QSize editorSize = editor->sizeHint();

        const QPoint globalTopRight = mapToGlobal(selectionRect.topRight());
        const QPoint globalTopLeft = mapToGlobal(selectionRect.topLeft());
        const QPoint globalCenter = mapToGlobal(selectionRect.center());
        const QRect globalBounds = geometry();

        int desiredX = globalTopRight.x() + margin;
        if (desiredX + editorSize.width() > globalBounds.right() - margin) {
            desiredX = globalTopLeft.x() - editorSize.width() - margin;
        }

        int desiredY = globalCenter.y() - (editorSize.height() / 2);

        const int minX = globalBounds.left() + margin;
        const int maxX = std::max(minX, globalBounds.right() - editorSize.width() - margin);
        const int minY = globalBounds.top() + margin;
        const int maxY = std::max(minY, globalBounds.bottom() - editorSize.height() - margin);

        desiredX = std::clamp(desiredX, minX, maxX);
        desiredY = std::clamp(desiredY, minY, maxY);

        editor->move(desiredX, desiredY);
    }
}

void ScreenshotDisplay::drawArrow(QPainter& painter, const QPoint& start, const QPoint& end) {
    painter.drawLine(start, end);

    double angle = std::atan2(start.y() - end.y(), start.x() - end.x());

    const double arrowHeadLength = borderWidth * 5;
    const double arrowHeadAngle = M_PI / 15;

    QPoint arrowP1 = end + QPoint(std::cos(angle + arrowHeadAngle) * arrowHeadLength,
        std::sin(angle + arrowHeadAngle) * arrowHeadLength);
    QPoint arrowP2 = end + QPoint(std::cos(angle - arrowHeadAngle) * arrowHeadLength,
        std::sin(angle - arrowHeadAngle) * arrowHeadLength);

    QPolygon arrowHead;
    arrowHead << end << arrowP1 << arrowP2;

    painter.setBrush(QBrush(editor->getCurrentColor()));
    painter.drawPolygon(arrowHead);
}

void ScreenshotDisplay::drawBorderCircle(QPainter& painter, const QPoint& position) {
    painter.setPen(QPen(editor->getCurrentColor(), 2, Qt::SolidLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(position, borderWidth, borderWidth);
}

QRect ScreenshotDisplay::toPixmapRect(const QRect& rect) const {
    if (!rect.isValid()) {
        return QRect();
    }

    const qreal scale = pixmapDeviceRatio <= 0.0 ? 1.0 : pixmapDeviceRatio;
    const QPointF topLeft = rect.topLeft();
    const QSizeF size = rect.size();
    const QPoint scaledTopLeft(std::lround(topLeft.x() * scale), std::lround(topLeft.y() * scale));
    const QSize scaledSize(std::lround(size.width() * scale), std::lround(size.height() * scale));
    return QRect(scaledTopLeft, scaledSize);
}

void ScreenshotDisplay::adjustTextEditSize() {
    QFontMetrics fm(textEdit->font());
    int width = fm.horizontalAdvance(textEdit->toPlainText().replace('\n', ' ')) + 10;
    textEdit->setFixedSize(width, textEdit->height());
}

void ScreenshotDisplay::finalizeTextEdit() {
    if (textEdit) {
        saveStateForUndo();
        QPainter painter(&originalPixmap);
        painter.setFont(textEdit->font());
        painter.setPen(QPen(editor->getCurrentColor()));

        QFontMetrics fm(textEdit->font());
        QStringList lines = textEdit->toPlainText().split('\n');
        QPoint currentPos = textEditPosition;

        QMargins contentMargins = textEdit->contentsMargins();
        int leftMargin = contentMargins.left();
        int topMargin = contentMargins.top();

        currentPos.setY(currentPos.y() + fm.height() + topMargin);
        currentPos.setX(currentPos.x() + fm.descent() + leftMargin);

        for (const QString& line : lines) {
            painter.drawText(currentPos, line);
            currentPos.setY(currentPos.y() + fm.height());
        }

        textEdit->deleteLater();
        textEdit = nullptr;
        update();
    }
}

void ScreenshotDisplay::saveStateForUndo() {
    undoStack.push(originalPixmap);
}

void ScreenshotDisplay::undo() {
    if (!undoStack.empty()) {
        originalPixmap = undoStack.top();
        undoStack.pop();
        update();
    }
}
