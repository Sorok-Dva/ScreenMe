#include "include/utils.h"
#include "include/screenshotdisplay.h"
#include <QDir>
#include <QScreen>
#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

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

void CaptureScreenshot(const QString& savePath) {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) {
        qDebug() << "No primary screen found";
        return;
    }
    QPixmap originalPixmap = screen->grabWindow(0);
    originalPixmap.save(savePath);
}

void displayScreenshotOnScreen(const QPixmap& pixmap) {
    ScreenshotDisplay* displayWidget = new ScreenshotDisplay(pixmap);
    displayWidget->setGeometry(QApplication::primaryScreen()->geometry());
    displayWidget->show();
}

void saveLoginInfo(const QString& id, const QString& email, const QString& nickname, const QString& token) {
    QFile file("resources/login_info.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObj;
        jsonObj["id"] = id;
        jsonObj["email"] = email;
        jsonObj["nickname"] = nickname;
        jsonObj["token"] = token;

        QJsonDocument jsonDoc(jsonObj);
        file.write(jsonDoc.toJson());
    }
}

QString loadLoginInfo() {
    QFile file("resources/login_info.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        return QString(data);
    }
    return QString();
}

void clearLoginInfo() {
    QFile file("resources/login_info.json");
    file.remove();
}