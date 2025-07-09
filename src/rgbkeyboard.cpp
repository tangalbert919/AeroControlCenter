#include "rgbkeyboard.h"

#include "data.h"
#include <QFile>
#include <QDataStream>

RGBKeyboard::RGBKeyboard()
{
    int res;
    libusb_device **list;
    libusb_device *keyboard_found = NULL;
    libusb_device *lightbar_found = NULL;
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
            keyboard_found = device;
            setRGBSupport(desc.idVendor, desc.idProduct);
            continue;
        }
        if (desc.idVendor == GIGABYTE && desc.idProduct == 0x8011) {
            lightbar_found = device;
            if (keyboard_found != NULL) {
                break;
            }
            continue;
        }
    }

    if (keyboard_found) {
        res = libusb_open(keyboard_found, &keyboard_handle);
        if (res) {
            qInfo("Could not open keyboard");
            goto done;
        }
        keyboardAttached = true;
    } else
        goto done;

    // Present on some AORUS models.
    if (lightbar_found) {
        res = libusb_open(lightbar_found, &light_bar);
        if (res) {
            qInfo("Could not open light bar");
            goto done;
        }
        lightbarAttached = true;
    }

    libusb_free_device_list(list, 1);

    res = libusb_set_auto_detach_kernel_driver(keyboard_handle, 1);
    if (res) {
        qInfo("Could not enable automatic kernel driver detachment");
        goto done;
    }

    if (lightbarAttached) {
        res = libusb_set_auto_detach_kernel_driver(light_bar, 1);
        if (res) {
            qInfo("Could not enable automatic kernel driver detachment from light bar");
            goto done;
        }
    }

    // Claim interface 3. This interface handles RGB.
    res = libusb_claim_interface(keyboard_handle, 3);
    if (res) {
        qInfo("Failed to claim keyboard RGB interface");
        goto done;
    }
    keyboardClaimed = true;

    if (lightbarAttached) {
        res = libusb_claim_interface(light_bar, 3);
        if (res) {
            qInfo("Failed to claim light bar interface");
            goto done;
        }
        lightbarClaimed = true;
    }

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
    if (keyboardClaimed) {
        int res = libusb_release_interface(keyboard_handle, 3);
        if (res)
            qInfo("Failed to release keyboard interface");
    }
    if (keyboardAttached) {
        libusb_close(keyboard_handle);
    }
    if (lightbarClaimed) {
        int res = libusb_release_interface(light_bar, 3);
        if (res)
            qInfo("Failed to release light bar interface");
    }
    if (lightbarAttached) {
        libusb_close(light_bar);
    }
    libusb_exit(NULL);
}

void RGBKeyboard::setKeyboardRGB(int mode, int speed, int brightness, int color, int random)
{
    setKeyboardRGB(mode, speed, brightness, color, random, false);
}

void RGBKeyboard::setKeyboardRGB(int mode, int speed, int brightness, int color, int random, bool lightbar)
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
    if (!lightbar)
        packet.reserved = 0x00;
    else
        packet.reserved = 0x02;
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

    res = libusb_control_transfer(keyboard_handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0)
        qWarning("Failed to set RGB");

    if (mode > 12)
        setCustomMode(mode, brightness);
    keyboard_rgb.brightness = brightness;
    keyboard_rgb.mode = mode;
    keyboard_rgb.speed = speed;
    keyboard_rgb.color = color;
    keyboard_rgb.random = random;
}

void RGBKeyboard::setKeyRGB(int index, int r, int g, int b)
{
    m_white_data[index*4] = 0x00;
    m_white_data[index*4+1] = r;
    m_white_data[index*4+2] = g;
    m_white_data[index*4+3] = b;
}

void RGBKeyboard::getCustomModeLayout(int mode)
{
    // Set keyboard mode to read config for custom mode
    packet packet;
    int res;

    packet.instruction = RGB_READCONFIG;
    packet.mode = mode - 13;
    packet.offset = 0x01;
    packet.checksum = (0xFF - ((RGB_READCONFIG - packet.mode - packet.offset) & 0xFF));
    res = libusb_control_transfer(keyboard_handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to enter report reading mode");
        return;
    }
    // Call IN endpoint to start the transfer
    res = libusb_control_transfer(keyboard_handle, 0xA1, 0x01, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to start transfer");
        return;
    }
    for (uint8_t i = 0; i < 8; i++) {
        int transferred = 0;
        res = libusb_interrupt_transfer(keyboard_handle, 0x85, m_white_data + (i * 64), 64, &transferred, 50);
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

int RGBKeyboard::getCustomModeSupport()
{
    return rgb_support_level;
}

uint8_t *RGBKeyboard::getKeyRGB()
{
    return m_white_data;
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

    res = libusb_control_transfer(keyboard_handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0)
        qWarning("Failed to enter programming mode");

    // TODO: Push custom data instead
    for (uint8_t i = 0; i < 8; i++) {
        int transferred = 0;
        res = libusb_interrupt_transfer(keyboard_handle, 0x06, m_white_data + (i * 64), 64, &transferred, 0);
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

    res = libusb_control_transfer(keyboard_handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0)
        qWarning("Failed to set RGB");
}

void RGBKeyboard::setRGBSupport(uint16_t idVendor, uint16_t idProduct)
{
    // TODO: implement
    if (idVendor == GIGABYTE) {
        switch (idProduct) {
        case 0x7a3f: // 17H BXF / 17X AXF
        case 0x7a43: // 15 BKF
        case 0x8004: // 15 BMG/BKG
            rgb_support_level = RGB_THREEZONE;
        default:
            rgb_support_level = RGB_PERKEY;
        }
    }
    else if (idVendor == CHU_YUEN) {
        switch (idProduct) {
            // GET CASES IN
        case 0x7a3f: // 17 YE5
            rgb_support_level = RGB_THREEZONE;
        default:
            rgb_support_level = RGB_PERKEY;
        }
    }
}

int RGBKeyboard::getFeatureReport()
{
    packet packet;
    uint16_t chksum = 0;
    int res;

    packet.instruction = RGB_GETREPORT;
    // If light bar is present, set reserved or we get no configuration.
    if (lightbarClaimed)
        packet.reserved = 0x02;
    packet.checksum = (uint8_t)(0xFF - (RGB_GETREPORT & 0xFF) - packet.reserved);

    res = libusb_control_transfer(keyboard_handle, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to enter report reading mode");
        goto done;
    }
    // Now we can get the report from the input endpoint.
    res = libusb_control_transfer(keyboard_handle, 0xA1, 0x01, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to obtain report");
        goto done;
    }
    // Send needed data to set initial RGB UI settings
    if (packet.mode >= CUSTOM_ONE) {
        keyboard_rgb.mode = packet.mode - 38;
        keyboard_rgb.color = packet.color;
    } else {
        keyboard_rgb.mode = packet.mode - 1;
        keyboard_rgb.color = packet.color - 1;
    }
    keyboard_rgb.brightness = packet.brightness / 25;
    keyboard_rgb.speed = 11 - packet.speed;
    keyboard_rgb.random = packet.color & 0x08;

    qInfo("Packet debug: instruction %d reserved %d mode %d speed %d brightness %d color %d offset %d checksum %d",
          packet.instruction, packet.reserved, packet.mode, packet.speed, packet.brightness, packet.color, packet.offset, packet.checksum);

    if (packet.mode >= CUSTOM_ONE) {
        getCustomModeLayout(keyboard_rgb.mode);
    }

    if (lightbarClaimed)
        getLightbarReport();

done:
    qInfo("RGB finished, got %d", packet.mode);
    return 0;
}

int RGBKeyboard::getLightbarReport()
{
    packet packet;
    int res;

    packet.instruction = RGB_GETREPORT;
    packet.checksum = (uint8_t) 0xFF - (RGB_GETREPORT & 0xFF);

    res = libusb_control_transfer(light_bar, 0x21, 0x09, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to enter report reading mode");
        goto done;
    }

    res = libusb_control_transfer(light_bar, 0xA1, 0x01, 0x300, 0x03, (uint8_t*)&packet, 0x08, 0);
    if (res < 0) {
        qWarning("Unable to obtain report");
        goto done;
    }

    lightbar_rgb.mode = packet.mode;
    lightbar_rgb.speed = packet.speed;
    lightbar_rgb.brightness = packet.brightness;
    lightbar_rgb.color = packet.color;
    lightbar_rgb.random = packet.color & 0x08;

done:
    return 0;
}
