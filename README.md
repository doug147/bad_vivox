# bad_vivox
 Stop the many vivox related errors in the ARK Dev Kit using detours and dll proxying

## Prerequisites
I made this to work on the latest version of the dev kit (v320.1) but it should work on previous versions.

The latest Visual Studio C++ runtime files, located here (*chances are you probably already have these installed*):

https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads

## Install instructions
1. Compile the source, or download the precompiled dll from https://github.com/doug147/bad_vivox/releases - note there are **2** versions.. one (**bad_vivox_1_0_with_console.zip**) makes a console window also appear when ark launches as a standalone game and it prints debug information for you to see as it stops the crashes, and the other (**bad_vivox_1_0.zip**) does not print any debug information.

![Example of version with console window and debug output](https://i.imgur.com/Wtpdn1N.png)

2. Drop **winmm.dll** into the same directory as **UE4Editor.exe**.  For example, the directory I placed mine in is **C:\Program Files\Epic Games\ARKEditor\Engine\Binaries\Win64**.

![Path to drop winmm.dll](https://i.imgur.com/xMY9TDM.png)

3. That's all, you should now be able to run ARK from the dev kit in standalone mode without the Vivox errors.  See below for how it works.

*Note: By having winmm.dll in that directory it could mean that any .exe file that runs from that specific directory will also cause our winmm.dll to attach to it.. but it should not cause any problems.  The dll will autmoatically unload itself after a few minutes once it does not find the standalone ark window and once it does not find the module UE4-ShooterGame.dll.  If you do not want it to do this or if it causes problems with anything (it has not in my testing), just rename winmm.dll to something else while you are not using it, and rename it back when you want to launch ark as a standalone game from the dev kit.*

## How it works

1. DLL proxying (more specifically in this case it is a dll wrapper) is used for purpose of having the DLL loaded automatically once UE4Editor.exe starts, in order to quickly hook the Vivox functions that cause crashes.  When UE4Editor.exe starts, it initially looks in it's own directory for **winmm.dll** before searching elsewhere and eventually the real winmm.dll at *c:\windows\system32\winmm.dll*.  We simply place our dll in it's directory and forward all non-implemented functionality back to the original DLL.

2. There are 5 functions that I found which all cause crashes and are all related in one way or another to Vivox: 

* UShooterGameInstance::VivoxInitialize
* UShooterGameInstance::VivoxLogout
* UShooterGameInstance::VivoxUninitialize
* UShooterGameUserSettings::ApplySFXVolumes
* AShooterPlayerController::ToggleSpeaking

These functions are located through their signatures which I created from their opcodes, and are in the module *UE4Editor-ShooterGame.dll*.

All 5 functions are hooked using Microsoft Detours and rather than letting them execute I simply force them to do nothing, so the original code that was causing the crashes never actually executes.

## Removal

1. Delete **winmm.dll** from the directory you placed it in
