#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cmath>
#include <QMessageBox>
#include <QFile>
#include <QTabBar>
#include <QDBusReply>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Detect if program is running on an Aero or AORUS machine.
    // Otherwise, create a popup and exit.
    QFile dmi("/sys/class/dmi/id/product_family"); // This should NOT fail.
    if (dmi.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&dmi);
        QString model(stream.readAll());
        dmi.close();
        if (model != "AERO\n" && model != "AORUS\n") {
            QMessageBox msgBox;
            msgBox.setWindowTitle("WARNING");
            msgBox.setText("This program will only run on an Aero or AORUS machine.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
            parent->close();
        }
    }

    // Setup hardware monitor.
    utils = new HWMonitor();

    // Setup embedded controller.
    ec = new EC();

    // Setup RGB keyboard.
    rgb = new RGBKeyboard();

    // Setup initial UI.
    ui->setupUi(this);

    // Setup the menus for the menubar.
    setupMenu();

    // Set UI to first tab. Useful when we are doing UI changes and forget to set currentIndex to 0.
    ui->tabWidget->setCurrentIndex(0);

    // Configure tab bar. Make sure tabs span the application width.
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);

    // Setup fan RPM display.
    ui->fan1RPM->display(0);
    ui->fan2RPM->display(0);

    // Setup the fan speed controls. Only effective in custom mode.
    connect(ui->fanCustomSlider, &QSlider::sliderReleased, this, &MainWindow::updateSliderPosition);
    connect(ui->fanCustomSlider, &QSlider::valueChanged, this,
            [=]() { this->printSliderPosition(0); });
    connect(ui->setFanSpeedBtn, &QPushButton::clicked, this,
            [=]() { this->updateCustomFanSpeed(ui->fanCustomSlider->value()); });

    // Setup charging limit control.
    connect(ui->chargeLimitSlider, &QSlider::valueChanged, this,
            [=]() { this->printSliderPosition(1); });

    // Disable controls if kernel driver is not running. Just checking custom fan speed will do.
    if (ec->getCustomFanSpeed() == 0) {
        ui->fanCustomSlider->setDisabled(true);
        ui->setFanSpeedBtn->setDisabled(true);
        ui->chargeLimitSlider->setDisabled(true);
    } else {
        ui->fanCustomSlider->setValue(ec->getCustomFanSpeed());
        ui->chargeLimitSlider->setValue(ec->getChargeLimit());
        ui->fanCustomPercent->setText(QString::number(ec->getCustomFanSpeed()) + "%");
        ui->chargeLimitLabel->setText(QString::number(ec->getChargeLimit()) + "%");
    }
    // Hide fan controls if not on custom mode.
    if (ec->getFanMode() < 3) {
        ui->fanCustomSlider->setVisible(false);
        ui->fanCustomPercent->setVisible(false);
        ui->setFanSpeedBtn->setVisible(false);
    }
    // Hide charging limit if not on custom mode.
    if (ec->getChargeMode() < 1) {
        ui->chargeLimitLabel->setVisible(false);
        ui->chargeLimitSlider->setVisible(false);
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

    // Hook up charging mode buttons.
    connect(ui->chargeCustomBtn, &QPushButton::clicked, this,
            [=]() { this->updateChargingMode(1); });
    connect(ui->chargeDefaultBtn, &QPushButton::clicked, this,
            [=]() { this->updateChargingMode(0); });

    // Setup RGB controls.
    setupRGB();

    // Setup CPU, GPU, and memory views and gauges.
    setupGauges();

    // Get CPU info.
    getCPUInfo();

    // Setup D-Bus.
    setupDBus();

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
    delete utils;
    delete rgb;
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

void MainWindow::setupDBus()
{
    dbus = new QDBusInterface("com.gigabyte.daemon", "/com/gigabyte/Platform",
                              "com.gigabyte.Platform", QDBusConnection::systemBus());
    ec->setDBus(dbus);
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

void MainWindow::setupRGB()
{
    connect(ui->rgbSetBtn, &QPushButton::clicked, rgb, [=]() {
        int mode = ui->rgbModes->currentIndex() + ui->rgbCustomModes->currentIndex();
        int speed = ui->rgbSpeedSlider->value();
        int brightness = ui->rgbBrightnessSlider->value();
        int color = ui->rgbColors->currentIndex();
        int random = ui->rgbRandom->isChecked();
        bool lightbar = ui->lightbarToggle->isChecked();
        if (rgb->getCustomModeSupport() == RGB_THREEZONE && mode > 12) {
            rgb->setKeyboardRGB(20, speed, brightness, color, random, lightbar);
            rgb->setZoneRGB(3, ui->leftZoneR->value(), ui->leftZoneG->value(), ui->leftZoneB->value());
            rgb->setZoneRGB(4, ui->centerZoneR->value(), ui->centerZoneG->value(), ui->centerZoneB->value());
            rgb->setZoneRGB(5, ui->rightZoneR->value(), ui->rightZoneG->value(), ui->rightZoneB->value());
        }
        else
            rgb->setKeyboardRGB(mode, speed, brightness, color, random, lightbar);
    });

    rgbView = new RGBGraphicsView(ui->rgbTab);
    rgbView->setFixedSize(780,242);
    rgbView->move(0,10);
    rgbScene = new QGraphicsScene(this);
    rgbView->setScene(rgbScene);
    rgbScene->setSceneRect(0,0,760,240);
    rgbView->setupLayout();

    rgbView->bindKeyboard(rgb);

    connect(ui->rgbModes, &QComboBox::currentIndexChanged,
            rgbView, &RGBGraphicsView::changeMode);
    connect(ui->rgbModes, &QComboBox::currentIndexChanged, rgbView, [=]() {
        // hide color selection on wave, neon, rainbow marquee and raindrop
        if (ui->rgbModes->currentIndex() == 2 ||
            (ui->rgbModes->currentIndex() >= 7 &&
            ui->rgbModes->currentIndex() <= 9)) {
            ui->rgbColors->setHidden(true);
            ui->rgbRandom->setHidden(true);
        }
        else {
            ui->rgbColors->setHidden(false);
            ui->rgbRandom->setHidden(false);
        }
        if (ui->rgbModes->currentIndex() < 13) {
            if (rgb->getCustomModeSupport() == RGB_THREEZONE)
                ui->rgbZoneGroup->setHidden(true);
            else
                ui->rgbCustomModes->setHidden(true);
        }
        else {
            if (rgb->getCustomModeSupport() == RGB_THREEZONE)
                ui->rgbZoneGroup->setHidden(false);
            else
                ui->rgbCustomModes->setHidden(false);
        }
    });
    connect(ui->rgbCustomModes, &QComboBox::currentIndexChanged, rgbView, [=]() {
        rgbView->changeMode(ui->rgbCustomModes->currentIndex() + 13);
    });
    connect(ui->rgbColors, &QComboBox::currentIndexChanged,
            rgbView, &RGBGraphicsView::changeColors);
    connect(ui->rgbRandom, &QCheckBox::checkStateChanged, ui->rgbColors, &QComboBox::setDisabled);
    connect(ui->rgbBlueBox, &QSpinBox::valueChanged, rgbView, [=]() {
        rgbView->adjustBrush(ui->rgbBlueBox->value(), 2);
    });
    connect(ui->rgbGreenBox, &QSpinBox::valueChanged, rgbView, [=]() {
        rgbView->adjustBrush(ui->rgbGreenBox->value(), 1);
    });
    connect(ui->rgbRedBox, &QSpinBox::valueChanged, rgbView, [=]() {
        rgbView->adjustBrush(ui->rgbRedBox->value(), 0);
    });

    if (rgb->getCustomModeSupport() == RGB_THREEZONE)
        ui->rgbCustomModes->setHidden(true);
    else
        ui->rgbZoneGroup->setHidden(true);

    if (rgb->keyboard_rgb.mode > 12) {
        ui->rgbModes->setCurrentIndex(13);
        if (rgb->getCustomModeSupport() == RGB_THREEZONE)
            ui->rgbZoneGroup->setHidden(false);
        else {
            ui->rgbCustomModes->setCurrentIndex(rgb->keyboard_rgb.mode - 13);
            ui->rgbCustomModes->setHidden(false);
        }
    }
    else {
        ui->rgbModes->setCurrentIndex(rgb->keyboard_rgb.mode);
        if (rgb->getCustomModeSupport() == RGB_THREEZONE)
            ui->rgbZoneGroup->setHidden(true);
        else
            ui->rgbCustomModes->setHidden(true);
    }
    if (rgb->keyboard_rgb.color < 0) // For modes not requiring color selection
        ui->rgbColors->setCurrentIndex(0);
    else
        ui->rgbColors->setCurrentIndex(rgb->keyboard_rgb.color);
    ui->rgbRandom->setChecked(rgb->keyboard_rgb.random);
    ui->rgbBrightnessSlider->setSliderPosition(rgb->keyboard_rgb.brightness);
    ui->rgbSpeedSlider->setSliderPosition(rgb->keyboard_rgb.speed);
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

void MainWindow::printSliderPosition(unsigned short slider)
{
    unsigned short speed;
    if (slider == 0) {
        speed = ui->fanCustomSlider->sliderPosition();
        ui->fanCustomPercent->setText(QString::number(speed) + "%");
    }
    else {
        speed = ui->chargeLimitSlider->sliderPosition();
        ui->chargeLimitLabel->setText(QString::number(speed) + "%");
    }
}

void MainWindow::updateSliderPosition()
{
    unsigned short speed = ui->fanCustomSlider->sliderPosition();
    unsigned short remainder = speed % 5;
    if (remainder != 0) {
        speed = speed - remainder;
        ui->fanCustomSlider->setValue(speed);
    }
    printSliderPosition(0);
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
    QString qCpu("CPU: %1%");
    qCpu = qCpu.arg(cpuUse * 100, 0, 'f', 1);
    cpuText->setPlainText(qCpu);
    cpuText->setX(127 - (cpuText->boundingRect().width() / 2));
    /*QString qGpu("GPU: %1%");
    qGpu = qGpu.arg(gpuUse * 100, 0, 'f', 1);
    gpuText->setX(127 - (gpuText->boundingRect().width() / 2));*/
    QString qMem("Memory: %1% (%2 GB/%3 GB)");
    qMem = qMem.arg(memoryUse * 100, 0, 'f', 1)
            // Convert from kilobytes to gigabytes.
            .arg((double) utils->getMemoryUsageBytes() / 1048576, 0, 'f', 2)
            .arg((double) utils->getMemoryTotal() / 1048576, 0, 'f', 2);
    memoryText->setPlainText(qMem);
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

void MainWindow::updateChargingMode(unsigned short mode)
{
    if (ec->getChargeMode() == mode)
        return;

    ec->setChargeMode(mode);
}

void MainWindow::updateChargingLimit(unsigned short limit)
{
    if (ec->getChargeLimit() == limit)
        return;

    ec->setChargeLimit(limit);
}
