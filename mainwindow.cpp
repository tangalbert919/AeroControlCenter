#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QMessageLogger log;
    // Detect if program is running on an Aero machine.
    // Otherwise, create a popup and exit.
    char temp[64];
    FILE *file = fopen("/sys/class/dmi/id/product_family", "r");
    fgets(temp, 64, file);
    fclose(file);
    if (strcmp(temp, "AERO\n") != 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("WARNING");
        msgBox.setText("This program will only run on an Aero machine.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        parent->close();
    }

    // TODO: Rename this class.
    utils = new Utils();

    // Setup initial UI.
    ui->setupUi(this);

    // Setup fan RPM display (TODO: Actually get fan RPM)
    ui->fan1RPM->display(1987);
    ui->fan2RPM->display(2018);

    // Setup CPU, GPU, and memory views
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
