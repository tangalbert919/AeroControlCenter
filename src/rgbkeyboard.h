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

enum rgb_support {
    RGB_NONE = 0,
    RGB_ONEZONE = 1,
    RGB_THREEZONE = 2,
    RGB_PERKEY = 3
};

class RGBKeyboard : public QObject
{
    Q_OBJECT
public:
    rgb_settings keyboard_rgb;
    rgb_settings lightbar_rgb;
    RGBKeyboard();
    ~RGBKeyboard();

public slots:
    void setKeyboardRGB(int mode, int speed, int brightness, int color, int random);
    void setKeyboardRGB(int mode, int speed, int brightness, int color, int random, bool lightbar);
    void setKeyRGB(int index, int r, int g, int b);
    void setZoneRGB(int zone, int r, int g, int b);
    void getCustomModeLayout(int mode);
    int getCustomModeSupport();
    uint8_t *getKeyRGB();

private:
    libusb_device_handle *keyboard_handle, *light_bar;
    bool keyboardAttached = false, keyboardClaimed = false;
    bool lightbarAttached = false, lightbarClaimed = false;
    int rgb_support_level;

    void setCustomMode(int mode, int brightness);
    void setRGBSupport(uint16_t idVendor, uint16_t idProduct);
    int getFeatureReport();
    int getLightbarReport();
};

#endif // RGBKEYBOARD_H
