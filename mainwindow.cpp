#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup initial UI.
    ui->setupUi(this);

    // Setup fan RPM display (TODO: Actually get fan RPM)
    ui->fan1RPM->display(1987);
    ui->fan2RPM->display(2018);

    // Setup CPU, GPU, and memory views
    // TODO: Remove scroll bars on all three views
    cpuScene = new QGraphicsScene(this);
    ui->cpuView->setScene(cpuScene);
    cpuScene->setSceneRect(QRectF(0,0,256,192));
    cpuScene->addText("TODO: Get CPU usage");

    gpuScene = new QGraphicsScene(this);
    ui->gpuView->setScene(gpuScene);
    gpuScene->setSceneRect(QRectF(0,0,256,192));
    gpuScene->addText("TODO: Get GPU usage");

    memoryScene = new QGraphicsScene(this);
    ui->memoryView->setScene(memoryScene);
    memoryScene->setSceneRect(QRectF(0,0,256,192));
    memoryScene->addText("TODO: Get memory usage");
}

MainWindow::~MainWindow()
{
    delete ui;
}
