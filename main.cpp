#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Aero Control Center");
    QCoreApplication::setApplicationVersion("0.0.1");

    MainWindow window;
    window.show();
    return app.exec();
}
