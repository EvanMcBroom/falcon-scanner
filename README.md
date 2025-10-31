# Falcon Scanner

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE.txt)

CrowdStrike's Falcon ships with a collection of extensions.
These extensions allow the agent to process additional content at runtime such as files, network data, and memory acquisitions.
These extensions include CrowdStrike's machine learning features.

Falcon communicates with each of these extensions using a private set of interfaces which conform closely to Microsoft's COM architecture and are known internally as the Aviary APIs.
This repo aims to document these APIs and provide example code for using a Falcon extension to scan data.

> :warning: This repo is not complete and does contain inaccuracies that should be corrected but it provides a starting foundation for the next round of analysis work on Falcon.

## Building

The project uses [CMake](https://cmake.org/) to generate and run the build system files for your platform.
The project does not rely on any library manager to allow it to be easily built in an offline environment if desired.

```
git clone --recurse-submodules https://github.com/SpecterOps/falcon-scanner.git
cd falcon-scanner/builds
cmake .. -A {Win32 | x64}
cmake --build . --config Release
```

The `examples` directory demonstrates how to use the library.

By default CMake will build a `falcon` library with equivalent functionality to a subset of the Aviary API.
There is a `libfalcon.hpp` file that may be used as the SDK for the library.
The `examples` directory demonstrates how to use the library.

CMake will also build a `pyfalcon` Python module to allow for easy access to the API.
For `pyfalcon` to successfully build, you must install the Python debug binaries (an optional component during installation) and build using the `Release` config.
Any non-free-threading version of Python 3.8 and higher should work, except for version 3.13.
Version 3.13 introduced nonstandard file names when adding support for free-threaded binaries which [pybind11 never fully supported](https://github.com/pybind/pybind11/issues/5112).