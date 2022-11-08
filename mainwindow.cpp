#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->fan1RPM->setDigitCount(4);
    ui->fan2RPM->setDigitCount(4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

