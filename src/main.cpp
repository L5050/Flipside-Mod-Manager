#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

std::map < std::string, std::string > modPaths;

bool checkExtracted() {
  std::filesystem::path extractedPath("extracted");
  std::filesystem::path isoPath("spm.iso");
  std::filesystem::path wbfsPath("spm.wbfs");

  if (std::filesystem::exists(extractedPath)) {
    std::cout << "The extracted folder exists, reading contents...\n";
  } else if (std::filesystem::exists(isoPath)) {
    std::cout << "Extracting from spm.iso...\n";
    int result = system("wit x spm.iso extracted");
    if (result == -1) {
      std::cerr << "Error occurred during extraction from spm.iso\n";
      return false;
    }
  } else if (std::filesystem::exists(wbfsPath)) {
    std::cout << "Extracting from spm.wbfs...\n";
    int result = system("wit x spm.wbfs extracted");
    if (result == -1) {
      std::cerr << "Error occurred during extraction from spm.wbfs\n";
      return false;
    }
  } else {
    std::cout << "No extracted folder or iso could be found.\n";
    return false;
  }
  return true;
}

std::string getModName(const std::string & modFolder) {
  std::string line;
  std::ifstream modinfo(modFolder + "/modinfo.ini");
  if (modinfo.is_open()) {
    std::getline(modinfo, line);
    auto pos = line.find('=');
    if (pos != std::string::npos) {
      return line.substr(pos + 1);
    }
    modinfo.close();
  } else {
    std::cout << "Unable to open modinfo.ini file for mod: " << modFolder << std::endl;
  }
  return "";
}

void installMod(const std::string &modName) {
  std::cout << "Installing " << modName << "...\n";

  std::filesystem::path backupPath("backup");
  if (!std::filesystem::exists(backupPath)) {
    std::filesystem::create_directory(backupPath);
  }

  std::filesystem::path modPath = modPaths[modName];

  for (const auto &entry : std::filesystem::recursive_directory_iterator(modPath)) {
    if (entry.is_regular_file()) {
      std::filesystem::path filePath = entry.path();
      if (filePath.filename() != "modinfo.ini") {
        std::filesystem::path relPath = std::filesystem::relative(filePath, modPath);
        std::filesystem::path destination = "extracted/files/" / relPath;
        if (std::filesystem::exists(destination)) {
          std::filesystem::path backupDest = "backup/" / relPath;
          if(!std::filesystem::exists(backupDest)) {  // Check if file already exists in backup
            std::filesystem::create_directories(backupDest.parent_path());
            std::filesystem::rename(destination, backupDest);
          }
        }
        std::filesystem::create_directories(destination.parent_path());
        std::filesystem::copy(filePath, destination, std::filesystem::copy_options::overwrite_existing);
      }
    }
  }

  std::cout << "Installed " << modName << "\n";
}



void uninstallAllMods() {
  std::cout << "Uninstalling all mods...\n";
  std::filesystem::path backupPath("backup");
  std::filesystem::path extractedPath("extracted/files");

  if (std::filesystem::exists(backupPath)) {
    for (const auto & entry: std::filesystem::recursive_directory_iterator(backupPath)) {
      if (entry.is_regular_file()) {
        std::filesystem::path filePath = entry.path();
        std::filesystem::path relPath = std::filesystem::relative(filePath, backupPath);
        std::filesystem::path destination = extractedPath / relPath;
        std::filesystem::create_directories(destination.parent_path());
        std::filesystem::copy(filePath, destination, std::filesystem::copy_options::overwrite_existing);
      }
    }
    std::cout << "All mods uninstalled, original files restored.\n";
  } else {
    std::cout << "Backup folder doesn't exist, nothing to restore.\n";
  }

  std::filesystem::path modFolderPath = extractedPath / "mod";
  if (std::filesystem::exists(modFolderPath)) {
    std::filesystem::remove_all(modFolderPath);
  }
}


void uninstallMod(const std::string &modName) {
  std::cout << "Uninstalling " << modName << "...\n";

  std::filesystem::path backupPath("backup");
  std::filesystem::path extractedPath("extracted/files");
  std::filesystem::path modPath = modPaths[modName];
  std::filesystem::path modFolderPath = extractedPath / "mod";

  if (std::filesystem::exists(backupPath)) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(modPath)) {
      if (entry.is_regular_file()) {
        std::filesystem::path filePath = entry.path();
        std::filesystem::path relPath = std::filesystem::relative(filePath, modPath);
        std::filesystem::path backupDest = backupPath / relPath;
        if (std::filesystem::exists(backupDest)) {
          std::filesystem::path destination = extractedPath / relPath;
          // Skip if destination is within modFolderPath
          if (destination.string().find(modFolderPath.string()) != 0) {
            std::filesystem::copy(backupDest, destination, std::filesystem::copy_options::overwrite_existing);
          }
        }
      }
    }
    std::cout << "Mod " << modName << " uninstalled, original files restored.\n";
    if (std::filesystem::exists(modFolderPath)) {
      std::filesystem::remove_all(modFolderPath);
    }
  } else {
    std::cout << "Backup folder doesn't exist, nothing to restore.\n";
  }
}

int main() {
  bool extracted = checkExtracted();

  if (extracted == true) {
    std::map < int, std::string > mods;
    int modIndex = 1;

    for (const auto & entry: std::filesystem::directory_iterator("./mods")) {
      std::string modName = getModName(entry.path());
      mods[modIndex] = modName;
      modPaths[modName] = entry.path();
      modIndex++;
    }

    while (true) {
      std::cout << "\nMOD MANAGER\n\n";
      for (const auto & mod: mods) {
        std::cout << mod.first << ": " << mod.second << "\n";
      }
      std::cout << "Enter the number of the mod to install, -1 to uninstall all mods, -2 followed by the mod number to uninstall a single mod, or 0 to quit: ";
      int choice;
      std::cin >> choice;

      if (choice == 0) {
        std::string compileResponse;
        std::cout << "Do you want to compile your game rom? (yes/no): ";
        std::cin >> compileResponse;
        if (compileResponse == "yes" || compileResponse == "Yes") {
          std::cout << "Compiling your game rom...\n";
          int result = system("wit copy --align-files extracted spm.wbfs");
          if (result == -1) {
            std::cerr << "Error occurred during compiling of the game rom\n";
          }
        }
        break;
      } else if (choice == -1) {
        uninstallAllMods();
      } else if (choice == -2) {
        std::cout << "Enter the number of the mod to uninstall: ";
        std::cin >> choice;
        if (mods.find(choice) != mods.end()) {
          uninstallMod(mods[choice]);
        } else {
          std::cout << "Invalid mod number.\n";
        }
      } else if (mods.find(choice) != mods.end()) {
        installMod(mods[choice]);
      } else {
        std::cout << "Invalid choice.\n";
      }
    }
  }
  return 0;
}
