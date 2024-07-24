#include "include/config_manager.h"
#include "include/utils.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

ConfigManager::ConfigManager(const QString& configPath) : configPath(configPath) {
    QString filePath = getConfigFilePath("config.json");
    QFile configFile(filePath);
    if (!configFile.exists()) {
        QJsonObject defaultConfig;
        defaultConfig["screenshot_hotkey"] = "Print";
        defaultConfig["fullscreen_hotkey"] = "Ctrl+Shift+Print";
        defaultConfig["file_extension"] = "png";
        defaultConfig["image_quality"] = 90;
        defaultConfig["default_save_folder"] = QDir::homePath() + "/Pictures/ScreenMe";
        defaultConfig["start_with_system"] = true;
        saveConfig(defaultConfig);
    }
}

QJsonObject ConfigManager::loadConfig() {
    QString filePath = getConfigFilePath("config.json");
    QFile configFile(filePath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QJsonObject();
    }
    QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
    configFile.close();
    return doc.object();
}

void ConfigManager::saveConfig(const QJsonObject& config) {
    QString filePath = getConfigFilePath("config.json");
    QFile configFile(filePath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QJsonDocument doc(config);
    configFile.write(doc.toJson());
    configFile.close();
}
