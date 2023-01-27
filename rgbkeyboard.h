#ifndef RGBKEYBOARD_H
#define RGBKEYBOARD_H

#include <QObject>

#include <hidapi/hidapi.h>

class RGBKeyboard : public QObject
{
    Q_OBJECT
public:
    RGBKeyboard();
    ~RGBKeyboard();

private:
    hid_device *handle;
    struct hid_device_info *info;
};

#endif // RGBKEYBOARD_H
