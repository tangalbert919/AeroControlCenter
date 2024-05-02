#ifndef RGBGRAPHICSVIEW_H
#define RGBGRAPHICSVIEW_H

#include "rgbkeyboard.h"
#include <QGraphicsView>

class RGBGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    RGBGraphicsView(QWidget* parent = nullptr);

    void setupLayout();
    void bindKeyboard(RGBKeyboard *keyboard);

public slots:
    void changeColors(int color);
    void changeMode(int mode);
    void adjustBrush(int color, int rgb);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QBrush customBrush;
    int r, g, b;
    RGBKeyboard *rgb;
    int mode, color;
};

#endif // RGBGRAPHICSVIEW_H
