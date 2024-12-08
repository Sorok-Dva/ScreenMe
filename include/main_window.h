#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QNetworkReply>
#include <QJsonObject>
#include <QPointer>
#include "screenshotdisplay.h"
#include "config_manager.h"
#include "UGlobalHotkeys.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ConfigManager* configManager, QWidget* parent = nullptr);

public slots:
    void takeScreenshot();
    void takeFullscreenScreenshot();
    void handleHotkeyActivated(size_t id);
    void handleScreenshotClosed();
    void reloadHotkeys();
    void onUpdateCheckFinished(QNetworkReply* reply, bool fromAction);
    void downloadUpdate(const QString& downloadUrl);
    void onDownloadFinished(QNetworkReply* reply);

public:
    void checkForUpdates(bool fromAction);

signals:
    void screenshotClosed();

private:
    QPointer<ScreenshotDisplay> screenshotDisplay;
    ConfigManager* configManager;
    UGlobalHotkeys* hotkeyManager;
    bool isScreenshotDisplayed;
};
