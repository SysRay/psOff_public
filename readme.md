# psOff - Emulation for playstation 4
Windows only.
Tested only with AMD GPU. May not run with nvidia, currently.

Will add more to the "private" part later.

Target is to have a standalone framework for shader reconstruction, commandbuffer translation and managing the GPU memory, for others to use. That’s why the project is currently split.

## Description
Yet another compatibility layer! I solely focused on the rendering part and recently started to implement the system functions to run some binaries. It renders decently fast. The planned changes will make it almost perfect. Had to reverse engineer and try out a lot previously.

It currently doesn't support:
+ multiple command buffer submits
+ for loops in shader
+ missing opcodes


I’m happy to receive any help. Just hop in, system part isn't complicated. \
I don't serve spaghetti, only bugs.

## Getting Started
### Dependencies
+ Vulkan SDK 1.3.268.0, minimum
+ up to date graphic drivers

For development:

+ Ninja
+ CMake
+ Visual Studio 2019, for the build environment (clang-cl,c++20)

### Installing
For testing, just download the latest release. And install the Vulkan SDK

For development, set CMAKE_INSTALL_PREFIX to the downloaded emulators folder.

### Executing program
```
.\emulator.exe --h
.\emulator.exe --file="C:/****/eboot.bin"
```
If no --root is defined, it uses the folder from --file \
For testing/benchmark, add --vsync=0


It expects the Target to be in the following format:
```
sce_module
sce_sys
eboot.bin
```

## Development

Uses boost's thread, mutex and conditions. The waits have to be alert able in order to receive fake signals from "kernel".

