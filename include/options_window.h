#ifndef OPTIONS_WINDOW_H
#define OPTIONS_WINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QKeyEvent>
#include <QSet>
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
    void loadOptions();
    void saveOptions();
    void browseFolder();
    void startRecordingHotkey();
    void startRecordingFullscreenHotkey();
    void handleGlobalKeyPress(QKeySequence keySequence);

private:
    void recordKey(QKeyEvent* event);

    ConfigManager* configManager;
    QLineEdit* hotkeyEdit;
    QLineEdit* fullscreenHotkeyEdit;
    QLineEdit* hotkeyEditing;
    QComboBox* extensionCombo;
    QSpinBox* qualitySpinbox;
    QLineEdit* folderEdit;
    QCheckBox* startWithSystemCheckbox;
    QString currentKeys;
    QSet<int> pressedKeys;
};

#endif // OPTIONS_WINDOW_H
