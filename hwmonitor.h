#ifndef HWMONITOR_H
#define HWMONITOR_H

#include <QTimer>

class HWMonitor : public QObject
{
    Q_OBJECT
public:
    HWMonitor();
    ~HWMonitor();

    double getCPUUsage();
    double getGPUUsage();
    double getMemoryUsage();
    unsigned long long getMemoryUsageBytes();
    unsigned long long getMemoryTotal();

public slots:
    void updateStats();

private:
    QTimer *timer;

    long long physMemTotal, physMemUsed, physMemAvailable;
    unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    double cpuUsage = 0, gpuUsage = 0;

    double updateCPUUsage();
    double updateGPUUsage();
};

#endif // UTILS_H
