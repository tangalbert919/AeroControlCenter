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
    rgb_settings keyboard_rgb;
    rgb_settings lightbar_rgb;
    RGBKeyboard();
    ~RGBKeyboard();

public slots:
    void setKeyboardRGB(int mode, int speed, int brightness, int color, int random);
    void setKeyboardRGB(int mode, int speed, int brightness, int color, int random, bool lightbar);
    void setKeyRGB(int index, int r, int g, int b);
    void getCustomModeLayout(int mode);
    uint8_t *getKeyRGB();

private:
    libusb_device_handle *keyboard_handle, *light_bar;
    bool keyboardAttached = false, keyboardClaimed = false;
    bool lightbarAttached = false, lightbarClaimed = false;

    void setCustomMode(int mode, int brightness);
    int getFeatureReport();
    int getLightbarReport();
};

#endif // RGBKEYBOARD_H
