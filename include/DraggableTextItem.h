#ifndef DRAGGABLETEXTITEM_H
#define DRAGGABLETEXTITEM_H

#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>

class DraggableTextItem : public QGraphicsTextItem {
public:
    explicit DraggableTextItem(const QString& text, QGraphicsItem* parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QPointF offset;
};

#endif // DRAGGABLETEXTITEM_H
