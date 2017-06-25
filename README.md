# L Engine

## About
L is a simple public-domain, minimal dependency, Vulkan game engine.

## Building

### Windows
Simply run [premake](https://github.com/premake/premake-core/wiki/Using-Premake) with the action you want,
then open the solution file and build from there. Alternatively `run.sh` should do that for you.

### Unix
You can either run the [premake](https://github.com/premake/premake-core/wiki/Using-Premake) and build from there, or you can run the `run.sh [build|run] [dbg|dev|rls]` commandline that will fetch premake, build it, build the engine (in configuration debug, development or release) and then optionally run the sample. All this is abstracted away by the VSCode settings if you're willing to use VSCode.

## Dependencies
- Premake (binaries supplied)
- Vulkan

### Windows
- WinAPI

### Unix
- Xlib

## Copyright
All parts of this software are released into the public domain (as per [this notice](LICENSE)) unless otherwise stated. Any contribution to this repository is implicitly subjected to the same release.
