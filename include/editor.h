#pragma once

#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QList>
#include <QHBoxLayout>

class Editor : public QWidget {
    Q_OBJECT
public:
    enum Tool {
        None,
        Pen,
        Text,
        Rectangle,
        Ellipse,
        Line,
        Arrow
    };
    QColor currentColor;

    explicit Editor(QWidget* parent = nullptr);

    Tool getCurrentTool() const { return currentTool; }
    QColor getCurrentColor() const;

signals:
    void toolChanged(Tool tool);
    void colorChanged(const QColor& color);
    void saveRequested();
    void copyRequested();
    void publishRequested();
    void closeRequested();

public slots:
    void deselectTools();
    void setCurrentColor(const QColor& color);


private:
    void createToolButton(const QString& toolName, Tool tool, const QIcon& icon);
    void createActionButton(const QString& tooltip, const QIcon& icon, const QString& signal);

    QVBoxLayout* layout;
    QHBoxLayout* actionLayout;
    QPushButton* colorButton;
    QList<QPushButton*> toolButtons;
    Tool currentTool;
};

#endif // EDITOR_H
