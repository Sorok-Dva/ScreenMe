#include "include/DraggableTextItem.h"

DraggableTextItem::DraggableTextItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditorInteraction);
}

void DraggableTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    offset = event->pos();
    QGraphicsTextItem::mousePressEvent(event);
}

void DraggableTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    setPos(mapToScene(event->pos() - offset));
    QGraphicsTextItem::mouseMoveEvent(event);
}
