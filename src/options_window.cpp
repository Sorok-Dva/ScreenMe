#include "../include/options_window.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QJsonObject>
#include <QEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QCheckBox>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include "../include/globalKeyboardHook.h"
#endif

OptionsWindow::OptionsWindow(ConfigManager* configManager, QWidget* parent)
    : QDialog(parent), configManager(configManager), hotkeyEditing(nullptr) {
    setWindowTitle(tr("ScreenMe Options"));
    setWindowIcon(QIcon("resources/icon.png"));
    setAttribute(Qt::WA_QuitOnClose, false);  // Prevent the application from quitting

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* hotkeyLabel = new QLabel(tr("Screenshot Hotkey:"), this);
    layout->addWidget(hotkeyLabel);
    hotkeyEdit = new QLineEdit(this);
    hotkeyEdit->setPlaceholderText(tr("Press any key..."));
#ifdef Q_OS_WIN
    hotkeyEdit->setReadOnly(true);
#else
    hotkeyEdit->setReadOnly(false);
#endif
    hotkeyEdit->installEventFilter(this);  // Install event filter
    layout->addWidget(hotkeyEdit);

    QLabel* fullscreenHotkeyLabel = new QLabel(tr("Fullscreen Screenshot Hotkey:"), this);
    layout->addWidget(fullscreenHotkeyLabel);
    fullscreenHotkeyEdit = new QLineEdit(this);
    fullscreenHotkeyEdit->setPlaceholderText(tr("Press any key..."));
#ifdef Q_OS_WIN
    fullscreenHotkeyEdit->setReadOnly(true);
#else
    fullscreenHotkeyEdit->setReadOnly(false);
#endif
    fullscreenHotkeyEdit->installEventFilter(this);  // Install event filter
    layout->addWidget(fullscreenHotkeyEdit);

    QLabel* extensionLabel = new QLabel(tr("File Extension:"), this);
    layout->addWidget(extensionLabel);
    extensionCombo = new QComboBox(this);
    extensionCombo->addItems({ "png", "jpg" });
    layout->addWidget(extensionCombo);

    QLabel* qualityLabel = new QLabel(tr("Image Quality:"), this);
    layout->addWidget(qualityLabel);
    qualitySpinbox = new QSpinBox(this);
    qualitySpinbox->setRange(1, 100);
    layout->addWidget(qualitySpinbox);

    QLabel* folderLabel = new QLabel(tr("Default Save Folder:"), this);
    layout->addWidget(folderLabel);
    folderEdit = new QLineEdit(this);
    layout->addWidget(folderEdit);
    QPushButton* browseButton = new QPushButton(tr("Browse"), this);
    layout->addWidget(browseButton);

    QLabel* languageLabel = new QLabel(tr("Language:"), this);
    layout->addWidget(languageLabel);
    languageCombo = new QComboBox(this);
    languageCombo->addItem(tr("English"), QStringLiteral("en"));
    languageCombo->addItem(tr("Français"), QStringLiteral("fr_FR"));
    layout->addWidget(languageCombo);

    startWithSystemCheckbox = new QCheckBox(tr("Start with system"), this);
#ifndef Q_OS_WIN
    startWithSystemCheckbox->setVisible(false);
#endif
    layout->addWidget(startWithSystemCheckbox);

    QPushButton* saveButton = new QPushButton(tr("Save"), this);
    layout->addWidget(saveButton);

    connect(browseButton, &QPushButton::clicked, this, &OptionsWindow::browseFolder);
    connect(saveButton, &QPushButton::clicked, this, &OptionsWindow::saveOptions);

    loadOptions();

#ifdef Q_OS_WIN
    // Hook up the global keyboard hook
    auto* keyboardHook = new GlobalKeyboardHook(this);
    connect(keyboardHook, &GlobalKeyboardHook::keyPressed, this, &OptionsWindow::handleGlobalKeyPress);
    keyboardHook->start();
#endif
}

void OptionsWindow::loadOptions() {
    QJsonObject config = configManager->loadConfig();
    hotkeyEdit->setText(config["screenshot_hotkey"].toString());
    fullscreenHotkeyEdit->setText(config["fullscreen_hotkey"].toString());
    extensionCombo->setCurrentText(config["file_extension"].toString());
    qualitySpinbox->setValue(config["image_quality"].toInt());
    folderEdit->setText(config["default_save_folder"].toString());
    startWithSystemCheckbox->setChecked(config["start_with_system"].toBool());
    const QString language = config["language"].toString(QStringLiteral("en"));
    int idx = languageCombo->findData(language);
    if (idx < 0) {
        idx = 0;
    }
    languageCombo->setCurrentIndex(idx);
}

void OptionsWindow::saveOptions() {
    QJsonObject config = configManager->loadConfig();
    const QString previousLanguage = config["language"].toString(QStringLiteral("en"));

    config["screenshot_hotkey"] = hotkeyEdit->text();
    config["fullscreen_hotkey"] = fullscreenHotkeyEdit->text();
    config["file_extension"] = extensionCombo->currentText();
    config["image_quality"] = qualitySpinbox->value();
    config["default_save_folder"] = folderEdit->text();
    config["start_with_system"] = startWithSystemCheckbox->isChecked();
    config["language"] = languageCombo->currentData().toString();

    configManager->saveConfig(config);

    if (previousLanguage != config["language"].toString()) {
        QMessageBox::information(this,
                                 tr("Language updated"),
                                 tr("Restart ScreenMe to apply the new language."));
    }

    emit reloadHotkeys();

    accept();
}

void OptionsWindow::browseFolder() {
    QString folder = QFileDialog::getExistingDirectory(this, tr("Select Folder"));
    if (!folder.isEmpty()) {
        folderEdit->setText(folder);
    }
}

void OptionsWindow::startRecordingHotkey() {
    hotkeyEdit->setText("");
    hotkeyEdit->setPlaceholderText(tr("Press any key..."));
    hotkeyEditing = hotkeyEdit;
    hotkeyEdit->setFocus(Qt::MouseFocusReason);
}

void OptionsWindow::startRecordingFullscreenHotkey() {
    fullscreenHotkeyEdit->setText("");
    fullscreenHotkeyEdit->setPlaceholderText(tr("Press any key..."));
    hotkeyEditing = fullscreenHotkeyEdit;
    fullscreenHotkeyEdit->setFocus(Qt::MouseFocusReason);
}

void OptionsWindow::handleGlobalKeyPress(QKeySequence keySequence) {
    if (hotkeyEditing) {
        hotkeyEditing->setText(keySequence.toString(QKeySequence::NativeText));
        hotkeyEditing->clearFocus();
        hotkeyEditing = nullptr;
    }
}

bool OptionsWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == hotkeyEdit || watched == fullscreenHotkeyEdit) {
        if (event->type() == QEvent::MouseButtonPress) {
            if (watched == hotkeyEdit) {
                startRecordingHotkey();
            }
            else {
                startRecordingFullscreenHotkey();
            }
            return true;
        }
#ifndef Q_OS_WIN
        if (event->type() == QEvent::KeyPress && hotkeyEditing) {
            auto* keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key();
            if (key == Qt::Key_unknown) {
                return true;
            }

            if (key == Qt::Key_Control || key == Qt::Key_Shift ||
                key == Qt::Key_Alt || key == Qt::Key_Meta) {
                return true;
            }

            Qt::KeyboardModifiers mods = keyEvent->modifiers();
            QKeySequence sequence(mods | key);

            handleGlobalKeyPress(sequence);
            return true;
        }
#endif
    }

    return QDialog::eventFilter(watched, event);
}
