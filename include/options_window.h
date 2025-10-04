#ifndef OPTIONS_WINDOW_H
#define OPTIONS_WINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QKeyEvent>
#include <QKeySequence>
#include "config_manager.h"

class OptionsWindow : public QDialog {
    Q_OBJECT

public:
    explicit OptionsWindow(ConfigManager* configManager, QWidget* parent = nullptr);

signals:
    void reloadHotkeys();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void loadOptions();
    void saveOptions();
    void browseFolder();
    void startRecordingHotkey();
    void startRecordingFullscreenHotkey();
    void handleGlobalKeyPress(QKeySequence keySequence);

private:
    ConfigManager* configManager;
    QLineEdit* hotkeyEdit;
    QLineEdit* fullscreenHotkeyEdit;
    QLineEdit* hotkeyEditing;
    QComboBox* extensionCombo;
    QSpinBox* qualitySpinbox;
    QLineEdit* folderEdit;
    QCheckBox* startWithSystemCheckbox;
    QComboBox* languageCombo;
};

#endif // OPTIONS_WINDOW_H
