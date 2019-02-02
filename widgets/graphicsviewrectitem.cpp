#include "graphicsviewrectitem.h"

GraphicsViewRectItem::GraphicsViewRectItem(QObject *parent):
    QObject(parent), QGraphicsItem()
{

}

GraphicsViewRectItem::GraphicsViewRectItem(QRectF boundingRect) :
    _boundingRect(boundingRect)
{

}

GraphicsViewRectItem::~GraphicsViewRectItem()
{

}

QRectF GraphicsViewRectItem::boundingRect() const
{
    return  _boundingRect;
}

void GraphicsViewRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(QBrush(QColor(Qt::red)), 2);
    painter->setPen(pen);
    painter->setBrush(Qt::transparent);
    painter->drawRect(_boundingRect);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void GraphicsViewRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    this->setPos(mapToScene(event->pos()));
}

void GraphicsViewRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->setCursor(QCursor(Qt::SizeAllCursor));
    Q_UNUSED(event);
}

void GraphicsViewRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event);
}
