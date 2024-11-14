# Aero Control Center (Linux)

This is a Linux port of Gigabyte's Control Center for their Aero line of laptops.

The program works best if you have my [kernel driver](https://github.com/tangalbert919/gigabyte-laptop-wmi) installed.

## Supported models

Tested:
- Aero 15 Classic (SA/WA/XA/YA)

Untested:
- Other Aero models
- AORUS laptops

## Installation
For RGB to work, you must copy `70-keyboard.rules` to `/etc/udev/rules.d`.

Full installation instructions are currently not available yet.

## Contributing
This program uses Qt. If you wish to contribute, Qt Creator is the recommended IDE to use.

The following libraries are required:
* Qt (version 6 is recommended)
* libusb (for RGB keyboard support)
* CMake (to build the program)

Make sure you have development tools installed (compilers, library headers,
etc) on your system. The easiest way to do that is by installing meta-packages
available through your package manager (such as `build-essential` on Ubuntu
and `base-devel` on Arch Linux).

### Adding new models
Refer to [Adding-new-models.md](./Adding-new-models.md) for more details.

## Compiling
Refer to the "Contributing" section on the dependencies required.

This program uses CMake as the build system. To build, simply run:
```
cmake -B aero
cmake --build aero
```

To run the program, just run:
```
./aero/AeroControlCenter
```

## TODO
- [ ] Complete UI work
- [x] Add documentation to the code
- [x] Obtain fan speeds and switch fan modes manually
- [ ] Dynamically switch fan modes based on current process
- [ ] Detect what Aero model this program is running on
- [ ] Obtain Gigabyte's XML files used for their Control Center
- [ ] Test on more models
- [x] Add keyboard RGB functionality
- [x] Add UI for keyboard RGB
- [x] Add support for AORUS models

## Interesting projects
- [fusion-kbd-controller](https://github.com/martin31821/fusion-kbd-controller) (only supports Aero 15X)
