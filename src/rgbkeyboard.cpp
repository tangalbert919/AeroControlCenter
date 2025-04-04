#include "rgbkeyboard.h"

#include "data.h"
#include <QFile>
#include <QDataStream>

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
        if (desc.idVendor == CHU_YUEN || (desc.idVendor == GIGABYTE && desc.idProduct == 0x8004)) {
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

    // Get current keyboard configuration
    res = getFeatureReport();
    if (res) {
        qInfo("Failed to get current configuration");
    }
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

void RGBKeyboard::setKeyboardRGB(int mode, int speed, int brightness, int color, int random)
{
    packet packet;
    uint16_t chksum = 0;
    uint8_t *data = (uint8_t*) &packet;
    int res;

    if (!keyboardAttached) {
        qInfo("RGB is not supported on this machine");
        return;
    }

    packet.instruction = RGB_MODE;
    packet.reserved = 0x00;
    if (mode <= 12)
        packet.mode = mode + 1;
    else
        packet.mode = mode + 38;
    packet.speed = 11 - speed; // reverse, slowest is 0x0A
    packet.brightness = brightness * 25;
    if (random)
        packet.color = RANDOM;
    else
        packet.color = color + 1;
    packet.offset = 0x01; // does nothing
    for (int i = 0; i < 7; i++)
        chksum += data[i];
    packet.checksum = (uint8_t)(0xFF - (chksum & 0xFF));

    res = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0)
        qWarning("Failed to set RGB");

    if (mode > 12)
        setCustomMode(mode, brightness);
    current.brightness = brightness;
    current.mode = mode;
    current.speed = speed;
    current.color = color;
    current.random = random;
}

void RGBKeyboard::getCustomModeLayout()
{
    // Set keyboard mode to read config for custom mode
    packet packet;
    int res;

    packet.instruction = RGB_READCONFIG;
    packet.checksum = (0xFF - (RGB_READCONFIG & 0xFF));
    res = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to enter report reading mode");
        return;
    }
    // Call IN endpoint to start the transfer
    res = libusb_control_transfer(handle, 0xA1, 0x01, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to start transfer");
        return;
    }
    for (uint8_t i = 0; i < 8; i++) {
        int transferred = 0;
        res = libusb_interrupt_transfer(handle, 0x85, m_white_data + (i * 64), 64, &transferred, 50);
        if (res < 0 || transferred != 64)
            qInfo("Interrupt transfer failed at index %d: ret is %d, transferred is %d", i, res, transferred);
    }
    // Debugging purposes.
    QFile debugFile("keyboard_data.txt");
    if (!debugFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qInfo("Failed to open file");
    }
    QDataStream out(&debugFile);
    for (uint8_t data : m_white_data) {
        out << data;
    }
}

void RGBKeyboard::setCustomMode(int mode, int brightness)
{
    packet packet;
    uint16_t chksum = 0;
    uint8_t *data = (uint8_t*) &packet;
    int res;

    packet.instruction = RGB_PROGRAM;
    packet.reserved = 0x00;
    packet.mode = mode - 13; // Custom Profile index is zero-based
    packet.speed = 0x00;
    packet.brightness = 0x00;
    packet.color = 0x00;
    packet.offset = 0x00; // does nothing
    for (int i = 0; i < 7; i++)
        chksum += data[i];
    packet.checksum = (uint8_t)(0xFF - (chksum & 0xFF));

    res = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0)
        qWarning("Failed to enter programming mode");

    // TODO: Push custom data instead
    for (uint8_t i = 0; i < 8; i++) {
        int transferred = 0;
        res = libusb_interrupt_transfer(handle, 0x06, m_white_data + (i * 64), 64, &transferred, 0);
        if (res < 0 || transferred != 64)
            qInfo("Interrupt transfer failed");
    }

    // Select custom profile again
    packet.instruction = RGB_MODE;
    packet.mode = mode + 38;
    packet.brightness = brightness * 25;
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
    packet packet;
    uint16_t chksum = 0;
    int res;

    packet.instruction = RGB_GETREPORT;
    packet.checksum = (uint8_t)(0xFF - (RGB_GETREPORT & 0xFF));

    res = libusb_control_transfer(handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to enter report reading mode");
        goto done;
    }
    // Now we can get the report from the input endpoint.
    res = libusb_control_transfer(handle, 0xA1, 0x01, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to obtain report");
        goto done;
    }
    // Send needed data to set initial RGB UI settings
    if (packet.mode >= CUSTOM_ONE) {
        current.mode = packet.mode - 38;
        current.color = packet.color;
    } else {
        current.mode = packet.mode - 1;
        current.color = packet.color - 1;
    }
    current.brightness = packet.brightness / 25;
    current.speed = 11 - packet.speed;
    current.random = packet.color & 0x08;

    if (packet.mode >= CUSTOM_ONE) {
        getCustomModeLayout();
    }

done:
    qInfo("RGB finished, got %d", packet.mode);
    return 0;
}
