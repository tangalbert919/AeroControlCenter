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
        if (i == 13) // Backspace
            scene->addRect(QRectF(i*40,20,80,40), pen);
        else if (i > 13)
            scene->addRect(QRectF(i*40+40,20,40,40), pen);
        else
            scene->addRect(QRectF(i*40,20,40,40), pen);
    }
    // Third row
    for (int i = 0; i < 17; i++) {
        if (i == 0) // Tab button
            scene->addRect(QRectF(0,60,60,40), pen);
        else if (i == 13) // Backslash
            scene->addRect(QRectF(540,60,60,40), pen);
        else if (i > 0 && i < 13) // Between tab and backslash
            scene->addRect(QRectF(i*40+20,60,40,40), pen);
        else
            scene->addRect(QRectF(i*40+40,60,40,40), pen);
    }
    // Fourth row
    for (int i = 0; i < 16; i++) {
        if (i == 0) // Caps lock
            scene->addRect(QRectF(0,100,70,40), pen);
        else if (i == 12) // Enter
            scene->addRect(QRectF(510,100,90,40), pen);
        else if (i > 0 && i < 12)
            scene->addRect(QRectF(i*40+30,100,40,40), pen);
        else
            scene->addRect(QRectF(i*40+80,100,40,40), pen);
    }
    // Add button (number pad)
    scene->addRect(QRectF(720,60,40,80), pen);
    // Fifth row
    for (int i = 0; i < 16; i++) {
        if (i == 0) // Left shift
            scene->addRect(QRectF(0,140,90,40), pen);
        else if (i == 11) // Right shift
            scene->addRect(QRectF(490,140,70,40), pen);
        else if (i > 0 && i < 11)
            scene->addRect(QRectF(i*40+50,140,40,40), pen);
        else
            scene->addRect(QRectF(i*40+80,140,40,40), pen);
    }
    // Sixth row
    for (int i = 0; i < 13; i++) {
        if (i == 0) // Left control
            scene->addRect(QRectF(0,180,50,40), pen);
        else if (i == 4) // Space bar
            scene->addRect(QRectF(170,180,220,40), pen);
        else if (i == 7) // Right control
            scene->addRect(QRectF(470,180,50,40), pen);
        else if (i > 0 && i < 4)
            scene->addRect(QRectF(i*40+10,180,40,40), pen);
        else if (i > 4 && i < 7)
            scene->addRect(QRectF(i*40+190,180,40,40), pen);
        else
            scene->addRect(QRectF(i*40+200,180,40,40), pen);
    }
    // Enter key (number pad)
    scene->addRect(QRectF(720,140,40,80), pen);
}
