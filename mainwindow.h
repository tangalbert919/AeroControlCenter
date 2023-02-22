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

private:
    EC *ec;
    QTimer *timer;
    Ui::MainWindow *ui;
    HWMonitor *utils;

    // Hardware statistics
    QGraphicsEllipseItem *cpuGauge, *gpuGauge, *memoryGauge;
    QGraphicsScene *cpuScene, *gpuScene, *memoryScene;
    QGraphicsTextItem *cpuText, *gpuText, *memoryText;

    // Menu bar
    QMenu *fileMenu;
    QAction *aboutAction;
    void getCPUInfo();
    void setupGauges();
    void setupMenu();

private slots:
    void openAboutPopup();
    void printSliderPosition();
    void updateSliderPosition();
    void updateGauge();
    void updateFanMode(unsigned short mode);
};
#endif // MAINWINDOW_H
