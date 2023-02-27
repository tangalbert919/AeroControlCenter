# Adding new models

This document outlines how to add new models to the program.

## Overview
Currently, this program should run on any Aero and AORUS model. It only checks
the `product_family` file in the Linux Desktop Management Interface (DMI) and
makes sure it is "AERO" or "AORUS".

This does not mean the program will run perfectly on every Aero or AORUS model.
Certain models may have specific hardware that we do not support, and because
of this, we must add support for them when we can.

## Keyboard
Support for new keyboards must be added to `rgbkeyboard.cpp`.

**NOTE:** All old Aero 14 models, and all Aero models introduced after 2021
lack RGB capabilities. Ensuring the program does not execute RGB functions on
those models will be addressed at a later time!

Before adding support for your keyboard, make sure you are able to configure
RGB through Gigabyte's Control Center on Windows. That way, you can be certain
that your keyboard truly supports RGB, even if Gigabyte did not advertise that
on their website for your model.

Most models supporting RGB use keyboards from Chu Yuen Enterprise. If your
model does not, you can add it to the code. To add support for your keyboard,
you must use `lsusb` to identify its vendor ID.

For example, on an Aero 15 Classic:
```
Bus 001 Device 020: ID 1044:7a3f Chu Yuen Enterprise Co., Ltd USB-HID Keyboard
```
The vendor ID here is `0x1044`.

You must add your vendor ID to the `vendors` enum in `data.h`. An example is
here with `EXAMPLE` as vendor `0x0476`:
```c
enum vendors {
    CHU_YUEN = 0x1044,
    EXAMPLE = 0x0476
};
```
