#include "rgbgraphicsview.h"

#include <QMouseEvent>

RGBGraphicsView::RGBGraphicsView(QWidget* parent) : QGraphicsView(parent)
{

}

void RGBGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // TODO: Implement
    QPointF point = mapToScene(event->pos());
    qInfo("Click at (%f,%f)", point.x(), point.y());
}
