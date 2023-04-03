#ifndef RGBGRAPHICSVIEW_H
#define RGBGRAPHICSVIEW_H

#include <QGraphicsView>

class RGBGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    RGBGraphicsView(QWidget* parent = nullptr);

    void setupLayout();

public slots:
    void changeColors(int color);
    void changeMode(int mode);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // RGBGRAPHICSVIEW_H
