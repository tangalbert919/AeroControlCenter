#include "ec.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <cstdio>

EC::EC()
{
    // Find the hwmon directory. Only works if our kernel driver is running.
    // We could hardcode it in, but the number can change on next reboot.
    QStringList filters("hwmon*");
    QString temp("/sys/devices/platform/gigabyte_laptop/hwmon/");
    QDir dir(temp);
    QStringList dirs = dir.entryList(filters);

    if (dirs.size() > 0)
        hwmonDir = temp.append(dirs.at(0));
    else
        qInfo("Kernel driver not running, some controls will not work!");

    // Create the timer.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(checkEC()));
    // Check every two seconds.
    timer->start(2000);
}

EC::~EC()
{
    timer->stop();
    delete timer;
}

unsigned short *EC::getFanRPM()
{
    // Don't check fan RPM if our kernel driver is not running.
    if (hwmonDir.isNull())
        return fanRPM;

    // Create copies of hwmonDir, and then append those copies.
    QString qstrone(hwmonDir);
    qstrone.append("/fan1_input");
    QFile qfanone(qstrone);
    if (!qfanone.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug("failed");
    QTextStream inone(&qfanone);
    QString line = inone.readLine();
    fanRPM[0] = line.toInt();
    qfanone.close();

    QString qstrtwo(hwmonDir);
    qstrtwo.append("/fan2_input");
    QFile qfantwo(qstrtwo);
    if (!qfantwo.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug("failed");
    QTextStream intwo(&qfantwo);
    line = intwo.readLine();
    fanRPM[1] = line.toInt();
    qfantwo.close();
    return fanRPM;
}

void EC::checkEC()
{
    qInfo("Checking timer");
}

void EC::setFanMode(unsigned int mode)
{
    // TODO: Figure out polkit. This only works if running as root!
    char buf[2];
    QFile qf("/sys/devices/platform/gigabyte_laptop/fan_mode");
    if (!qf.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug("failed");
    std::sprintf(buf, "%d", mode);
    qf.write(buf);
    qf.close();
}
