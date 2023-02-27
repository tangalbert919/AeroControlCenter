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
