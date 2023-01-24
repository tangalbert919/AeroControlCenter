#ifndef UTILS_H
#define UTILS_H

#include <QTimer>

class Utils : public QObject
{
    Q_OBJECT
public:
    Utils();
    QTimer *timer;
    double getCPUUsage();
    double getGPUUsage();
    double getMemoryUsage();
    unsigned long long getMemoryUsageBytes();

public slots:
    void updateStats();

private:
    long long totalPhysMem, physMemUsed;
    unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    double cpuUsage = 0, gpuUsage = 0;

    double updateCPUUsage();
    double updateGPUUsage();
};

#endif // UTILS_H
