#include "../include/customTextEdit.h"

CustomTextEdit::CustomTextEdit(QWidget* parent)
    : QTextEdit(parent) {}

void CustomTextEdit::focusOutEvent(QFocusEvent* event) {
    emit focusOut();
    QTextEdit::focusOutEvent(event);
}
