#include "clickablegs.h"

ClickableGS::ClickableGS()
{

}

void ClickableGS::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void ClickableGS::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->items().length() > 0)
    {
        this->items().first()->setPos(event->pos());
    }
}
