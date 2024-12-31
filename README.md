# Flipside-Mod-Manager

Flipside-Mod-Manager is a command-line tool for managing mods for the game Super Paper Mario. It provides an interface for installing and uninstalling mods, as well as backing up files that need to be replaced

## Features

1. **Mod Installation**: Install mods stored in a designated directory with just a few clicks.
2. **Backup**: Backup your original game files before mod installation to ensure that you can always return to the original game.
3. **Mod Uninstallation**: Remove one or all mods and restore the game to its original state.
4. **Compiling Game ROM**: Option to compile your game ROM after mod installation.
5. Comes with a mod library including SPM Hard Mode Mod as an example of what the mods folder should look like. Be sure to check out the [installing.md](https://github.com/L5050/SPM-Hard-Mode/blob/master/INSTALLING.md) file for more details on how to enable rel mods.

## How to Use

### Prerequisites
- Please make sure that you have wimms iso tools installed, specifically wit
- Make sure you have the game Super Paper Mario in `.iso` or `.wbfs` format
- All mods should be placed within a directory named `mods`, and each mod should be in its own folder within the `mods` directory.
- Each mod directory must contain a `modinfo.ini` file. The first line of this file should be `modname=Your Mod Name`. Usually this is provided by the mod creator, but if not then you will have to make it yourself
- Any rel mods will require you to install a gecko code to load the rel file. If the mod creator did not provide you with one then get the one for your version of SPM [here](https://github.com/SeekyCt/spm-rel-loader/tree/master/spm-rel-loader/loader)

### Usage

1. Run the Mod-Manager executible.
2. It will ask you for your SPM version, please enter it
3. You can extract your game files by running your SPM rom with the mod manager before opening it (note: windows only) or by putting your game rom in the same directory as the executible and naming it "spm.iso" or "spm.wbfs"
4. You will see a list of available mods to install. Each mod will be numbered.
5. To install a mod, enter the corresponding number and press `Enter`.
6. If you want to uninstall all mods, enter `-1` and press `Enter`. This will restore the game to its original state using the backup.
7. If you are finished and want to quit, enter `0` and press `Enter`. The program will then ask you if you want to compile your game rom. If you want to do this, enter `yes`, then enter any input to exit.

### List of mods
- Practice codes
- Hard Mode
- Hard Mode: Rubies and Magic
- Brobot after some Windex
- PitRando

If you were linked to this page by one of these mods, this is their installer. Enjoy!

### Credit
PitRando and Practice codes are made by [shiken-yme](https://github.com/shiken-yme) and [SeekyCt](https://github.com/SeekyCt) respectively, all other mods in the mod library are made by me.

**Disclaimer:** Modding can potentially damage game files. Always ensure that you have a backup of your original game files before installing any mods: it is highly recommended to move the original game rom out of the folder once it has been extracted as a failsafe way to get back to vanilla. Furthermore, there is currently very limited support for multiple rel mods, assume that you can only have one rel mod installed unless the mod maker says specifically that it can be installed with other rel mods.
