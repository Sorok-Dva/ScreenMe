#include "include/editor.h"

Editor::Editor(QWidget* parent)
    : QWidget(parent), layout(new QVBoxLayout(this)), currentTool(None), currentColor(Qt::black) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setLayout(layout);

    createToolButton("Pen", Pen);
    createToolButton("Text", Text);
    createToolButton("Rectangle", Rectangle);
    createToolButton("Ellipse", Ellipse);
    createToolButton("Line", Line);
    createToolButton("Arrow", Arrow);

    QPushButton* colorButton = new QPushButton(this);
    colorButton->setFixedSize(24, 24);
    colorButton->setStyleSheet("background-color: black");
    connect(colorButton, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::black, this, "Select Color");
        if (color.isValid()) {
            currentColor = color;
            emit colorChanged(color);
        }
    });
    layout->addWidget(colorButton);
}

void Editor::createToolButton(const QString& toolName, Tool toolId) {
    QPushButton* button = new QPushButton(toolName, this);
    button->setCheckable(true);
    connect(button, &QPushButton::clicked, [this, toolId, button]() {
        currentTool = toolId;
        emit toolChanged(toolId);
        for (QPushButton* btn : toolButtons) {
            if (btn != button) {
                btn->setChecked(false);
            }
        }
    });
    layout->addWidget(button);
    toolButtons.append(button);
}

void Editor::deselectTools() {
    currentTool = None;
    emit toolChanged(None);
    for (QPushButton* button : toolButtons) {
        button->setChecked(false);
    }
}

void Editor::setCurrentColor(const QColor& color) {
    currentColor = color;
    emit colorChanged(color);
}
