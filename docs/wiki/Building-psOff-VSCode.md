# Building psOff modules using VSCode

> [!WARNING]
> This guide assuming that you have all the Vulkan/MSVC/Clang stuff installed. If you don't have it follow [the first two steps here](https://github.com/SysRay/psOff_public/wiki/Building-psOff-from-scratch) and then return to this guide.

## Preparing the environment

When you open psOff repository with VSCode for the first time, it will show you the notification about installing recommended extensions. You should click `Show Recommenadtions` there.

![Code_2024-03-31_18-53-20](https://github.com/igor725/psOff_public/assets/40758030/cf68c43d-880e-455d-861f-e79dd5fbc346)

If you missed this notification, then press `Ctrl+Shift+P` and type `Focus on Workspace Recommendations View` and perss this button.

![WindowsSandboxClient_2024-03-31_18-57-38](https://github.com/igor725/psOff_public/assets/40758030/0f880db8-99d3-45a8-bf58-ffd670c83ae7)

Now you'll see the sidebar with all the recommended extensions, you should install all of them. Or just press the `Install Workspace Recommended Extensions` button above, the one with the cloud icon.

![WindowsSandboxClient_2024-03-31_18-53-44](https://github.com/igor725/psOff_public/assets/40758030/f26d29ef-6d83-4dcd-9b6d-54a123f1730f)

It is recommended to restart the VSCode after you finish installing all the recommended extensions.

All you have to do now is switch the default compiler to `clang-cl` if it didn't set automatically and set build target to Release.

![WindowsSandboxClient_2024-03-31_18-55-18](https://github.com/igor725/psOff_public/assets/40758030/07a06620-e44a-4ab9-a98d-d44abd770551)

![WindowsSandboxClient_2024-03-31_18-55-26](https://github.com/igor725/psOff_public/assets/40758030/4b1c1821-d2b4-459f-905f-b06d057dcd38)

Make sure the CMake Extension uses it too:

![Code_2024-03-31_19-44-08](https://github.com/igor725/psOff_public/assets/40758030/d801a28f-8e57-4c1a-9c66-f45861b78e7f)

> [!WARNING]
> You won't be able to build the Debug version of emulator, you should switch CMake target to Release. But worry not, this target already uses debug symbols the only thing you should do to improve your debugging experience - disable compiler optimizations, open the main `CMakeLists.txt` file and change `CMAKE_CXX_FLAGS_RELEASE` flag `-Ofast` to `-O0`.
>
> ![Code_2024-03-31_19-44-15](https://github.com/igor725/psOff_public/assets/40758030/0ba33d03-31c7-4b36-814d-df8b5dd90545)
> ![Code_2024-04-03_11-09-31](https://github.com/igor725/psOff_public/assets/40758030/525039ee-97e5-48d1-8aef-411e78c26466)

If you reading this, it seems you all set! So now you can use VSCode to build psOff modules. Just press `Ctrl+Shift+P` and type `CMake Focus` and press `CMake: Focus on Project Status View` then click on the `Configure` button first:

![firefox_2024-04-03_11-16-29](https://github.com/igor725/psOff_public/assets/40758030/446c7fb1-a4e7-4b21-8ce5-2a3586e95042)

Then the `Build` one:

![image](https://github.com/igor725/psOff_public/assets/40758030/e890e20c-cc88-416c-8549-c55764a0dd88)

You can add these command to `Pinned Commands` list and use them there:

![2024-04-03_11-17-57](https://github.com/igor725/psOff_public/assets/40758030/1b83e2ed-d570-437a-bb2b-8b3ccd3cf084)

In fact, the only button you need is `Install`. When you press it, cmake will configure the project, build it and then copy ready to use files to `_build/_Install` directory.
