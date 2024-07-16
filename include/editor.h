#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QColorDialog>

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

    explicit Editor(QWidget* parent = nullptr);

signals:
    void toolChanged(Tool tool);
    void colorChanged(const QColor& color);

public slots:
    void deselectTools();
    Tool getCurrentTool() const { return currentTool; }
    void setCurrentColor(const QColor& color);

private:
    void createToolButton(const QString& toolName, Tool toolId);

    QVBoxLayout* layout;
    QList<QPushButton*> toolButtons;
    Tool currentTool;
    QColor currentColor;
};

#endif // EDITOR_H
