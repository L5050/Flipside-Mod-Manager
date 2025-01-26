#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
using namespace std;

namespace fs = filesystem;
map < string, string > modPaths;

string readConfigFile() {
  string line;
  ifstream config("config.ini");
  if (config.is_open()) {
    getline(config, line);
    auto pos = line.find('=');
    if (pos != string::npos) {
      return line.substr(pos + 1);
    }
    config.close();
  } else {
    cout << "Unable to open config.ini\n";
  }
  return "";
}
string gameVersion = readConfigFile();

void writeConfigFile(const string & version) {
  ofstream config("config.ini");
  if (config.is_open()) {
    config << "version=" << version << "\n";
    config.close();
  } else {
    cerr << "Unable to create config.ini\n";
  }
}

filesystem::path getFilesPath() {
  fs::path dataPath("extracted/DATA/files");
  fs::path defaultPath("extracted/files");

  if (filesystem::exists(dataPath)) {
    return dataPath;
  } else {
    return defaultPath;
  }
}

bool checkExtracted(int argc, char * argv[]) {
  fs::path extractedPath("extracted");
  fs::path isoPath("spm.iso");
  fs::path wbfsPath("spm.wbfs");

  if (fs::exists(extractedPath)) {
    cout << "The extracted folder exists, reading contents...\n";
    return true;
  } else if (fs::exists(isoPath)) {
    cout << "Extracting from spm.iso...\n";
    int result = system("wit x spm.iso extracted");
    if (result != 0) {
      cerr << "Error occurred during extraction from spm.iso\n";
      return false;
    } else {
      return true;
    }
  } else if (fs::exists(wbfsPath)) {
    cout << "Extracting from spm.wbfs...\n";
    int result = system("wit x spm.wbfs extracted");
    if (result != 0) {
      cerr << "Error occurred during extraction from spm.wbfs\n";
      return false;
    } else {
      return true;
    }
  } else if (argc > 1) {
    fs::path argPath(argv[1]);
    if (fs::exists(argPath) && (argPath.extension() == ".iso" || argPath.extension() == ".wbfs")) {
      string pathStr = argPath.string();
      replace(pathStr.begin(), pathStr.end(), '\\', '/');

      cout << "Extracting from " << pathStr << "...\n";
      string command = "wit x \"" + pathStr + "\" extracted";
      int result = system(command.c_str());
      if (result != 0) {
        cerr << "Error occurred during extraction from " << pathStr << '\n';
        string responsee;
        cout << "Press enter to continue\n";
        cin >> responsee;
        return false;
      } else {
        return true;
      }
    } else {
      return false;
    }
  }

  if (fs::exists(extractedPath)) {
    return true;
  } else {
    cout << "No extracted folder could be found.\n";
    string response;
    cout << "Press enter to continue\n";
    cin >> response;
    return false;
  }
}

string getModName(const string & modFolder, const string & gameVersion) {
  // Check for the game version folder before considering the mod valid
  if (!filesystem::exists(modFolder + "/" + gameVersion)) {
    return "";
  }

  string line;
  ifstream modinfo(modFolder + "/modinfo.ini");
  if (modinfo.is_open()) {
    getline(modinfo, line);
    auto pos = line.find('=');
    if (pos != string::npos) {
      return line.substr(pos + 1);
    }
    modinfo.close();
  } else {
    cout << "Unable to open modinfo.ini file for mod: " << modFolder << endl;
  }
  return "";
}

void installMod(const string & modName) {
  cout << "Installing " << modName << "...\n";

  filesystem::path backupPath("backup");
  filesystem::path extractedPath = getFilesPath();

  if (!filesystem::exists(backupPath)) {
    filesystem::create_directory(backupPath);
  }

  filesystem::path modPath = modPaths[modName];

  for (const auto & entry: filesystem::recursive_directory_iterator(modPath)) {
    if (entry.is_regular_file()) {
      filesystem::path filePath = entry.path();
      if (filePath.filename() != "modinfo.ini") {
        filesystem::path relPath = filesystem::relative(filePath, modPath);
        filesystem::path destination = extractedPath / relPath;
        if (filesystem::exists(destination)) {
          filesystem::path backupDest = backupPath / relPath;
          if (!filesystem::exists(backupDest) && filePath.filename() != "mod.rel") { // Check if file already exists in backup, check if its a mod.rel, if either is true then we dont need to backup
            filesystem::create_directories(backupDest.parent_path());
            filesystem::rename(destination, backupDest);
          }
        }
        filesystem::create_directories(destination.parent_path());
        filesystem::copy(filePath, destination, filesystem::copy_options::overwrite_existing);
      }
    }
  }
  system("clear");
  cout << "Installed " << modName << "\n";

}

void uninstallAllMods() {
  cout << "Uninstalling all mods...\n";
  filesystem::path backupPath("backup");
  filesystem::path extractedPath = getFilesPath();

  if (filesystem::exists(backupPath)) {
    for (const auto & entry: filesystem::recursive_directory_iterator(backupPath)) {
      if (entry.is_regular_file()) {
        filesystem::path filePath = entry.path();
        filesystem::path relPath = filesystem::relative(filePath, backupPath);
        filesystem::path destination = extractedPath / relPath;
        filesystem::create_directories(destination.parent_path());
        filesystem::copy(filePath, destination, filesystem::copy_options::overwrite_existing);
      }
    }
    cout << "All mods uninstalled, original files restored.\n";
  } else {
    cout << "Backup folder doesn't exist, nothing to restore.\n";
  }

  filesystem::path modFolderPath = extractedPath / "mod";
  if (filesystem::exists(modFolderPath)) {
    filesystem::remove_all(modFolderPath);
  }
  string responsee;
  cout << "Press enter to continue\n";
  cin >> responsee;
  system("clear");
}

void uninstallMod(const string & modName) {
  cout << "Uninstalling " << modName << "...\n";

  filesystem::path backupPath("backup");
  filesystem::path extractedPath = getFilesPath();
  filesystem::path modPath = modPaths[modName];
  filesystem::path modFolderPath = extractedPath / "mod";

  if (filesystem::exists(backupPath)) {
    for (const auto & entry: filesystem::recursive_directory_iterator(modPath)) {
      if (entry.is_regular_file()) {
        filesystem::path filePath = entry.path();
        filesystem::path relPath = filesystem::relative(filePath, modPath);
        filesystem::path backupDest = backupPath / relPath;
        if (filesystem::exists(backupDest)) {
          filesystem::path destination = extractedPath / relPath;
          // Skip if destination is within modFolderPath
          if (destination.string().find(modFolderPath.string()) != 0) {
            filesystem::copy(backupDest, destination, filesystem::copy_options::overwrite_existing);
          }
        }
      }
    }
    cout << "Mod " << modName << " uninstalled, original files restored.\n";
    if (filesystem::exists(modFolderPath)) {
      filesystem::remove_all(modFolderPath);
    }
  } else {
    cout << "Backup folder doesn't exist, nothing to restore.\n";
  }
  string responsee;
  cout << "Press enter to continue\n";
  cin >> responsee;
  system("clear");
}

int main(int argc, char * argv[]) {
  if (gameVersion.empty()) {
    cout << "Could not determine game version. Would you like to enter it manually? (yes/no): ";
    string response;
    cin >> response;
    if (response == "yes" || response == "Yes") {
      cout << "Please reopen Flipside Mod Manager after choosing your SPM version\n";
      cout << "Examples of valid versions include US2, US0, JP0, EU0\n";
      cout << "Please enter your SPM version: ";
      cin >> gameVersion;
      writeConfigFile(gameVersion);

    } else {
      cout << "Exiting.\n";
      return 1;
    }
  }

  bool extracted = checkExtracted(argc, argv);

  if (extracted == true) {
    map < int, string > mods;
    int modIndex = 1;

    for (const auto & entry: filesystem::directory_iterator("./mods")) {
      string modName = getModName(entry.path().string(), gameVersion); // Pass game version
      if (!modName.empty()) { // Only add the mod if the game version folder exists
        mods[modIndex] = modName;
        modPaths[modName] = (entry.path() / gameVersion).string(); // Append game version to mod path
        modIndex++;
      }
    }

    while (true) {
      cout << "\nMOD MANAGER\n\n";
      for (const auto & mod: mods) {
        cout << mod.first << ": " << mod.second << "\n";
      }
      cout << "Enter the number of the mod to install, -1 to uninstall all mods, -2 followed by the mod number to uninstall a single mod, or 0 to compile: ";
      int choice;
      cin >> choice;

      if (choice == 0) {
        string compileResponse;
        cout << "Do you want to compile your game rom? (yes/no): ";
        cin >> compileResponse;
        if (compileResponse == "yes" || compileResponse == "Yes") {
          cout << "Compiling your game rom...\n";
          int result = system("wit copy --align-files extracted patched.wbfs");
          if (result != 0) {
            cerr << "Error occurred during compiling of the game rom\n";
          }
          cout << "Press any input to exit\n";
          cin >> compileResponse;
        }
        break;
      } else if (choice == -1) {
        uninstallAllMods();
      } else if (choice == -2) {
        cout << "Enter the number of the mod to uninstall: ";
        cin >> choice;
        if (mods.find(choice) != mods.end()) {
          uninstallMod(mods[choice]);
        } else {
          cout << "Invalid mod number.\n";
        }
      } else if (mods.find(choice) != mods.end()) {
        installMod(mods[choice]);
      } else {
        cout << "Invalid choice.\n";
      }
    }
  } else {
    cout << "\nextracted != true\n\n";
  }
  return 0;
}
