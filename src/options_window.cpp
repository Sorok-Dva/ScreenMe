#include "include/options_window.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QKeyEvent>
#include <QJsonObject>
#include <QSet>
#include <QKeySequence>
#include <QDebug>

OptionsWindow::OptionsWindow(ConfigManager* configManager, QWidget* parent)
    : QDialog(parent), configManager(configManager), hotkeyEditing(nullptr) {
    setWindowTitle("Options");
    setAttribute(Qt::WA_QuitOnClose, false);  // Prevent the application from quitting

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* hotkeyLabel = new QLabel("Screenshot Hotkey:", this);
    layout->addWidget(hotkeyLabel);
    hotkeyEdit = new QLineEdit(this);
    hotkeyEdit->setPlaceholderText("Press any key...");
    hotkeyEdit->setReadOnly(true);
    hotkeyEdit->installEventFilter(this);  // Install event filter
    layout->addWidget(hotkeyEdit);

    QLabel* fullscreenHotkeyLabel = new QLabel("Fullscreen Screenshot Hotkey:", this);
    layout->addWidget(fullscreenHotkeyLabel);
    fullscreenHotkeyEdit = new QLineEdit(this);
    fullscreenHotkeyEdit->setPlaceholderText("Press any key...");
    fullscreenHotkeyEdit->setReadOnly(true);
    fullscreenHotkeyEdit->installEventFilter(this);  // Install event filter
    layout->addWidget(fullscreenHotkeyEdit);

    QLabel* extensionLabel = new QLabel("File Extension:", this);
    layout->addWidget(extensionLabel);
    extensionCombo = new QComboBox(this);
    extensionCombo->addItems({ "png", "jpg" });
    layout->addWidget(extensionCombo);

    QLabel* qualityLabel = new QLabel("Image Quality:", this);
    layout->addWidget(qualityLabel);
    qualitySpinbox = new QSpinBox(this);
    qualitySpinbox->setRange(1, 100);
    layout->addWidget(qualitySpinbox);

    QLabel* folderLabel = new QLabel("Default Save Folder:", this);
    layout->addWidget(folderLabel);
    folderEdit = new QLineEdit(this);
    layout->addWidget(folderEdit);
    QPushButton* browseButton = new QPushButton("Browse", this);
    layout->addWidget(browseButton);

    startWithSystemCheckbox = new QCheckBox("Start with system", this);
    layout->addWidget(startWithSystemCheckbox);

    QPushButton* saveButton = new QPushButton("Save", this);
    layout->addWidget(saveButton);

    connect(browseButton, &QPushButton::clicked, this, &OptionsWindow::browseFolder);
    connect(saveButton, &QPushButton::clicked, this, &OptionsWindow::saveOptions);

    loadOptions();
}

void OptionsWindow::loadOptions() {
    QJsonObject config = configManager->loadConfig();
    hotkeyEdit->setText(config["screenshot_hotkey"].toString());
    fullscreenHotkeyEdit->setText(config["fullscreen_hotkey"].toString());
    extensionCombo->setCurrentText(config["file_extension"].toString());
    qualitySpinbox->setValue(config["image_quality"].toInt());
    folderEdit->setText(config["default_save_folder"].toString());
    startWithSystemCheckbox->setChecked(config["start_with_system"].toBool());
}

void OptionsWindow::saveOptions() {
    QJsonObject config;
    config["screenshot_hotkey"] = hotkeyEdit->text();
    config["fullscreen_hotkey"] = fullscreenHotkeyEdit->text();
    config["file_extension"] = extensionCombo->currentText();
    config["image_quality"] = qualitySpinbox->value();
    config["default_save_folder"] = folderEdit->text();
    config["start_with_system"] = startWithSystemCheckbox->isChecked();

    configManager->saveConfig(config);
    accept();
}

void OptionsWindow::browseFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!folder.isEmpty()) {
        folderEdit->setText(folder);
    }
}

void OptionsWindow::startRecordingHotkey() {
    hotkeyEdit->setText("");
    hotkeyEdit->setPlaceholderText("Press any key...");
    hotkeyEditing = hotkeyEdit;
    currentKeys.clear();
    pressedKeys.clear();
}

void OptionsWindow::startRecordingFullscreenHotkey() {
    fullscreenHotkeyEdit->setText("");
    fullscreenHotkeyEdit->setPlaceholderText("Press any key...");
    hotkeyEditing = fullscreenHotkeyEdit;
    currentKeys.clear();
    pressedKeys.clear();
}

void OptionsWindow::recordKey(QKeyEvent* event) {
    int key = event->key();
    if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta) {
        return;  // Ignore standalone modifier keys
    }

    QString keyString = QKeySequence(key | event->modifiers()).toString(QKeySequence::NativeText);
    if (event->type() == QEvent::KeyPress) {
        if (!pressedKeys.contains(key)) {
            pressedKeys.insert(key);
            if (!currentKeys.contains(keyString)) {
                currentKeys += keyString + " ";
            }
        }
    }
    else if (event->type() == QEvent::KeyRelease) {
        pressedKeys.remove(key);
    }

    hotkeyEditing->setText(currentKeys.trimmed());
}

void OptionsWindow::keyPressEvent(QKeyEvent* event) {
    recordKey(event);
}

void OptionsWindow::keyReleaseEvent(QKeyEvent* event) {
    recordKey(event);
}

bool OptionsWindow::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        if (watched == hotkeyEdit) {
            startRecordingHotkey();
        }
        else if (watched == fullscreenHotkeyEdit) {
            startRecordingFullscreenHotkey();
        }
        return true;
    }
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        recordKey(keyEvent);
        return true;
    }
    return QDialog::eventFilter(watched, event);
}
