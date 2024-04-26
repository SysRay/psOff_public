## Building from scratch

**TL;DR**: If you know yml code, then just check out the [GitHub action workflow](https://github.com/SysRay/psOff_public/blob/main/.github/workflows/build.yml)

### Step 1: Setting up the environment

First things first, you need to download [Git](https://git-scm.com/download/win), [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) or its [Build Tools package](https://aka.ms/vs/17/release/vs_BuildTools.exe), both are fine. The last thing you need is [VulkanSDK](https://vulkan.lunarg.com/sdk/home), on this site you should click on _VulkanSDK-x.x.xxx.x-Installer.exe_ link, then just run the said installer and install it without any changes in its options.

#### Step 1.2: Visual Studio installation

When you click on the installer's exe you'll see a window with a bunch of checkboxes. You need to check Desktop development with C++ on the left pane, and then C++ Clang tools for Windows on the right pane, now you all set! Just click the install button and wait for the process to finish.

### Step 2: Clone the psOff repo
Open any folder you want (I personally prefer %USERPROFILE%\Documents\GitHub) then hold Shift and click RMB on the empty space in this folder. In the new context menu, click "Open in Terminal". Type in the following command: 
`git.exe clone --recurse-submodules https://github.com/SysRay/psOff_public.git`

### Step 3: Building the emulator

#### Step 3.1: Configure the project

Type this into your terminal window:
`cmake.exe -S. -B_build/_Release -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=_build/_Install -DCMAKE_C_COMPILER=clang-cl.exe -DCMAKE_CXX_COMPILER=clang-cl.exe`
 wait 'til it finishes. There shouldn't be any errors.

#### Step 3.2: Build the project

And now type this:
`cmake.exe --build _build/_Release -j4` This process could take a while. If you see any red text, then you probably missed something. Ask for help on out discord server.

### Step 4: Installing

Finally, type this command: 
`cmake.exe --install _build/_Release`
 Its execution usually takes under 2 or 3 seconds

Done! Now you can try to modify the emulator's code and see how it works. Just open `_build/_Install/` folder and you'll see `emulator.exe` there.
 
---
author: [igor](https://github.com/igor725)
---
