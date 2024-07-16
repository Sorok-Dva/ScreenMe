#include "include/config_manager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

ConfigManager::ConfigManager(const QString& configPath) : configPath(configPath) {
    QFile file(configPath);
    if (!file.exists()) {
        QJsonObject defaultConfig;
        defaultConfig["screenshot_hotkey"] = "Ctrl+Shift+S";
        defaultConfig["fullscreen_hotkey"] = "Ctrl+Shift+F";
        defaultConfig["file_extension"] = "png";
        defaultConfig["image_quality"] = 90;
        defaultConfig["default_save_folder"] = QDir::homePath();
        defaultConfig["start_with_system"] = false;
        saveConfig(defaultConfig);
    }
}

QJsonObject ConfigManager::loadConfig() {
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QJsonObject();
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    return doc.object();
}

void ConfigManager::saveConfig(const QJsonObject& config) {
    QFile file(configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QJsonDocument doc(config);
    file.write(doc.toJson());
    file.close();
}
