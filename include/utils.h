#pragma once

#include <QString>
#include <QPixmap>

QString getUniqueFilePath(const QString& folder, const QString& baseName, const QString& extension);
void CaptureScreenshot(const QString& savePath);
void displayScreenshotOnScreen(const QPixmap& pixmap);