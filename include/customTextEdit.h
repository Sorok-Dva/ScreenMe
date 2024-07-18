#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>

class CustomTextEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit CustomTextEdit(QWidget* parent = nullptr);

signals:
    void focusOut();

protected:
    void focusOutEvent(QFocusEvent* event) override;
};

#endif // CUSTOMTEXTEDIT_H
