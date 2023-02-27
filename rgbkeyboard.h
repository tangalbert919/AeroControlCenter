#ifndef RGBKEYBOARD_H
#define RGBKEYBOARD_H

#include <QObject>

#include <libusb-1.0/libusb.h>

class RGBKeyboard : public QObject
{
    Q_OBJECT
public:
    RGBKeyboard();
    ~RGBKeyboard();

private:
    libusb_device_handle *handle;
    bool keyboardAttached = false, interfaceClaimed = false;

    int registerKeyboard();
    int getFeatureReport();
};

#endif // RGBKEYBOARD_H
