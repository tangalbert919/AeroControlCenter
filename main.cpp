#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Aero Control Center");
    QCoreApplication::setApplicationVersion(BUILD_VERSION);

    MainWindow window;
    window.show();
    return app.exec();
}
