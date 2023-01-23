#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "utils.h"

#include <QMainWindow>
#include <QGraphicsScene>

#include <libusb-1.0/libusb.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer;

private:
    Ui::MainWindow *ui;
    Utils *utils;

    QGraphicsScene *cpuScene, *gpuScene, *memoryScene;
};
#endif // MAINWINDOW_H
