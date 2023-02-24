#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cmath>
#include <cstring>
#include <QMessageBox>
#include <QFile>
#include <QTabBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Detect if program is running on an Aero or AORUS machine.
    // Otherwise, create a popup and exit.
    char temp[64];
    FILE *file = std::fopen("/sys/class/dmi/id/product_family", "r");
    std::fgets(temp, 64, file);
    std::fclose(file);
    if (std::strcmp(temp, "AERO\n") != 0 &&
            std::strcmp(temp, "AORUS\n") != 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("WARNING");
        msgBox.setText("This program will only run on an Aero or AORUS machine.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        parent->close();
    }

    // Setup hardware monitor.
    utils = new HWMonitor();

    // Setup embedded controller.
    ec = new EC();

    // Setup initial UI.
    ui->setupUi(this);

    // Setup the menus for the menubar.
    setupMenu();

    // Configure tab bar. Make sure tabs span the application width.
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);

    // Setup fan RPM display.
    ui->fan1RPM->display(0);
    ui->fan2RPM->display(0);

    // Setup the fan speed controls. Only effective in custom mode.
    connect(ui->fanCustomSlider, &QSlider::sliderReleased, this, &MainWindow::updateSliderPosition);
    connect(ui->fanCustomSlider, &QSlider::valueChanged, this, &MainWindow::printSliderPosition);
    connect(ui->setFanSpeedBtn, &QPushButton::clicked, this,
            [=]() { this->updateCustomFanSpeed(ui->fanCustomSlider->value()); });

    // Disable controls if kernel driver is not running.
    if (ec->getCustomFanSpeed() == 0) {
        ui->fanCustomSlider->setDisabled(true);
        ui->setFanSpeedBtn->setDisabled(true);
    } else {
        ui->fanCustomSlider->setValue(ec->getCustomFanSpeed());
        ui->fanCustomPercent->setText(QString::number(ec->getCustomFanSpeed()) + "%");
    }
    // Hide them if we are not on custom mode or kernel driver is not running.
    if (ec->getFanMode() < 3) {
        ui->fanCustomSlider->setVisible(false);
        ui->fanCustomPercent->setVisible(false);
        ui->setFanSpeedBtn->setVisible(false);
    }

    // Hook up fan mode buttons.
    connect(ui->fanNormalBtn, &QPushButton::clicked, this,
            [=]() { this->updateFanMode(0); });
    connect(ui->fanQuietBtn, &QPushButton::clicked, this,
            [=]() { this->updateFanMode(1); });
    connect(ui->fanGamingBtn, &QPushButton::clicked, this,
            [=]() { this->updateFanMode(2); });
    connect(ui->fanCustomBtn, &QPushButton::clicked, this,
            [=]() { this->updateFanMode(3); });

    // Setup CPU, GPU, and memory views and gauges.
    setupGauges();

    // Get CPU info.
    getCPUInfo();

    // Start timer to refresh gauge
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updateGauge()));
    timer->start(2000);
}

MainWindow::~MainWindow()
{
    timer->stop();
    delete timer;

    delete ui;
}

void MainWindow::getCPUInfo()
{
    QString cpuInfo = " Not obtained";
    QFile qf("/proc/cpuinfo");
    if (!qf.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug("failed");
    else {
        QTextStream in(&qf);
        QString line = in.readLine();
        bool cpuInfoFound = false;
        while (!line.isNull() && !cpuInfoFound) {
            if (line.contains("model name")) {
                cpuInfo = line.sliced(line.indexOf(":") + 1);
                cpuInfoFound = true;
            }
            line = in.readLine();
        }
    }
    ui->cpuLabel->setText("CPU:" + cpuInfo);
    qf.close();
}

void MainWindow::setupGauges()
{
    // Setup the scene first.
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

    // Setup the gauges.
    QPen pen(QColor(253,126,20), 3, Qt::SolidLine);
    cpuGauge = new QGraphicsEllipseItem(53,21,150,150);
    cpuGauge->setStartAngle(225 * 16);
    cpuGauge->setSpanAngle(-270 * 16);
    cpuGauge->setPen(pen);
    cpuScene->addItem(cpuGauge);
    cpuText = cpuScene->addText("Polling CPU usage...");
    cpuText->setY(150);
    cpuText->setX(127 - (cpuText->boundingRect().width() / 2));

    // Disabled for now, as polling GPU usage is not available.
    /*gpuGauge = new QGraphicsEllipseItem(53,21,150,150);
    gpuGauge->setStartAngle(225 * 16);
    gpuGauge->setSpanAngle(-270 * 16);
    gpuGauge->setPen(pen);
    gpuScene->addItem(gpuGauge);
    gpuText = gpuScene->addText("Polling GPU usage...");
    gpuText->setY(150);
    gpuText->setX(127 - (gpuText->boundingRect().width() / 2));*/

    memoryGauge = new QGraphicsEllipseItem(53,21,150,150);
    memoryGauge->setStartAngle(225 * 16);
    memoryGauge->setSpanAngle(-270 * 16);
    memoryGauge->setPen(pen);
    memoryScene->addItem(memoryGauge);
    memoryText = memoryScene->addText("Polling memory usage...");
    memoryText->setY(150);
    memoryText->setX(127 - (memoryText->boundingRect().width() / 2));
}

void MainWindow::setupMenu()
{
    fileMenu = new QMenu("File");
    aboutAction = new QAction("About...");
    aboutAction->setShortcut(QKeySequence::SelectAll);
    aboutAction->setIcon(QIcon("document-open"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::openAboutPopup);
    fileMenu->addAction(aboutAction);
    ui->menubar->addMenu(fileMenu);
}

// Add new slots below this comment.
void MainWindow::openAboutPopup()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About this program");
    aboutBox.setText("Aero Control Center " BUILD_VERSION);
    aboutBox.setIcon(QMessageBox::Information);
    aboutBox.exec();
}

void MainWindow::printSliderPosition()
{
    unsigned short speed;
    char temp[6];
    //qInfo("Slider position: %d", ui->fanCustomSlider->sliderPosition());
    speed = ui->fanCustomSlider->sliderPosition();
    std::sprintf(temp, "%d%%", speed);
    ui->fanCustomPercent->setText(temp);
}

void MainWindow::updateSliderPosition()
{
    unsigned short speed = ui->fanCustomSlider->sliderPosition();
    unsigned short remainder = speed % 5;
    if (remainder != 0) {
        speed = speed - remainder;
        ui->fanCustomSlider->setValue(speed);
    }
    printSliderPosition();
}

void MainWindow::updateGauge()
{
    // Because we are going clockwise, span angle must be negative.
    double cpuUse = utils->getCPUUsage();
    double cpuSpanAngle = -270 * cpuUse;
    cpuGauge->setSpanAngle((int) std::round(cpuSpanAngle) * 16);
    //double gpuUse = utils->getGPUUsage();
    //double gpuSpanAngle = -270 * gpuUse;
    //gpuGauge->setSpanAngle((int) std::round(gpuSpanAngle) * 16);
    double memoryUse = utils->getMemoryUsage();
    double memorySpanAngle = -270 * memoryUse;
    memoryGauge->setSpanAngle((int) std::round(memorySpanAngle) * 16);

    // Update and realign text for the gauges.
    char temp[48];
    std::sprintf(temp, "CPU: %0.1f%%", cpuUse * 100);
    cpuText->setPlainText(temp);
    cpuText->setX(127 - (cpuText->boundingRect().width() / 2));
    /*std::sprintf(temp, "GPU: %0.1f%%", gpuUse * 100);
    gpuText->setPlainText(temp);
    gpuText->setX(127 - (gpuText->boundingRect().width() / 2));*/
    std::sprintf(temp, "Memory: %0.1f%% (%0.2f GB/%0.2f GB)",
            memoryUse * 100,
            // Convert from kilobytes to gigabytes.
            (double) utils->getMemoryUsageBytes() / 1048576,
            (double) utils->getMemoryTotal() / 1048576);
    memoryText->setPlainText(temp);
    memoryText->setX(127 - (memoryText->boundingRect().width() / 2));

    // Update fan RPM display.
    unsigned short *fanRPM = ec->getFanRPM();
    ui->fan1RPM->display(fanRPM[0]);
    ui->fan2RPM->display(fanRPM[1]);
}

void MainWindow::updateFanMode(unsigned short mode)
{
    if (ec->getFanMode() == mode)
        return;

    QString temp("Fan Modes (current: ");
    if (ec->setFanMode(mode) == 0) {
        if (mode == 0)
            temp.append("Normal)");
        else if (mode == 1)
            temp.append("Quiet)");
        else if (mode == 2)
            temp.append("Gaming)");
        else if (mode == 3)
            temp.append("Custom)");
    } else
        temp.append("DISABLED)");
    ui->fanModeLabel->setText(temp);
}

void MainWindow::updateCustomFanSpeed(unsigned short speed)
{
    if (ec->getCustomFanSpeed() == speed)
        return;

    ec->setCustomFanSpeed(speed);
}
