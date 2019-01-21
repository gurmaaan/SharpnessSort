#ifndef CLICKABLEGS_H
#define CLICKABLEGS_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QDebug>
#include <QGuiApplication>

class ClickableGS : public QGraphicsScene
{
    Q_OBJECT

public:
    ClickableGS();
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

#endif // CLICKABLEGS_H
