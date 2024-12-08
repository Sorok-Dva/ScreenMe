#include "../include/editor.h"
#include <QIcon>

Editor::Editor(QWidget* parent)
    : QWidget(parent), layout(new QVBoxLayout(this)), currentTool(None), currentColor(Qt::black) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setLayout(layout);

    colorButton = new QPushButton(this);
    colorButton->setFixedSize(24, 24);
    colorButton->setStyleSheet("background-color: black");
    connect(colorButton, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(currentColor, this, "Select Color");
        if (color.isValid()) {
            setCurrentColor(color);
            emit colorChanged(color);
        }
    });
    layout->addWidget(colorButton);

    createToolButton("Pen", Pen, QIcon(":/resources/icons/pen.png"));
    createToolButton("Text", Text, QIcon(":/resources/icons/text.png"));
    createToolButton("Rectangle", Rectangle, QIcon(":/resources/icons/rectangle.png"));
    createToolButton("Oval", Ellipse, QIcon(":/resources/icons/ellipse.png"));
    createToolButton("Line", Line, QIcon(":/resources/icons/line.png"));
    createToolButton("Arrow", Arrow, QIcon(":/resources/icons/arrow.png"));

    actionLayout = new QHBoxLayout();
    createActionButton("Save", QIcon(":/resources/icons/save.png"), "saveRequested");
    createActionButton("Copy to clipboard (CTRL + C)", QIcon(":/resources/icons/copy.png"), "copyRequested");
    createActionButton("Upload to ScreenMe", QIcon(":/resources/icons/upload.png"), "publishRequested");
    createActionButton("Find similar image", QIcon(":/resources/icons/search.png"), "searchRequested");
    createActionButton("Close editor", QIcon(":/resources/icons/close.png"), "closeRequested");
    layout->addLayout(actionLayout);
}

QColor Editor::getCurrentColor() const
{
    return currentColor;
}

void Editor::createToolButton(const QString& toolName, Tool tool, const QIcon& icon) {
    QPushButton* button = new QPushButton(this);
    button->setIcon(icon);
    button->setIconSize(QSize(15, 15));
    button->setCheckable(true);
    button->setToolTip(toolName);
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
    layout->addWidget(button);
    toolButtons.append(button);
}

void Editor::createActionButton(const QString& tooltip, const QIcon& icon, const QString& signal) {
    QPushButton* button = new QPushButton(this);
    button->setIcon(icon);
    button->setIconSize(QSize(15, 15));
    button->setToolTip(tooltip);
    connect(button, &QPushButton::clicked, this, [this, signal]() {
        QMetaObject::invokeMethod(this, signal.toUtf8().constData());
    });
    layout->addWidget(button);
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
    colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
}
