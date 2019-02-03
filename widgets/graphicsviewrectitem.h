#ifndef GRAPHICSVIEWRECTITEM_H
#define GRAPHICSVIEWRECTITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QRectF>

class GraphicsViewRectItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit GraphicsViewRectItem(QObject *parent = nullptr);
    GraphicsViewRectItem(QRectF boundingRect);
    ~GraphicsViewRectItem();


signals:
    void posChanged(QRectF rect);

private:
    QRectF _boundingRect;
    //
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:

};

#endif // GRAPHICSVIEWRECTITEM_H
