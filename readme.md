# psOff - PlayStation 4 Emulation (Windows)
* Unmatched performance
* State-of-the-art shader reconstruction
* Steadily improved


| Release | Nightly | Discord |
|---------|----------|---------|
| [<img src="https://github.com/SysRay/psOff_public/actions/workflows/build.yml/badge.svg?branch=main"/>](https://github.com/SysRay/psOff_public/releases) | [<img src="https://github.com/SysRay/psOff_public/actions/workflows/build.yml/badge.svg?branch=features"/>](https://github.com/SysRay/psOff_public/actions/workflows/build.yml?query=branch%3Afeatures) | [<img src="https://img.shields.io/discord/1215784508708749322?color=5865F2&label=ps_off&logo=discord&logoColor=white"/>](https://discord.gg/Jd2AuBN6eW) |

</div>

  ---


**In development. Does it run ...?** Check out our [Compatibility List](https://github.com/SysRay/psOff_compatibility/issues) to find out.

## Getting Started
[wiki: Building psoff](https://github.com/SysRay/psOff_public/wiki/Building-psOff-from-scratch)

### Executing a program
> [!IMPORTANT]  
> Vulkan SDK min. 1.3.268.0 has to be installed!

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
> [!TIP]
> Try out [input_ps4](https://github.com/igor725/input_ps4) to see if the emulator and your controller work correctly

### Dependencies
+ Vulkan SDK 1.3.268.0, minimum
+ Up to date graphic drivers

(May add the Vulkan libs in Future Releases, if needed.)

For development:

+ Ninja
+ CMake 3.24+
+ Visual Studio 2022, it's just for the build environment (uses clang-cl,c++20)
+ Windows SDK 10.0.22621.0 or newer

## Development

Use boosts thread, mutex and conditions. The waits have to be alertable in order to receive fake signals from "kernel".

Project uses p7 for tracing -> Baical server.

* modules: the ps4 libraries
* core: kernel, memory etc.
* psoff.exe: (From Releases) contains the runtime and rendering

'.vscode\tasks.json' contains the config & build tasks.

