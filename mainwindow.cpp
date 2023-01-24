#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cmath>
#include <QGraphicsEllipseItem>
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

    gpuScene = new QGraphicsScene(this);
    ui->gpuView->setScene(gpuScene);
    gpuScene->setSceneRect(QRectF(0,0,256,192));
    gpuScene->addText("TODO: Get GPU usage");

    memoryScene = new QGraphicsScene(this);
    ui->memoryView->setScene(memoryScene);
    memoryScene->setSceneRect(QRectF(0,0,256,192));

    // Setup CPU, GPU, and memory gauges.
    QPen pen(QColor(253,126,20), 3, Qt::SolidLine);
    cpuGauge = new QGraphicsEllipseItem(53,21,150,150);
    cpuGauge->setStartAngle(225 * 16);
    cpuGauge->setSpanAngle(-270 * 16);
    cpuGauge->setPen(pen);
    cpuScene->addItem(cpuGauge);

    // Disabled for now, as polling GPU usage is not available.
    /*gpuGauge = new QGraphicsEllipseItem(53,21,150,150);
    gpuGauge->setStartAngle(225 * 16);
    gpuGauge->setSpanAngle(-270 * 16);
    gpuGauge->setPen(pen);
    gpuScene->addItem(gpuGauge);*/

    memoryGauge = new QGraphicsEllipseItem(53,21,150,150);
    memoryGauge->setStartAngle(225 * 16);
    memoryGauge->setSpanAngle(-270 * 16);
    memoryGauge->setPen(pen);
    memoryScene->addItem(memoryGauge);

    // Start timer to refresh gauge
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updateGauge()));
    timer->start(2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateGauge() {
    int cpuUse = (int) std::round(270 * (utils->getCPUUsage()));
    cpuGauge->setSpanAngle(-cpuUse * 16);
    //int gpuUse = (int) std::round(270 * utils->getGPUUsage());
    //gpuGauge->setSpanAngle(-gpuUse * 16);
    int memoryUse = (int) std::round(270 * utils->getMemoryUsage());
    memoryGauge->setSpanAngle(-memoryUse * 16);
}
