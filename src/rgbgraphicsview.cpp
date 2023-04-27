#include "rgbgraphicsview.h"

#include <QGraphicsItem>
#include <QMouseEvent>

RGBGraphicsView::RGBGraphicsView(QWidget* parent) : QGraphicsView(parent)
{

}

void RGBGraphicsView::changeColors(int color)
{
    QGraphicsScene *scene = this->scene();
    QList<QGraphicsItem*> items = scene->items();
    QBrush brush;
    int colors[7] = { Qt::red, Qt::green, Qt::yellow, Qt::blue, 0, 0, Qt::white };
    qInfo("Color is %d", color);
    if (color == 4) // orange
        brush = QBrush(QColor(253,126,20));
    else if (color == 5) // purple
        brush = QBrush(QColor(153,0,204));
    else
        brush = QBrush((Qt::GlobalColor) colors[color]);
    for (auto i = items.begin(), end = items.end(); i != end; i++) {
        QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem*>(*i);
        rect->setBrush(brush);
        rect->update();
    }
}

void RGBGraphicsView::changeMode(int mode)
{
    // TODO: Implement
    QGraphicsScene *scene = this->scene();
    QList items = scene->items();
    qInfo("Mode is %d", mode);
    if (mode == 0) { // static
        // TODO: Implement
    }
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

    QPen pen(QColor(Qt::black), 3, Qt::SolidLine);
    // Function row
    for (int i = 0; i < 19; i++) {
        scene->addRect(QRectF(i*40,10,40,20), pen);
    }
    // Second row
    for (int i = 0; i < 18; i++) {
        if (i == 13) // Backspace
            scene->addRect(QRectF(i*40,30,80,40), pen);
        else if (i > 13)
            scene->addRect(QRectF(i*40+40,30,40,40), pen);
        else
            scene->addRect(QRectF(i*40,30,40,40), pen);
    }
    // Third row
    for (int i = 0; i < 17; i++) {
        if (i == 0) // Tab button
            scene->addRect(QRectF(0,70,60,40), pen);
        else if (i == 13) // Backslash
            scene->addRect(QRectF(540,70,60,40), pen);
        else if (i > 0 && i < 13) // Between tab and backslash
            scene->addRect(QRectF(i*40+20,70,40,40), pen);
        else
            scene->addRect(QRectF(i*40+40,70,40,40), pen);
    }
    // Fourth row
    for (int i = 0; i < 16; i++) {
        if (i == 0) // Caps lock
            scene->addRect(QRectF(0,110,70,40), pen);
        else if (i == 12) // Enter
            scene->addRect(QRectF(510,110,90,40), pen);
        else if (i > 0 && i < 12)
            scene->addRect(QRectF(i*40+30,110,40,40), pen);
        else
            scene->addRect(QRectF(i*40+80,110,40,40), pen);
    }
    // Add button (number pad)
    scene->addRect(QRectF(720,70,40,80), pen);
    // Fifth row
    for (int i = 0; i < 16; i++) {
        if (i == 0) // Left shift
            scene->addRect(QRectF(0,150,90,40), pen);
        else if (i == 11) // Right shift
            scene->addRect(QRectF(490,150,70,40), pen);
        else if (i > 0 && i < 11)
            scene->addRect(QRectF(i*40+50,150,40,40), pen);
        else
            scene->addRect(QRectF(i*40+80,150,40,40), pen);
    }
    // Sixth row
    for (int i = 0; i < 13; i++) {
        if (i == 0) // Left control
            scene->addRect(QRectF(0,190,50,40), pen);
        else if (i == 4) // Space bar
            scene->addRect(QRectF(170,190,220,40), pen);
        else if (i == 7) // Right control
            scene->addRect(QRectF(470,190,50,40), pen);
        else if (i > 0 && i < 4)
            scene->addRect(QRectF(i*40+10,190,40,40), pen);
        else if (i > 4 && i < 7)
            scene->addRect(QRectF(i*40+190,190,40,40), pen);
        else
            scene->addRect(QRectF(i*40+200,190,40,40), pen);
    }
    // Enter key (number pad)
    scene->addRect(QRectF(720,150,40,80), pen);
}
