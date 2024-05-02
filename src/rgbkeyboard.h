#ifndef RGBKEYBOARD_H
#define RGBKEYBOARD_H

#include <QObject>

#include <libusb-1.0/libusb.h>

// This will be used by MainWindow.
struct rgb_settings {
    uint8_t mode;
    uint8_t speed;
    uint8_t brightness;
    uint8_t color;
    uint8_t random;
};

class RGBKeyboard : public QObject
{
    Q_OBJECT
public:
    rgb_settings current;
    RGBKeyboard();
    ~RGBKeyboard();

public slots:
    void setKeyboardRGB(int mode, int speed, int brightness, int color, int random);
    void getCustomModeLayout();

private:
    libusb_device_handle *handle;
    bool keyboardAttached = false, interfaceClaimed = false;

    void setCustomMode(int mode, int brightness);
    int registerKeyboard();
    int getFeatureReport();
};

#endif // RGBKEYBOARD_H
