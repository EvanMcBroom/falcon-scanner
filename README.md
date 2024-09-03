# Falcon Scanner

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE.txt)

CrowdStrike's Falcon ships with a collection of extensions.
These extensions allow the agent to process additional content at runtime such as files, network data, and memory acquisitions.
These extensions include CrowdStrike's machine learning features.

Falcon communicates with each of these extensions using a private set of COM interfaces known internally as the Aviary APIs.
This repo aims to document these APIs and provide example code for using a Falcon extension to scan data.

> :warning: This repo is not complete and does contain inaccuracies that should be corrected but it provides a starting foundation for the next round of analysis work on Falcon.

## Building

The project uses [CMake](https://cmake.org/) to generate and run the build system files for your platform.
The project does not rely on any library manager to allow it to be easily built in an offline environment if desired.

```
git clone --recurse-submodules https://github.com/SpecterOps/falcon-scanner.git
cd falcon-scanner/builds
cmake .. -A {Win32 | x64}
cmake --build .
```

The `examples` directory demonstrates how to use the library.