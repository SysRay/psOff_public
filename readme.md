# psOff - PlayStation 4 Emulation (Windows)

[<img src="https://img.shields.io/discord/1215784508708749322?color=5865F2&label=ps_off&logo=discord&logoColor=white"/>](https://discord.gg/Jd2AuBN6eW)

**In development. Does it run ...? No!**


Runtime and rendering part is private currently.
Target is to have a standalone framework for shader reconstruction, command buffer translation and managing the GPU memory, for others to use. This project uses it to run "Linux" binaries on windows.

<img width="400" alt="tombRaiderRemaster" src="https://github.com/SysRay/psOff_public/assets/48695846/c4d25350-9e19-4a50-b065-a86e5d756f69">
<img width="400" alt="sonic" src="https://github.com/SysRay/psOff_public/assets/48695846/d3c646b9-bb14-4c9f-a2f2-80bc5a184d74">
<img width="400" src="https://github.com/SysRay/psOff_public/assets/48695846/6156412e-569d-4f2a-b8d2-061d6942e107">
<img width="400" alt="blackhole" src="https://github.com/SysRay/psOff_public/assets/48695846/86767eae-b5f2-4094-bd54-3ab94c043a68">


## Description
Yet another compatibility layer! I solely focused on the rendering part and recently started to implement the system functions to run some binaries.

It renders decently fast, the planned changes will make it almost perfect. Had to reverse engineer and try out a lot, before.

It currently doesn't support:
+ multiple command buffer submits
+ for loops in shader
+ missing opcodes
+ missing symbols


I’m happy for any help. Just hop in, system part isn't complicated. \
I don't serve spaghetti only bugs.


## Getting Started
[wiki: Building psoff](https://github.com/SysRay/psOff_public/wiki/Building-psOff-from-scratch)

### Dependencies
+ Vulkan SDK 1.3.268.0, minimum
+ Up to date graphic drivers

(May add the Vulkan libs in Future Releases, if needed.)

For development:

+ Ninja
+ CMake 3.24+
+ Visual Studio 2022, it's just for the build environment (uses clang-cl,c++20)
+ Windows SDK 10.0.22621.0 or newer



### Installing
For testing, just download the latest release and install the Vulkan SDK.

### Executing a program

```
.\psoff.exe --h
.\psoff.exe --file="C:/****/eboot.bin"
```
If no --root is defined, it uses the folder from --file \
For testing/benchmark, use --vsync=0

Update (app1):  _--update_
```
.\psoff.exe --file="C:/****/eboot.bin" --update="C:/FolderWithUpdate"
```

It expects the undecrypted Target to be in the following format:
```
sce_module
sce_sys
eboot.bin
```

Try out [input_ps4](https://github.com/igor725/input_ps4) to see if the emulator and your controller work correctly

## Development

Use boosts thread, mutex and conditions. The waits have to be alertable in order to receive fake signals from "kernel".

Project uses p7 for tracing -> Baical server.

* modules: the ps4 libraries
* core: kernel, memory etc.
* psoff.exe: (From Releases) contains the runtime and rendering

I'm using vscode, '.vscode\tasks.json' contains the config & build tasks.
