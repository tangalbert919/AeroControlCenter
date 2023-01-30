#include "ec.h"

#include <cstdio>

EC::EC()
{
    // Create the timer.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(checkEC()));
    // Check every two seconds.
    timer->start(2000);
}

unsigned short *EC::getFanRPM()
{
    return fanRPM;
}

void EC::checkEC()
{
    printf("Checking Timer\n");
    // This should be in modprobe.d
    //system("modprobe ec_sys write_support=1");
    // TODO: Get this working without the need for root access.
    FILE *fp = std::fopen("/sys/kernel/debug/ec/ec0/io", "r+");
    fanRPM[0] = read16(fp, 0xFC);
    fanRPM[1] = read16(fp, 0xFE);
}

// Much of the code below is derived from here: https://github.com/jertel/p37-ec
// It has been adapted for C++.
unsigned short EC::read16(FILE* f, unsigned char offset)
{
    std::fseek(f, offset, SEEK_SET);
    unsigned short s = 0;
    std::fread(&s, sizeof(unsigned short), 1, f);
    return htobe16(s);
}
