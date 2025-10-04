#pragma once

#include <QDialog>

class QPushButton;
class QLabel;

class CreditsDialog : public QDialog {
    Q_OBJECT
public:
    explicit CreditsDialog(QWidget* parent = nullptr);

private:
    void setupUi();
    void openLink(const QString& url);
};
