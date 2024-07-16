#pragma once

#include <QWidget>
#include "config_manager.h"
#include "UGlobalHotkeys.h"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(ConfigManager* configManager, QWidget* parent = nullptr);

public slots:
    void takeScreenshot();
    void takeFullscreenScreenshot();
    void handleHotkeyActivated(size_t id);
    void handleScreenshotClosed();

signals:
    void screenshotClosed();

private:
    ConfigManager* configManager;
    UGlobalHotkeys* hotkeyManager;
    bool isScreenshotDisplayed;
};
