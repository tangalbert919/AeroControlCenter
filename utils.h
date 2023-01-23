#ifndef UTILS_H
#define UTILS_H

#include <QTimer>

class Utils : public QObject
{
    Q_OBJECT
public:
    Utils();
    QTimer *timer;
    double getCpuUsage();
    unsigned long long getMemoryUsage();

public slots:
    void updateStats();

private:
    long long totalPhysMem, physMemUsed;
    unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    double cpuUsage = 0;

    void init();
    double getCurrentValue();
};

#endif // UTILS_H
