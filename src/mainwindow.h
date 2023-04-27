#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ec.h"
#include "hwmonitor.h"
#include "rgbgraphicsview.h"
#include "rgbkeyboard.h"

#include <QMainWindow>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QDBusInterface>

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
    RGBKeyboard *rgb;

    // Hardware statistics
    QGraphicsEllipseItem *cpuGauge, *gpuGauge, *memoryGauge;
    QGraphicsScene *cpuScene, *gpuScene, *memoryScene;
    QGraphicsTextItem *cpuText, *gpuText, *memoryText;

    // Menu bar
    QMenu *fileMenu;
    QAction *aboutAction;

    // RGB UI
    QGraphicsScene *rgbScene;
    RGBGraphicsView *rgbView;

    // D-Bus
    QDBusInterface *dbus;

    void getCPUInfo();
    void setupDBus();
    void setupGauges();
    void setupMenu();
    void setupRGB();

private slots:
    void openAboutPopup();
    void printSliderPosition(unsigned short slider);
    void updateSliderPosition();
    void updateGauge();
    void updateFanMode(unsigned short mode);
    void updateCustomFanSpeed(unsigned short speed);
    void updateChargingMode(unsigned short mode);
    void updateChargingLimit(unsigned short limit);
};
#endif // MAINWINDOW_H
