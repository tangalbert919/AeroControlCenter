# Adding new models

This document outlines how to add new models to the program.

## Overview
Currently, this program should run on any Aero model. It only checks the
`product_family` file in the Linux Desktop Management Interface (DMI) and makes
sure it is "AERO".

This does not mean the program will run perfectly on every Aero model. Certain
models may have specific hardware that we do not support, and because of this,
we must add support for them when we can.

## Keyboard
Support for new keyboards must be added to `rgbkeyboard.cpp`.

**NOTE:** All old Aero 14 models, and all Aero models introduced after 2021
lack RGB capabilities. Ensuring the program does not execute RGB functions on
those models will be addressed at a later time!

Keyboard vendors will differ between Aero models. To add support for your
keyboard, you must use `lsusb` to identify its vendor ID.

For example, on an Aero 15 Classic:
```
Bus 001 Device 020: ID 1044:7a3f Chu Yuen Enterprise Co., Ltd USB-HID Keyboard
```
The vendor ID here is `0x1044`.

Copy the following code into `RGBKeyboard::RGBKeyboard()` to add your keyboard,
replacing `<vendor-id>` with your vendor ID:
```
info = hid_enumerate(<vendor-id>, 0);
res = registerKeyboard(info);
if (!res) {
    keyboardAttached = true;
    goto end_probe;
}
```
