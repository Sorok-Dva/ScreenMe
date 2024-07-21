#pragma once

#include <QString>
#include <QPixmap>

QString getUniqueFilePath(const QString& folder, const QString& baseName, const QString& extension);
void CaptureScreenshot(const QString& savePath);
void displayScreenshotOnScreen(const QPixmap& pixmap);

void saveLoginInfo(const QString& id, const QString& email, const QString& nickname, const QString& token);
QString loadLoginInfo();
void clearLoginInfo();

const QString SCREEN_ME_HOST = "http://127.0.0.1:3001";
//const QString SCREEN_ME_HOST = "https://screen-me.cloud";