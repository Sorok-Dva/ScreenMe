#include "include/utils.h"
#include "include/screenshotdisplay.h"
#include <QDir>
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QDebug>

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
    displayWidget->setGeometry(QGuiApplication::primaryScreen()->geometry());
    displayWidget->show();
}