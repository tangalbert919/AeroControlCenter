#include "rgbgraphicsview.h"

#include <QGraphicsItem>
#include <QMouseEvent>

RGBGraphicsView::RGBGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
    // Setup custom brush
    r = 0;
    g = 0;
    b = 0;
    customBrush = QBrush(QColor(r, g, b));
}

void RGBGraphicsView::bindKeyboard(RGBKeyboard *keyboard)
{
    rgb = keyboard;
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
    else if (color >= 0)
        brush = QBrush((Qt::GlobalColor) colors[color]);
    else // Default if color mode does not have color selection.
        brush = QBrush((Qt::GlobalColor) colors[0]);
    for (auto i = items.begin(), end = items.end(); i != end; i++) {
        QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem*>(*i);
        rect->setBrush(brush);
        rect->update();
    }
    this->color = color;
}

void RGBGraphicsView::changeMode(int mode)
{
    // TODO: Implement
    QGraphicsScene *scene = this->scene();
    QList items = scene->items();
    qInfo("Mode is %d", mode);
    this->mode = mode;
    if (mode == 0) { // static
        changeColors(this->color);
    } else if (mode > 12) {
        uint8_t *rgb_data = rgb->getKeyRGB();
        rgb->getCustomModeLayout(mode);
        for (int i = 0; i < 128; i++) {
            for (auto j = items.begin(), end = items.end(); j != end; j++) {
                QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem*>(*j);
                if (!rect->data(0).toString().isEmpty() && rect->data(0).toInt() == i) {
                    rect->setBrush(QBrush(QColor(rgb_data[i*4+1], rgb_data[i*4+2], rgb_data[i*4+3])));
                    rect->update();
                    break;
                }
            }
        }
    }
}

void RGBGraphicsView::adjustBrush(int color, int rgb)
{
    if (rgb == 0)
        r = color;
    else if (rgb == 1)
        g = color;
    else
        b = color;
    customBrush.setColor(QColor(r, g, b));
}

void RGBGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // If not in custom mode, ignore mouse press events.
    if (this->mode <= 12) {
        return;
    }

    QPointF point = mapToScene(event->pos());
    qInfo("Click at (%f,%f)", point.x(), point.y());
    QGraphicsItem *item = this->itemAt(event->pos());
    if (item == nullptr) {
        qInfo("No item found here");
    }
    else {
        QGraphicsRectItem *rect = qgraphicsitem_cast<QGraphicsRectItem*>(item);
        rect->setBrush(customBrush);
        rect->update();
        if (!rect->data(0).toString().isEmpty()) {
            qInfo("Data: %d", rect->data(0).toInt());
            int index = rect->data(0).toInt();
            rgb->setKeyRGB(index, customBrush.color().red(),
                           customBrush.color().green(), customBrush.color().blue());
        }
    }
}

void RGBGraphicsView::setupLayout()
{
    QGraphicsScene *scene = this->scene();
    if (!scene) {
        qInfo("Setup a scene first!");
        return;
    }

    QPen pen(QColor(Qt::black), 3, Qt::SolidLine);
    // TODO: Reimplement this to use XML so we can support keyboards other than US/QWERTY.
    // Function row
    int fn_row_index[19] = {11, 17, 23, 29, 35, 41, 47, 53, 59, 65, 71, 77, 83, 89, 95, 101, 107, 113, 119};
    for (int i = 0; i < 19; i++) {
        QGraphicsRectItem* key = new QGraphicsRectItem(QRectF(i*40, 10, 40, 20));
        key->setData(0, fn_row_index[i]);
        key->setPen(pen);
        scene->addItem(key);
    }
    // Second row
    int sec_row_index[18] = {10, 16, 22, 28, 34, 40, 46, 52, 58, 64, 70, 76, 82, 94, 100, 106, 112, 118};
    for (int i = 0; i < 18; i++) {
        QGraphicsRectItem* key;
        if (i == 13) // Backspace
            key = new QGraphicsRectItem(QRectF(i*40, 30, 80, 40));
        else if (i > 13)
            key = new QGraphicsRectItem(QRectF(i*40+40, 30, 40, 40));
        else
            key = new QGraphicsRectItem(QRectF(i*40, 30, 40, 40));
        key->setData(0, sec_row_index[i]);
        key->setPen(pen);
        scene->addItem(key);
    }
    // Third row
    int thr_row_index[17] = {9, 15, 21, 27, 33, 39, 45, 51, 57, 63, 69, 75, 81, 87, 99, 105, 111};
    for (int i = 0; i < 17; i++) {
        QGraphicsRectItem* key;
        if (i == 0) // Tab button
            key = new QGraphicsRectItem(QRectF(0, 70, 60, 40));
        else if (i == 13) // Backslash
            key = new QGraphicsRectItem(QRectF(540 ,70, 60, 40));
        else if (i > 0 && i < 13) // Between tab and backslash
            key = new QGraphicsRectItem(QRectF(i*40+20, 70, 40, 40));
        else
            key = new QGraphicsRectItem(QRectF(i*40+40,70,40,40));
        key->setData(0, thr_row_index[i]);
        key->setPen(pen);
        scene->addItem(key);
    }
    // Fourth row
    int fou_row_index[16] = {8, 14, 20, 26, 32, 38, 44, 50, 56, 62, 68, 74, 92, 98, 104, 110};
    for (int i = 0; i < 16; i++) {
        QGraphicsRectItem* key;
        if (i == 0) // Caps lock
            key = new QGraphicsRectItem(QRectF(0, 110, 70, 40));
        else if (i == 12) // Enter
            key = new QGraphicsRectItem(QRectF(510, 110, 90, 40));
        else if (i > 0 && i < 12)
            key = new QGraphicsRectItem(QRectF(i*40+30, 110, 40, 40));
        else
            key = new QGraphicsRectItem(QRectF(i*40+80, 110, 40, 40));
        key->setData(0, fou_row_index[i]);
        key->setPen(pen);
        scene->addItem(key);
    }
    // Add button (number pad)
    QGraphicsRectItem* add_btn = new QGraphicsRectItem(QRectF(720, 70, 40, 80));
    add_btn->setData(0, 116);
    add_btn->setPen(pen);
    scene->addItem(add_btn);
    // Fifth row
    int fif_row_index[16] = {7, 19, 25, 31, 37, 43, 49, 55, 61, 67, 73, 85, 91, 97, 103, 109};
    for (int i = 0; i < 16; i++) {
        QGraphicsRectItem* key;
        if (i == 0) // Left shift
            key = new QGraphicsRectItem(QRectF(0, 150, 90, 40));
        else if (i == 11) // Right shift
            key = new QGraphicsRectItem(QRectF(490, 150, 70, 40));
        else if (i > 0 && i < 11)
            key = new QGraphicsRectItem(QRectF(i*40+50, 150, 40, 40));
        else
            key = new QGraphicsRectItem(QRectF(i*40+80, 150, 40, 40));
        key->setData(0, fif_row_index[i]);
        key->setPen(pen);
        scene->addItem(key);
    }
    // Sixth row
    int six_row_index[13] = {6, 12, 18, 24, 42, 60, 66, 72, 84, 90, 96, 102, 108};
    for (int i = 0; i < 13; i++) {
        QGraphicsRectItem* key;
        if (i == 0) // Left control
            key = new QGraphicsRectItem(QRectF(0, 190, 50, 40));
        else if (i == 4) // Space bar
            key = new QGraphicsRectItem(QRectF(170, 190, 220, 40));
        else if (i == 7) // Right control
            key = new QGraphicsRectItem(QRectF(470, 190, 50, 40));
        else if (i > 0 && i < 4)
            key = new QGraphicsRectItem(QRectF(i*40+10, 190, 40, 40));
        else if (i > 4 && i < 7)
            key = new QGraphicsRectItem(QRectF(i*40+190, 190, 40, 40));
        else
            key = new QGraphicsRectItem(QRectF(i*40+200, 190, 40, 40));
        key->setData(0, six_row_index[i]);
        key->setPen(pen);
        scene->addItem(key);
    }
    // Enter key (number pad)
    QGraphicsRectItem* enter_btn = new QGraphicsRectItem(QRectF(720, 150, 40, 80));
    enter_btn->setData(0, 114);
    enter_btn->setPen(pen);
    scene->addItem(enter_btn);
}
