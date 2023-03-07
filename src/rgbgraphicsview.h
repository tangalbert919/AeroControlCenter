#ifndef RGBGRAPHICSVIEW_H
#define RGBGRAPHICSVIEW_H

#include <QGraphicsView>

class RGBGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    RGBGraphicsView(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // RGBGRAPHICSVIEW_H
