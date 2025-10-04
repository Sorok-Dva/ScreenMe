#pragma once

#include <QPixmap>
#include <QRect>
#include <QString>

struct DesktopCapture {
    QPixmap pixmap;
    QRect geometry;

    bool isValid() const { return !pixmap.isNull() && geometry.isValid(); }
};

QString getUniqueFilePath(const QString& folder, const QString& baseName, const QString& extension);
DesktopCapture captureEntireDesktop();
void CaptureScreenshot(const QString& savePath);
void displayScreenshotOnScreen(const QPixmap& pixmap, const QRect& geometry = QRect());
QString getConfigFilePath(const QString& file);

void saveLoginInfo(const QString& id, const QString& email, const QString& nickname, const QString& token);
QString loadLoginInfo();
void clearLoginInfo();
void setAutoStart(bool enable);


//const QString SCREEN_ME_HOST = "http://127.0.0.1:3001";
const QString SCREEN_ME_HOST = "https://screen.sorokdva.eu";
const QString VERSION = "1.3.01";
