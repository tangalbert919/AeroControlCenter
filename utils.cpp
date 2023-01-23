#include "utils.h"

#include "sys/sysinfo.h"

#include <bits/types/FILE.h>
#include <cstdio>

struct sysinfo memInfo;

Utils::Utils()
{
    // Load initial values to populate main window later.
    init();
    sysinfo(&memInfo);

    // Get total RAM installed and available.
    totalPhysMem = memInfo.totalram;
    totalPhysMem *= memInfo.mem_unit;
    physMemUsed = memInfo.totalram - memInfo.freeram;
    physMemUsed *= memInfo.mem_unit;

    // Start timer.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(updateStats()));
    timer->start(2000);
}

double Utils::getCpuUsage() {
    return cpuUsage;
}

// TODO: Return percentage instead of bytes.
unsigned long long Utils::getMemoryUsage() {
    return physMemUsed;
}

void Utils::updateStats() {
    //printf("test utils\n");
    //printf("%llu\n", physMemUsed);
    //printf("%f\n", cpuUsage);
    cpuUsage = getCurrentValue();
}

// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
//static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void Utils::init()
{
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);
}

double Utils::getCurrentValue()
{
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else {
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}
