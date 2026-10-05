# Falcon Scanner

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE.txt)

CrowdStrike's Falcon ships with a collection of extensions.
These extensions allow the agent to process additional content at runtime such as files, network data, and memory acquisitions.
These extensions include `CsMl.dll`, CrowdStrike's machine learning features, which ships with any Falcon installation.

Falcon communicates with each of these extensions using a private set of interfaces which conform closely to Microsoft's COM architecture and are known internally as the Aviary APIs.
This repo documents these APIs and provides example code for using the `CsMl.dll` extension to scan files. The `CsMl.dll` extension uses a self-contained ML model and can be used without a license or Internet access.

## Building

The project uses [CMake](https://cmake.org/) to generate and run the build system files for your platform.
The project does not rely on any library manager to allow it to be easily built in an offline environment if desired.

```
git clone --recurse-submodules https://github.com/SpecterOps/falcon-scanner.git
cd falcon-scanner/builds
cmake .. -A x64
cmake --build . --config Release
```

The `examples` directory demonstrates how to use the library.

By default CMake will build a `falcon` library with equivalent functionality to a subset of the Aviary API.
There is a `libfalcon.hpp` file that may be used as the SDK for the library.
The `examples` directory demonstrates how to use the library.

CMake will also build a `pyfalcon` Python module to allow for easy access to the API.
Builds only currently support non-free-threaded Python installations.
To choose a specific Python installation, supply `-DPython_EXECUTABLE=<path to python.exe>` when configuring CMake.

## Running

If you have a licensed copy of Falcon, the `CsMl.dll` extension will be in its installation directory.
You may use it with the example C program like so:

```bash
scan_file.exe <path to CsMl.dll> <path to file>
```

For the Python script, first copy the built *.pyd file to the same directory as the script, then you may use it like so:

```bash
python pyscanner.py <path to CsMl.dll> <path to file>
```

## Acknowledgments

This work was done as a collaboration with [Valdemar Carøe](https://github.com/bytewreck).
I initially began the work in 2024.
Valdemar picked up the project in 2025, providing C++ updates to make it functional.
I merged their work in da13f98 and additional build and documentation updates and Python bindings.
The work has since been functional, with only minor updates done for improving the internal code.
