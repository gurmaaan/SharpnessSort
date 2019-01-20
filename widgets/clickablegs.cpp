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
    qDebug()<< event->scenePos();
}
