#pragma once

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QString>
#include <QJsonObject>

class ConfigManager {
public:
    ConfigManager(const QString& configPath);
    QJsonObject loadConfig();
    void saveConfig(const QJsonObject& config);

private:
    QString configPath;
};

#endif // CONFIG_MANAGER_H
