#include "../include/utils.h"
#include "../include/screenshotdisplay.h"
#include <QDir>
#include <QScreen>
#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <QGuiApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStandardPaths>
#include <QSettings>

QString getUniqueFilePath(const QString& folder, const QString& baseName, const QString& extension) {
    QDir dir(folder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    int i = 1;
    QString filePath;
    do {
        filePath = QString("%1/%2-%3.%4").arg(folder).arg(baseName).arg(i).arg(extension);
        i++;
    } while (QFile::exists(filePath));

    return filePath;
}

QString getConfigFilePath(const QString& file) {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath(file);
}

DesktopCapture captureEntireDesktop() {
    DesktopCapture capture;
    const QList<QScreen*> screens = QGuiApplication::screens();
    if (screens.isEmpty()) {
        qWarning() << "No screens detected";
        return capture;
    }

    QRect totalGeometry = screens.first()->geometry();
    for (int i = 1; i < screens.size(); ++i) {
        totalGeometry = totalGeometry.united(screens.at(i)->geometry());
    }

    if (!totalGeometry.isValid()) {
        qWarning() << "Combined screen geometry is invalid";
        return capture;
    }

    QPixmap desktopPixmap(totalGeometry.size());
    desktopPixmap.fill(Qt::transparent);

    QPainter painter(&desktopPixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    for (QScreen* screen : screens) {
        const QRect screenGeometry = screen->geometry();
        const QPoint offset = screenGeometry.topLeft() - totalGeometry.topLeft();
        QPixmap screenPixmap = screen->grabWindow(0);
        const qreal dpr = screenPixmap.devicePixelRatio();
        const QSizeF logicalSize = QSizeF(screenPixmap.size()) / dpr;
        const QRectF targetRect(QPointF(offset), logicalSize);
        painter.drawPixmap(targetRect, screenPixmap, QRectF(QPointF(0, 0), QSizeF(screenPixmap.size())));
    }

    painter.end();
    desktopPixmap.setDevicePixelRatio(1.0);

    capture.pixmap = desktopPixmap;
    capture.geometry = totalGeometry;
    return capture;
}

void CaptureScreenshot(const QString& savePath) {
    const DesktopCapture capture = captureEntireDesktop();
    if (!capture.isValid()) {
        qWarning() << "Unable to capture desktop";
        return;
    }
    capture.pixmap.save(savePath);
}

void displayScreenshotOnScreen(const QPixmap& pixmap, const QRect& geometry) {
    ScreenshotDisplay* displayWidget = new ScreenshotDisplay(pixmap, geometry);
    if (geometry.isValid()) {
        displayWidget->setGeometry(geometry);
    } else {
        displayWidget->resize(pixmap.size());
    }
    displayWidget->show();
}

void saveLoginInfo(const QString& id, const QString& email, const QString& nickname, const QString& token) {
    QString filePath = getConfigFilePath("login_info.json");
    QFile loginFile(filePath);
    if (loginFile.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObj;
        jsonObj["id"] = id;
        jsonObj["email"] = email;
        jsonObj["nickname"] = nickname;
        jsonObj["token"] = token;

        QJsonDocument jsonDoc(jsonObj);
        loginFile.write(jsonDoc.toJson());
    }
}

QString loadLoginInfo() {
    QString filePath = getConfigFilePath("login_info.json");
    QFile loginFile(filePath);
    if (loginFile.open(QIODevice::ReadOnly)) {
        QByteArray data = loginFile.readAll();
        return QString(data);
    }
    return QString();
}

void clearLoginInfo() {
    QString filePath = getConfigFilePath("login_info.json");
    QFile loginFile(filePath);
    loginFile.remove();
}


void setAutoStart(bool enable) {
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    if (enable) {
        const QString applicationName = QApplication::applicationName();
        const QString applicationPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        settings.setValue(applicationName, applicationPath);
    }
    else {
        settings.remove(QApplication::applicationName());
    }
#else
    Q_UNUSED(enable);
#endif
}
