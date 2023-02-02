#include "rgbkeyboard.h"

#include <cwchar>
#include <hidapi/hidapi.h>

#define MAX_STR 255

RGBKeyboard::RGBKeyboard()
{
    int res;
    unsigned char buf[65];
    wchar_t wstr[MAX_STR];

    // Initialize hidapi (not strictly necessary unless we are using multiple threads)
    res = hid_init();

    // The keyboards in Gigabyte's laptops come from multiple vendors, so we need to find them.
    // Start with Chu Yuen Enterprise Co., Ltd.
    info = hid_enumerate(0x1044, 0);
    if (info) {
        // Make sure the device manufacturer is Gigabyte and the device is a keyboard.
        if (std::wcscmp(info->manufacturer_string, L"GIGABYTE\n") &&
                std::wcscmp(info->product_string, L"USB-HID Keyboard\n")) {
            handle = hid_open(info->vendor_id, info->product_id, info->serial_number);
            if (!handle) {
                // Failed to open device.
                hid_free_enumeration(info);
                hid_exit();
            }
        }
    }
    else {
        printf("unsupported keyboard\n");
    }
}

RGBKeyboard::~RGBKeyboard()
{
    if (info)
        hid_free_enumeration(info);
    hid_close(handle);
    hid_exit();
}
