# Flipside-Mod-Manager

Flipside-Mod-Manager is a command-line tool for managing mods for Super Paper Mario. It can install and uninstall mods, patch the rom to be capable of loading rel mods, and back up files that are replaced by mods.

## How to Use Flipside Mod Manager

- Please read the entire guide first before following any of these steps

### Prerequisites
- Please make sure that you have [Wiimms ISO Tools](https://wit.wiimm.de/) installed (scroll down to "Download the current stable version")
- Please make sure that you have [Wiimms SZS Toolset](https://szs.wiimm.de/) aka wstrt installed (scroll down to "Download the current stable version")
- Restart your computer after installing these tools (Only necessary on Windows)
- Make sure you have your legally obtained Super Paper Mario rom in `.iso` or `.wbfs` format

### Usage
1. Drag and drop your SPM rom the mod manager executable (note: Windows only; on Linux, just run the path to your rom as a commandline argument) OR put your game rom in the same directory as the executable and name it spm.iso or spm.wbfs, then run the mod manager
2. It will ask you for your SPM version, please enter it
4. You will see a list of available mods to install. Each mod will be numbered.
5. To install a mod, enter the corresponding number and press `Enter`.
6. If you want to uninstall all mods, enter `-1` and press `Enter`. This will restore the game to its original state using the backup.
7. If you are finished installing mods, enter 0 and press Enter. The program will then ask you if you want to compile your game rom. If you want to do this, enter yes, and it will generate a new file called patched.wbfs. This is the modded version of your rom; your original rom has not been modified. You can play your mods by running patched.wbfs in Dolphin or on a usb loader!

### List of mods
- Practice codes
- Hard Mode JR
- Hard Mode: Rubies and Magic
- Brobot after some Windex
- PitRando

If you were linked to this page by one of these mods, this is their installer. Enjoy!

### Credit
PitRando and Practice codes are made by [shiken-yme](https://github.com/shiken-yme) and [SeekyCt](https://github.com/SeekyCt) respectively, all other mods in the mod library are made by me. Thank you to [shiken-yme](https://github.com/shiken-yme) for helping me write this readme!

**Disclaimer:** Modding can potentially damage game files. Always ensure that you have a backup of your original game files before installing any mods: it is highly recommended to move the original game rom out of the folder once it has been extracted as a failsafe way to get back to vanilla. Furthermore, there is currently very limited support for multiple rel mods, assume that you can only have one rel mod installed at a time unless the mod developer specifically says that it can be installed with other rel mods.
