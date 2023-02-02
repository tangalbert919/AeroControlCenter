# Aero Control Center (Linux)

This is a Linux port of Gigabyte's Control Center for their Aero line of laptops.

Currently, the program lacks functionality, and will be added as soon as possible.

## Models tested
- Aero 15 Classic (SA/WA/XA/YA)

## Contributing
This program uses Qt. If you wish to contribute, Qt Creator is the recommended IDE to use.

The following libraries are required:
* Qt (version 6 is recommended)
* hidapi (for RGB keyboard support)
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
- [ ] Add documentation to the code
- [ ] Obtain fan speeds and switch fan modes manually
- [ ] Dynamically switch fan modes based on current process
- [ ] Detect what Aero model this program is running on
- [ ] Obtain Gigabyte's XML files used for their Control Center
- [ ] Test on more models
- [ ] Add keyboard RGB functionality
- [ ] Add UI for keyboard RGB
- [ ] Add support for AORUS models (maybe)
- [ ] Add support for Gigabyte Gaming models (maybe)
- [ ] Port this program to Windows (maybe)
