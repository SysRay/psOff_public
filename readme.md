# psOff - PlayStation 4 Emulation (Windows)

[<img src="https://img.shields.io/discord/1215784508708749322?color=5865F2&label=ps_off&logo=discord&logoColor=white"/>](https://discord.gg/Jd2AuBN6eW)

**In development. Does it run ...? No!**


Runtime and rendering part is private currently.
Target is to have a standalone framework for shader reconstruction, command buffer translation and managing the GPU memory, for others to use. This project uses it to run "Linux" binaries on windows.

<img src="https://github.com/SysRay/psOff_public/assets/48695846/cd83ba97-b649-4599-b555-24c43e9b8394" width="400"> 
<img src="https://github.com/SysRay/psOff_public/assets/48695846/6156412e-569d-4f2a-b8d2-061d6942e107" width="400">
<img src="https://github.com/SysRay/psOff_public/assets/48695846/7e723874-5bb9-4cb7-bb28-5b0001d81f65" width="400" title="fps is capped internally">


No, the fps is not fake. :)

## Description
Yet another compatibility layer! I solely focused on the rendering part and recently started to implement the system functions to run some binaries.

It renders decently fast, the planned changes will make it almost perfect. Had to reverse engineer and try out a lot, before.

It currently doesn't support:
+ multiple command buffer submits
+ for loops in shader
+ missing opcodes

I’m happy for any help. Just hop in, system part isn't complicated. \
I don't serve spaghetti only bugs.

## Getting Started
### Dependencies
+ Vulkan SDK 1.3.268.0, minimum
+ Up to date graphic drivers

(May add the Vulkan libs in Future Releases, if needed.)

For development:

+ Ninja
+ CMake 3.24+
+ Visual Studio 2019 or newer, it's just for the build environment (uses clang-cl,c++20)

### Installing
For testing, just download the latest release and install the Vulkan SDK. 

### Executing program
```
.\emulator.exe --h
.\emulator.exe --file="C:/****/eboot.bin"
```
If no --root is defined, it uses the folder from --file \
For testing/benchmark, add --vsync=0


It expects the undecrypted Target to be in the following format:
```
sce_module
sce_sys
eboot.bin
```

## Development

Use boosts thread, mutex and conditions. The waits have to be alertable in order to receive fake signals from "kernel".

Project uses p7 for tracing -> Baical server.

* modules: the ps4 libraries
* core: kernel, memory etc.
* emulator.exe: (From Releases) contains the runtime and rendering

I'm using vscode, '.vscode\tasks.json' contains the config & build tasks.
