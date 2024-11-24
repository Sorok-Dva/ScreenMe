#pragma once

#include <QString>
#include <QPixmap>

QString getUniqueFilePath(const QString& folder, const QString& baseName, const QString& extension);
void CaptureScreenshot(const QString& savePath);
void displayScreenshotOnScreen(const QPixmap& pixmap);
QString getConfigFilePath(const QString& file);

void saveLoginInfo(const QString& id, const QString& email, const QString& nickname, const QString& token);
QString loadLoginInfo();
void clearLoginInfo();
void setAutoStart(bool enable);


//const QString SCREEN_ME_HOST = "http://127.0.0.1:3001";
const QString SCREEN_ME_HOST = "https://screen-me.cloud";
const QString VERSION = "1.2.2";