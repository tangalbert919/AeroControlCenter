#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup initial UI.
    ui->setupUi(this);
    ui->fan1RPM->display(1987);

    // Setup CPU view (TODO: Actually display CPU usage)
    scene = new QGraphicsScene(this);
    ui->cpuView->setScene(scene);
    //QGraphicsScene scene = ui->cpuView->scene();
    scene->setSceneRect(QRectF(0,0,256,192));
    //QGraphicsTextItem text = "Hello World!";
    scene->addText("Hello World!");
}

MainWindow::~MainWindow()
{
    delete ui;
}
