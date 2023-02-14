#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ec.h"
#include "hwmonitor.h"

#include <QMainWindow>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>

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

public slots:
    void updateGauge();

private:
    EC *ec;
    Ui::MainWindow *ui;
    HWMonitor *utils;

    // Hardware statistics
    QGraphicsEllipseItem *cpuGauge, *gpuGauge, *memoryGauge;
    QGraphicsScene *cpuScene, *gpuScene, *memoryScene;
    QGraphicsTextItem *cpuText, *gpuText, *memoryText;

    // Menu bar
    QMenu *fileMenu;
    QAction *aboutAction;

private slots:
    void openAboutPopup();
};
#endif // MAINWINDOW_H
