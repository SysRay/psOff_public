# Building psOff modules using VSCode

> [!WARNING]
> This guide assuming that you have all the Vulkan/MSVC/Clang stuff installed. If you don't have it follow [the first two steps here](https://github.com/SysRay/psOff_public/wiki/Building-psOff-from-scratch) and then return to this guide.

## Preparing the environment

When you open psOff repository with VSCode for the first time, it will show you the notification about installing recommended extensions. You should click `Show Recommenadtions` there.

![recomends_btn](/docs/wiki/images/vsbuild/recomends_btn.png)

If you missed this notification, then press `Ctrl+Shift+P` and type `Focus on Workspace Recommendations View` and perss this button.

![recomends_help](/docs/wiki/images/vsbuild/recomends_help.png)

Now you'll see the sidebar with all the recommended extensions, you should install all of them. Or just press the `Install Workspace Recommended Extensions` button above, the one with the cloud icon.

![recomends_install](/docs/wiki/images/vsbuild/recomends_install.png)

It is recommended to restart the VSCode after you finish installing all the recommended extensions.

All you have to do now is switch the default compiler to `clang-cl` if it didn't set automatically and set build target to Release.

![default_compiler](/docs/wiki/images/vsbuild/default_compiler.png)

![default_compiler_popup](/docs/wiki/images/vsbuild/default_compiler_popup.png)

Make sure the CMake Extension uses it too:

![default_compiler_sure](/docs/wiki/images/vsbuild/default_compiler_sure.png)

> [!WARNING]
> You won't be able to build the Debug version of emulator, you should switch CMake target to Release. But worry not, this target already uses debug symbols the only thing you should do to improve your debugging experience - disable compiler optimizations, open the main `CMakeLists.txt` file and change `CMAKE_CXX_FLAGS_RELEASE` flag `-Ofast` to `-O0`.
>
> ![debug_warning](/docs/wiki/images/vsbuild/debug_warning.png)
> ![optimizations](/docs/wiki/images/vsbuild/optimizations.png)

If you reading this, it seems you all set! So now you can use VSCode to build psOff modules. Just press `Ctrl+Shift+P` and type `CMake Focus` and press `CMake: Focus on Project Status View` then click on the `Configure` button first:

![cmake_configure](/docs/wiki/images/vsbuild/cmake_configure.png)

Then the `Build` one:

![cmake_build](/docs/wiki/images/vsbuild/cmake_build.png)

You can add these command to `Pinned Commands` list and use them there:

![cmake_pins](/docs/wiki/images/vsbuild/cmake_pins.png)

In fact, the only button you need is `Install`. When you press it, cmake will configure the project, build it and then copy ready to use files to `_build/_Install` directory.
