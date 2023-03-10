#include "rgbkeyboard.h"

#include "data.h"

RGBKeyboard::RGBKeyboard()
{
    int res;
    libusb_device **list;
    libusb_device *found = NULL;
    struct libusb_device_descriptor desc = {0};
    ssize_t cnt = 0;

    // Start libusb. Must ALWAYS happen first.
    res = libusb_init(NULL);
    if (res) {
        qInfo("Could not initialize libusb!");
        goto done;
    }

    cnt = libusb_get_device_list(NULL, &list);
    if (cnt < 0)
        goto done;

    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device *device = list[i];
        res = libusb_get_device_descriptor(device, &desc);
        if (res)
            goto done;
        if (desc.idVendor == CHU_YUEN) {
            found = device;
            break;
        }
    }

    if (found) {
        res = libusb_open(found, &handle);
        if (res) {
            qInfo("Could not open device");
            goto done;
        }
        keyboardAttached = true;
    } else
        goto done;

    libusb_free_device_list(list, 1);

    res = libusb_set_auto_detach_kernel_driver(handle, 1);
    if (res) {
        qInfo("Could not enable automatic kernel driver detachment");
        goto done;
    }

    // Claim interface 3. This interface handles RGB.
    res = libusb_claim_interface(handle, 3);
    if (res) {
        qInfo("Failed to claim interface");
        goto done;
    }
    interfaceClaimed = true;
done:
    qInfo("RGB keyboard init finished");
}

RGBKeyboard::~RGBKeyboard()
{
    // Shutdown libusb. Must ALWAYS happen when the application closes.
    if (interfaceClaimed) {
        int res = libusb_release_interface(handle, 3);
        if (res)
            qInfo("Failed to release interface");
    }
    if (keyboardAttached) {
        libusb_close(handle);
    }
    libusb_exit(NULL);
}

void RGBKeyboard::setKeyboardRGB(int mode, int speed, int brightness, int color)
{
    packet packet;
    uint16_t chksum = 0;
    uint8_t *data = (uint8_t*) &packet;
    int res;

    packet.instruction = RGB_MODE;
    packet.reserved = 0x00;
    packet.mode = mode + 1;
    packet.speed = speed;
    packet.brightness = brightness * 25;
    packet.color = color + 1;
    packet.offset = 0x01; // does nothing
    for (int i = 0; i < 7; i++)
        chksum += data[i];
    packet.checksum = (uint8_t)(0xFF - (chksum & 0xFF));

    res = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0)
        qWarning("Failed to set RGB");
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
