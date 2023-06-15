# Flipside-Mod-Manager

Flipside-Mod-Manager is a simple command-line tool for managing mods for the game Super Paper Mario. It provides an easy-to-use interface for installing, uninstalling, and backing up mods

## Features

1. **Mod Installation**: Install mods stored in a designated directory with just a few clicks.
2. **Backup**: Backup your original game files before mod installation to ensure that you can always return to the original game.
3. **Mod Uninstallation**: Remove one or all mods and restore the game to its original state.
4. **Compiling Game ROM**: Option to compile your game ROM after mod installation.
5. Comes with the SPM Hard Mode Mod as an example of what the mods folder should look like. Be sure to check out the [installing.md](https://github.com/L5050/SPM-Hard-Mode/blob/master/INSTALLING.md) file for more details

## How to Use

### Prerequisites
- Please make sure that you have wimms iso tools installed, specifically wit
- Make sure you have the game Super Paper Mario in `.iso` or `.wbfs` format in the same directory as the Mod Manager.
- All mods should be placed within a directory named `mods`, and each mod should be in its own folder within the `mods` directory.
- Each mod directory must contain a `modinfo.ini` file. The first line of this file should be `modname=Your Mod Name`. Usually this is provided by the mod creator, but if not then you will have to make it yourself

### Usage

1. Run the Flipside-Mod-Manager application.
2. If your game is not yet extracted, the tool will automatically extract it from `spm.iso` or `spm.wbfs` that it finds in the directory.
3. You will see a list of available mods to install. Each mod will be numbered.
4. To install a mod, simply enter the corresponding number and press `Enter`.
5. If you want to uninstall all mods, enter `-1` and press `Enter`. This will restore the game to its original state using the backup.
6. If you are finished and want to quit the application, enter `0` and press `Enter`. The program will then ask you if you want to compile your game rom. If you want to do this, enter `yes` and press `Enter`.

**Disclaimer:** Modding can potentially damage game files. Always ensure that you have a backup of your original game files before installing any mods. Furthermore, there is currently very limited support for multiple rel mods, assume that you can only have one rel mod installed unless the mod maker says specifically that it can be installed with other rel mods.
