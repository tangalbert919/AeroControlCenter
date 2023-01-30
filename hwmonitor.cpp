#include "hwmonitor.h"

#include "sys/sysinfo.h"

#include <bits/types/FILE.h>
#include <cstdio>

struct sysinfo memInfo;

HWMonitor::HWMonitor()
{
    // Get total RAM installed and available.
    sysinfo(&memInfo);
    totalPhysMem = memInfo.totalram;
    totalPhysMem *= memInfo.mem_unit;
    physMemUsed = memInfo.totalram - memInfo.freeram;
    physMemUsed *= memInfo.mem_unit;

    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);

    // Start timer.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updateStats()));
    timer->start(2000);
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
    return (double) physMemUsed / (double) totalPhysMem;
}

unsigned long long HWMonitor::getMemoryUsageBytes()
{
    return physMemUsed;
}

unsigned long long HWMonitor::getMemoryTotal()
{
    return totalPhysMem;
}

void HWMonitor::updateStats()
{
    physMemUsed = memInfo.totalram - memInfo.freeram;
    physMemUsed *= memInfo.mem_unit;
    cpuUsage = updateCPUUsage();
    gpuUsage = updateGPUUsage();
}

// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
double HWMonitor::updateCPUUsage()
{
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

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
