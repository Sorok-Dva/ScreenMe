#include "include/editor.h"

Editor::Editor(QWidget* parent)
    : QWidget(parent), layout(new QVBoxLayout(this)), currentTool(None), currentColor(Qt::black) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setLayout(layout);

    createToolButton("Pen", Pen);
    createToolButton("Text", Text);
    createToolButton("Rectangle", Rectangle);
    createToolButton("Oval", Ellipse);
    createToolButton("Line", Line);
    createToolButton("Arrow", Arrow);

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
}

QColor Editor::getCurrentColor() const
{
    return currentColor;
}

void Editor::createToolButton(const QString& toolName, Tool tool) {
    QPushButton* button = new QPushButton(toolName, this);
    button->setCheckable(true);
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
