#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

class Editor : public QWidget {
    Q_OBJECT
public:
    enum Tool {
        None,
        Pen,
        Text,
        Rectangle,
        Oval,
        Line,
        Arrow
    };

    explicit Editor(QWidget* parent = nullptr);

    void deselectTools();
    Tool currentTool() const { return currentTool_; }

signals:
    void toolSelected(Tool tool);

private:
    Tool currentTool_;
    QPushButton* penButton;
    QPushButton* textButton;
    QPushButton* rectangleButton;
    QPushButton* ovalButton;
    QPushButton* lineButton;
    QPushButton* arrowButton;

    void createButton(const QString& text, Tool tool);
};

#endif // EDITOR_H
