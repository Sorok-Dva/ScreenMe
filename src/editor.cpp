#include "include/editor.h"
#include <QHBoxLayout>
#include <QIcon>

Editor::Editor(QWidget* parent)
    : QWidget(parent), currentTool_(None) {

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout* layout = new QHBoxLayout(this);

    // Create tool buttons
    createButton("Pen", Pen);
    createButton("Text", Text);
    createButton("Rectangle", Rectangle);
    createButton("Oval", Oval);
    createButton("Line", Line);
    createButton("Arrow", Arrow);

    setLayout(layout);
}

void Editor::createButton(const QString& text, Tool tool) {
    QPushButton* button = new QPushButton(text, this);
    layout()->addWidget(button);
    connect(button, &QPushButton::clicked, this, [=]() {
        deselectTools();
        currentTool_ = tool;
        button->setChecked(true);
        emit toolSelected(tool);
    });
}

void Editor::deselectTools() {
    currentTool_ = None;
    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        button->setChecked(false);
    }
}

