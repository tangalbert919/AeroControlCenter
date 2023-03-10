#include "hwmonitor.h"

#include <bits/types/FILE.h>
#include <cstdio>

HWMonitor::HWMonitor()
{
    // Get inital CPU stats
    FILE *file = std::fopen("/proc/stat", "r");
    std::fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    std::fclose(file);

    // Get total RAM installed and available.
    file = std::fopen("/proc/meminfo", "r");
    std::fscanf(file, "MemTotal: %llu kB\nMemFree: %llu kB\nMemAvailable: %llu kB", &physMemTotal, &physMemUsed, &physMemAvailable);
    std::fclose(file);

    // Start timer.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updateStats()));
    timer->start(2000);
}

HWMonitor::~HWMonitor()
{
    timer->stop();
    delete timer;
}

double HWMonitor::getCPUUsage()
{
    return cpuUsage;
}

double HWMonitor::getGPUUsage()
{
    return gpuUsage;
}

double HWMonitor::getMemoryUsage()
{
    // Cast to double, or method returns 0.
    return (double) (physMemTotal - physMemAvailable) / (double) physMemTotal;
}

unsigned long long HWMonitor::getMemoryUsageBytes()
{
    return physMemTotal - physMemAvailable;
}

unsigned long long HWMonitor::getMemoryTotal()
{
    return physMemTotal;
}

void HWMonitor::updateStats()
{
    FILE *file = std::fopen("/proc/meminfo", "r");
    std::fscanf(file, "MemTotal: %llu kB\nMemFree: %llu kB\nMemAvailable: %llu kB", &physMemTotal, &physMemUsed, &physMemAvailable);
    std::fclose(file);
    cpuUsage = updateCPUUsage();
    gpuUsage = updateGPUUsage();
}

// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
double HWMonitor::updateCPUUsage()
{
    double percent;
    FILE *file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = std::fopen("/proc/stat", "r");
    std::fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    std::fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else {
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        //percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

double HWMonitor::updateGPUUsage()
{
    // TODO: Implement GPU usage.
    return 0;
}
