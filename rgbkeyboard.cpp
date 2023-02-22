#include "rgbkeyboard.h"

RGBKeyboard::RGBKeyboard()
{
    int res;
    libusb_device **list;
    libusb_device *found = NULL;
    struct libusb_device_descriptor desc = {0};
    ssize_t cnt = 0;

    res = libusb_init(NULL);
    if (res) {
        qInfo("Could not initialize libusb!");
    }

done:
    qInfo("RGB keyboard init finished");
}

RGBKeyboard::~RGBKeyboard()
{
    // TODO: Reimplement with libusb
    if (keyboardAttached) {
        libusb_close(handle);
        libusb_exit(NULL);
    }
}

int RGBKeyboard::registerKeyboard()
{
    // TODO: Reimplement with libusb
    return 0;
}

int RGBKeyboard::getFeatureReport()
{
    // TODO: Implement
    return 0;
}
