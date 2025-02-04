#include "ec.h"

#include <QDBusReply>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <cstdio>

EC::EC()
{
    // Check if our kernel driver is running. Easiest way is to find its directory.
    QDir kernelDir("/sys/devices/platform/aorus_laptop");
    if (!kernelDir.exists()) {
        qInfo("Kernel driver not running, some controls will not work!");
        adjustableFanMode = false;
        customFanSpeed = 0;
        fanMode = 0;
        fanRPM[0] = 0;
        fanRPM[1] = 0;
    } else {
        // Find the hwmon directory. Only works if our kernel driver is running.
        // We could hardcode it in, but the number can change on next reboot.
        QStringList filters("hwmon*");
        //QString temp("/sys/devices/platform/gigabyte_laptop/hwmon/");
        QDir dir(kernelDir.absolutePath() + "/hwmon/");
        QStringList dirs = dir.entryList(filters);

        if (dirs.size() > 0)
            hwmonDir = dir.absolutePath() + "/" + dirs.at(0);
        else {
            // Set all fan RPM indices to 0.
            fanRPM[0] = 0;
            fanRPM[1] = 0;
        }

        // Get the fan mode.
        QFile fanFile(kernelDir.absolutePath() + "/fan_mode");
        if (!fanFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            fanMode = 0;
            adjustableFanMode = false;
        } else {
            QTextStream stream(&fanFile);
            fanMode = stream.readLine().toInt();
            adjustableFanMode = true;
        }
        fanFile.close();

        // Get the custom fan speed.
        QFile speedFile(kernelDir.absolutePath() + "/fan_custom_speed");
        if (!speedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            customFanSpeed = 0;
        } else {
            QTextStream stream(&speedFile);
            customFanSpeed = stream.readLine().toInt();
        }
        speedFile.close();

        // Get the charge mode.
        QFile chargeModeFile(kernelDir.absolutePath() + "/charge_mode");
        if (!chargeModeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            chargeMode = 0;
        } else {
            QTextStream stream(&chargeModeFile);
            chargeMode = stream.readLine().toInt();
        }
        chargeModeFile.close();

        // Get the charge limit.
        QFile chargeLimitFile(kernelDir.absolutePath() + "/charge_limit");
        if (!chargeLimitFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            chargeLimit = 100;
        } else {
            QTextStream stream(&chargeLimitFile);
            chargeLimit = stream.readLine().toInt();
        }
        chargeLimitFile.close();
    }

    // Create the timer.
    /*timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(checkEC()));
    // Check every two seconds.
    timer->start(2000);*/
}

EC::~EC()
{
    //timer->stop();
    //delete timer;
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

unsigned short EC::getFanMode()
{
    return fanMode;
}

unsigned short EC::getCustomFanSpeed()
{
    return customFanSpeed;
}

unsigned short EC::getChargeMode()
{
    return chargeMode;
}

unsigned short EC::getChargeLimit()
{
    return chargeLimit;
}

void EC::checkEC()
{
    //qInfo("Checking timer");
}

int EC::setFanMode(unsigned short mode)
{
    // Don't adjust if our kernel driver is not running.
    if (!adjustableFanMode)
        return -1;

    QDBusReply<int> reply = dbus->call("SwitchFanMode", mode);
    if (!reply.isValid()) {
        qInfo("Error: %s", reply.error().message().toStdString().c_str());
        return 0;
    }
    else {
        qInfo("Reply: %d", reply.value());
    }
    fanMode = mode;
    return 0;
}

void EC::setCustomFanSpeed(unsigned short speed)
{
    // Don't adjust if our kernel driver is not running.
    if (!adjustableFanMode)
        return;

    QDBusReply<int> reply = dbus->call("SetFanSpeed", speed);
    if (!reply.isValid()) {
        qInfo("Error: %s", reply.error().message().toStdString().c_str());
        return;
    }
    else {
        qInfo("Reply: %d", reply.value());
    }
    customFanSpeed = speed;
}

void EC::setChargeMode(unsigned short mode)
{
    QDBusReply<int> reply = dbus->call("SetChargeMode", mode);
    if (!reply.isValid()) {
        qInfo("Error: %s", reply.error().message().toStdString().c_str());
        return;
    }
    else {
        qInfo("Reply: %d", reply.value());
    }
}

void EC::setChargeLimit(unsigned short limit)
{
    QDBusReply<int> reply = dbus->call("SetChargeLimit", limit);
    if (!reply.isValid()) {
        qInfo("Error: %s", reply.error().message().toStdString().c_str());
        return;
    }
    else {
        qInfo("Reply: %d", reply.value());
    }
}

void EC::setDBus(QDBusInterface *dbus)
{
    this->dbus = dbus;
}
