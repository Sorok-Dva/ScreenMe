#include "../include/editor.h"
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QLabel>
#include <QFrame>
#include <QSizePolicy>
#include <QStyle>

Editor::Editor(QWidget* parent)
    : QWidget(parent),
    mainLayout(new QVBoxLayout(this)),
    toolLayout(new QVBoxLayout()),
    actionLayout(new QVBoxLayout()),
    currentTool(None),
    currentColor(Qt::white) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("floatingEditor");

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(15, 23, 42, 160));
    setGraphicsEffect(shadow);

    setLayout(mainLayout);
    mainLayout->setContentsMargins(8, 12, 8, 12);
    mainLayout->setSpacing(8);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    setStyleSheet(R"(
        #floatingEditor {
            background-color: rgba(15, 23, 42, 0.93);
            border: 1px solid rgba(148, 163, 184, 0.25);
            border-radius: 14px;
        }
        #floatingEditor QPushButton {
            background-color: rgba(148, 163, 184, 0.12);
            border: 1px solid transparent;
            border-radius: 8px;
            color: #E2E8F0;
        }
        #floatingEditor QPushButton:hover {
            background-color: rgba(148, 163, 184, 0.22);
        }
        #floatingEditor QPushButton:checked {
            background-color: #2563EB;
            border-color: #1D4ED8;
        }
        #floatingEditor QPushButton[variant="color"] {
            border-radius: 18px;
            border: 2px solid rgba(241, 245, 249, 0.6);
            padding: 0;
        }
        #floatingEditor QPushButton[variant="color"]:hover {
            border-color: #60A5FA;
        }
    )");

    colorButton = new QPushButton(this);
    colorButton->setProperty("variant", "color");
    colorButton->setFixedSize(26, 26);
    colorButton->setCursor(Qt::PointingHandCursor);
    connect(colorButton, &QPushButton::clicked, [this]() {
        const QColor color = QColorDialog::getColor(currentColor, this, tr("Select color"));
        if (color.isValid()) {
            setCurrentColor(color);
            emit colorChanged(color);
        }
    });
    updateColorSwatch();
    mainLayout->addWidget(colorButton, 0, Qt::AlignLeft);

    auto addDivider = [this]() {
        QFrame* divider = new QFrame(this);
        divider->setFrameShape(QFrame::HLine);
        divider->setFrameShadow(QFrame::Plain);
        divider->setStyleSheet("color: rgba(148, 163, 184, 0.25);");
        mainLayout->addWidget(divider);
    };

    addDivider();

    QWidget* toolContainer = new QWidget(this);
    toolContainer->setLayout(toolLayout);
    toolContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(4);
    toolLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(toolContainer);

    createToolButton(tr("Freehand"), Pen, QIcon(":/resources/icons/pen.png"));
    createToolButton(tr("Text"), Text, QIcon(":/resources/icons/text.png"));
    createToolButton(tr("Rectangle"), Rectangle, QIcon(":/resources/icons/rectangle.png"));
    createToolButton(tr("Ellipse"), Ellipse, QIcon(":/resources/icons/ellipse.png"));
    createToolButton(tr("Line"), Line, QIcon(":/resources/icons/line.png"));
    createToolButton(tr("Arrow"), Arrow, QIcon(":/resources/icons/arrow.png"));

    addDivider();

    QWidget* actionContainer = new QWidget(this);
    actionContainer->setLayout(actionLayout);
    actionContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(4);
    actionLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(actionContainer);

    createActionButton(tr("Save"), QIcon(":/resources/icons/save.png"), "saveRequested");
    createActionButton(tr("Copy"), QIcon(":/resources/icons/copy.png"), "copyRequested");
    createActionButton(tr("Upload"), QIcon(":/resources/icons/upload.png"), "publishRequested");
    QIcon printIcon = QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView);
    createActionButton(tr("Print"), printIcon, "printRequested");
    createActionButton(tr("Search"), QIcon(":/resources/icons/search.png"), "searchRequested");
    createActionButton(tr("Close"), QIcon(":/resources/icons/close.png"), "closeRequested");
}

QColor Editor::getCurrentColor() const
{
    return currentColor;
}

void Editor::createToolButton(const QString& toolName, Tool tool, const QIcon& icon) {
    auto* button = new QPushButton(this);
    button->setIcon(icon);
    button->setIconSize(QSize(14, 14));
    button->setCheckable(true);
    button->setToolTip(toolName);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(28, 28);
    button->setFocusPolicy(Qt::NoFocus);

    connect(button, &QPushButton::clicked, [this, tool, button]() {
        if (button->isChecked()) {
            currentTool = tool;
            for (QPushButton* btn : toolButtons) {
                if (btn != button) {
                    btn->setChecked(false);
                }
            }
            emit toolChanged(tool);
        } else {
            currentTool = None;
            emit toolChanged(None);
        }
    });

    toolLayout->addWidget(button, 0, Qt::AlignLeft);
    toolButtons.append(button);
}

void Editor::createActionButton(const QString& tooltip, const QIcon& icon, const QString& signal) {
    auto* button = new QPushButton(this);
    button->setToolTip(tooltip);
    button->setIcon(icon);
    button->setIconSize(QSize(14, 14));
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(28, 28);
    button->setFocusPolicy(Qt::NoFocus);

    connect(button, &QPushButton::clicked, this, [this, signal]() {
        QMetaObject::invokeMethod(this, signal.toUtf8().constData());
    });

    actionLayout->addWidget(button, 0, Qt::AlignLeft);
}

void Editor::deselectTools() {
    currentTool = None;
    for (QPushButton* button : toolButtons) {
        button->setChecked(false);
    }
    emit toolChanged(None);
}

void Editor::setCurrentColor(const QColor& color) {
    currentColor = color;
    updateColorSwatch();
}

void Editor::updateColorSwatch() {
    const QString swatchStyle = QString("background-color: %1;").arg(currentColor.name());
    colorButton->setStyleSheet(swatchStyle);
}
