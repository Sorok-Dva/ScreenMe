#include "../include/credits_dialog.h"

#include <QDesktopServices>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

CreditsDialog::CreditsDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(tr("Credits"));
    setWindowIcon(QIcon(":/resources/icon.png"));
    setAttribute(Qt::WA_QuitOnClose, false);
    setModal(true);
    setFixedSize(420, 520);
    setStyleSheet(R"(
        CreditsDialog {
            background-color: #0f172a;
            color: #f8fafc;
            border-radius: 20px;
        }
        QLabel#titleLabel {
            font-size: 24px;
            font-weight: 700;
            color: #e0f2fe;
        }
        QLabel#subtitleLabel {
            font-size: 13px;
            letter-spacing: 0.3em;
            text-transform: uppercase;
            color: rgba(148, 163, 184, 0.85);
        }
        QLabel#bodyLabel {
            font-size: 14px;
            line-height: 1.6em;
            color: rgba(226, 232, 240, 0.92);
        }
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #1e3a8a, stop:1 #2563eb);
            border: none;
            color: #f8fafc;
            border-radius: 12px;
            padding: 10px 16px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #2563eb, stop:1 #38bdf8);
        }
        QWidget#card {
            background: rgba(15, 23, 42, 0.55);
            border: 1px solid rgba(148, 163, 184, 0.18);
            border-radius: 16px;
            padding: 20px;
        }
    )");

    setupUi();
}

void CreditsDialog::setupUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 28, 28, 28);
    layout->setSpacing(24);

    auto* header = new QWidget(this);
    header->setObjectName("card");
    auto* headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(18, 18, 18, 18);
    headerLayout->setSpacing(8);

    auto* subtitle = new QLabel(tr("SCREENME"), header);
    subtitle->setObjectName("subtitleLabel");
    subtitle->setAlignment(Qt::AlignLeft);

    auto* title = new QLabel(tr("Creative Credits"), header);
    title->setObjectName("titleLabel");

    auto* summary = new QLabel(tr("Crafted with ❤️ by Sorok Dva. Empowering faster, privacy-first captures across all your displays."), header);
    summary->setObjectName("bodyLabel");
    summary->setWordWrap(true);

    headerLayout->addWidget(subtitle);
    headerLayout->addWidget(title);
    headerLayout->addWidget(summary);

    auto* infoCard = new QWidget(this);
    infoCard->setObjectName("card");
    auto* infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(18, 18, 18, 18);
    infoLayout->setSpacing(8);

    auto* line1 = new QLabel(tr("Design direction & product vision: <b>Sorok Dva</b>"), infoCard);
    line1->setObjectName("bodyLabel");
    line1->setWordWrap(true);

    auto* line2 = new QLabel(tr("Tech stack: Qt Widgets · Custom Tailwind-inspired styling"), infoCard);
    line2->setObjectName("bodyLabel");
    line2->setWordWrap(true);

    auto* line3 = new QLabel(tr("Special thanks to contributors and early adopters for feedback."), infoCard);
    line3->setObjectName("bodyLabel");
    line3->setWordWrap(true);

    infoLayout->addWidget(line1);
    infoLayout->addWidget(line2);
    infoLayout->addWidget(line3);

    auto* buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(12);

    auto* githubButton = new QPushButton(tr("Voir mon GitHub"), this);
    auto* websiteButton = new QPushButton(tr("Voir mon site"), this);

    buttonRow->addWidget(githubButton, 1);
    buttonRow->addWidget(websiteButton, 1);

    connect(githubButton, &QPushButton::clicked, this, [this]() {
        openLink(QStringLiteral("https://p-42.fr/github-dev-sorokdva"));
    });
    connect(websiteButton, &QPushButton::clicked, this, [this]() {
        openLink(QStringLiteral("https://p-42.fr/sorokdva-portfolio"));
    });

    auto* closeButton = new QPushButton(tr("Back to app"), this);
    closeButton->setMinimumHeight(38);
    closeButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        " background: transparent;"
        " border: 1px solid rgba(148,163,184,0.35);"
        " color: rgba(226,232,240,0.95);"
        " border-radius: 12px;"
        " font-weight: 600;"
        " }"
        "QPushButton:hover {"
        " background: rgba(148,163,184,0.12);"
        " }"
    ));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(header);
    layout->addWidget(infoCard);
    layout->addLayout(buttonRow);
    layout->addWidget(closeButton);
}

void CreditsDialog::openLink(const QString& url) {
    QDesktopServices::openUrl(QUrl(url));
}
