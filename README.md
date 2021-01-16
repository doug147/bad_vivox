# bad_vivox
 Stop the many vivox related errors in the ARK Dev Kit using detours and dll proxying

## Prerequisites
The latest Visual Studio C++ runtime files, located here (*chances are you probably already have these installed*):

https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads

## Install instructions
1. Compile the source, or download the precompiled dll from https://github.com/doug147/bad_vivox/releases 
2. Drop **winmm.dll** into the same directory as **UE4Editor.exe**.  For example, the directory I placed mine in is **C:\Program Files\Epic Games\ARKEditor\Engine\Binaries\Win64**.

![Path to drop winmm.dll](https://i.imgur.com/KzdzUHz.png)

3. That's all, you should now be able to run ARK from the dev kit in standalone mode without the Vivox errors.  See below for how it works.

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
