# psOff - PlayStation 4 Emulation (Windows)

**In development**

Tested only with AMD GPU. May not run with NVidia, currently.

Will add more of the "private" part later.

Target is to have a standalone framework for shader reconstruction, command buffer translation and managing the GPU memory, for others to use. This project uses it to run "Linux" binaries on windows.


## Description
Yet another compatibility layer! I solely focused on the rendering part and recently started to implement the system functions to run some binaries, it renders decently fast. The planned changes will make it almost perfect. Had to reverse engineer and try out a lot, before.

It currently doesn't support:
+ multiple command buffer submits
+ for loops in shader
+ missing opcodes

I’m happy for any help. Just hop in, system part isn't complicated. \
I don't serve spaghetti only bugs.

## Getting Started
### Dependencies
+ Vulkan SDK 1.3.268.0, minimum
+ up to date graphic drivers

(May add the Vulkan libs in future Releases, if needed )

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

Use boosts thread, mutex and conditions. The waits have to be alert able in order to receive fake signals from "kernel". 

Project uses p7 for tracing -> Baical server.
