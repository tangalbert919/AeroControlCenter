#include "rgbgraphicsview.h"

#include <QMouseEvent>

RGBGraphicsView::RGBGraphicsView(QWidget* parent) : QGraphicsView(parent)
{

}

void RGBGraphicsView::changeColors(int color)
{
    // TODO: Implement
    setBackgroundBrush(Qt::yellow);
}

void RGBGraphicsView::changeMode(int mode)
{
    // TODO: Implement
}

void RGBGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // TODO: Implement
    QPointF point = mapToScene(event->pos());
    qInfo("Click at (%f,%f)", point.x(), point.y());
}

void RGBGraphicsView::setupLayout()
{
    QGraphicsScene *scene = this->scene();
    if (!scene) {
        qInfo("Setup a scene first!");
        return;
    }

    QPen pen(QColor(253,126,20), 3, Qt::SolidLine);
    // Function row
    for (int i = 0; i < 19; i++) {
        scene->addRect(QRectF(i*40,0,40,20), pen);
    }
    // Second row
    for (int i = 0; i < 18; i++) {
        // TODO: Implement
    }
}
