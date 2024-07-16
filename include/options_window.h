#ifndef OPTIONS_WINDOW_H
#define OPTIONS_WINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include "config_manager.h"

class OptionsWindow : public QDialog {
    Q_OBJECT

public:
    explicit OptionsWindow(ConfigManager* configManager, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void saveOptions();
    void browseFolder();
    void startRecordingHotkey();
    void startRecordingFullscreenHotkey();

private:
    ConfigManager* configManager;
    QLineEdit* hotkeyEdit;
    QLineEdit* fullscreenHotkeyEdit;
    QComboBox* extensionCombo;
    QSpinBox* qualitySpinbox;
    QLineEdit* folderEdit;
    QCheckBox* startWithSystemCheckbox;

    QLineEdit* hotkeyEditing;
    QString currentKeys;
    QSet<int> pressedKeys;

    void loadOptions();
    void recordKey(QKeyEvent* event);
};

#endif // OPTIONS_WINDOW_H
