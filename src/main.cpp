#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
using namespace std;

namespace fs = filesystem;
map<string, string> modPaths;
/// File path to executable home directory
fs::path exeDir;
/// String prefix of executable file path
string exePrefix;
/// Locale and revision of image file
string gameVersion;

int clear()
{
  int ret = system("clear");
  if (ret != 0)
  {
    ret = system("cls");
  }
  return ret;
}

string readConfigFile()
{
  string line;
  ifstream config(exePrefix + "/config.ini");
  if (config.is_open())
  {
    getline(config, line);
    auto pos = line.find('=');
    if (pos != string::npos)
    {
      return line.substr(pos + 1);
    }
    config.close();
  }
  else
  {
    cout << "Unable to open config.ini\n";
  }
  return "";
}

void writeConfigFile(const string &version)
{
  ofstream config(exePrefix + "/config.ini");
  if (config.is_open())
  {
    config << "version=" << version << "\n";
    config.close();
  }
  else
  {
    cerr << "Unable to create config.ini\n";
  }
}

filesystem::path getFilesPath()
{
  fs::path dataPath(exePrefix + "/extracted/DATA/files");
  fs::path defaultPath(exePrefix + "/extracted/files");

  if (filesystem::exists(dataPath))
  {
    return dataPath;
  }
  else
  {
    return defaultPath;
  }
}

filesystem::path getSysPath()
{
  fs::path dataPath(exePrefix + "extracted/DATA/sys");
  fs::path defaultPath(exePrefix + "extracted/sys");

  if (filesystem::exists(dataPath))
  {
    return dataPath;
  }
  else
  {
    return defaultPath;
  }
}

void writePatchedFile()
{
  filesystem::path files = getFilesPath();
  ofstream patched(files.string() + "/" + "patched.ini");
  if (patched.is_open())
  {
    patched.close();
  }
  else
  {
    cerr << "Unable to create patched.ini\n";
  }
}

bool readPatchFile()
{
  filesystem::path files = getFilesPath();
  ifstream patched(files.string() + "/" + "patched.ini");
  if (patched.is_open())
  {
    patched.close();
  }
  else
  {
    return false;
  }
  return true;
}

void applyDolPatch()
{
  if (!readPatchFile())
  {
    filesystem::path sys = getSysPath();
    string wstrtCall = "wstrt patch " + sys.string() + "/main.dol --add-sect " + exePrefix + "/gct/" + gameVersion + ".gct";
    int result = system(wstrtCall.c_str());
    if (result == 0)
    {
      writePatchedFile();
    }
    else
    {
      result = system("wstrt");
      if (result != 0)
      {
        cerr << ("wstrt is not installed, please install it from the link in the instructions\n");
        abort();
      }
      else
      {
        cout << "Something went wrong when patching the main.dol. Full wstrt command is below\n";
        cerr << wstrtCall + "\n";
        abort();
      }
    }
  }
}

bool checkExtracted(int argc, char *argv[])
{
  fs::path extractedPath(exePrefix + "/extracted");
  fs::path isoPath(exePrefix + "/spm.iso");
  fs::path wbfsPath(exePrefix + "/spm.wbfs");

  if (fs::exists(extractedPath))
  {
    cout << "The extracted folder exists, reading contents...\n";
    return true;
  }
  else if (fs::exists(isoPath))
  {
    cout << "Extracting from spm.iso...\n";
    string witIsoCmd = "wit x " + isoPath.string() + " " + extractedPath.string();
    int result = system(witIsoCmd.c_str());
    if (result != 0)
    {
      result = system("wit");
      if (result != 0)
      {
        clear();
        cerr << "wit is not installed, please install it from the link in the instructions. On windows you will need to restart your computer after installing it.";
      }
      else
      {
        cerr << "Error occurred during extraction from spm.iso\n";
      }
      abort();
      return false;
    }
    else
    {
      return true;
    }
  }
  else if (fs::exists(wbfsPath))
  {
    cout << "Extracting from spm.wbfs...\n";
    string witWbfsCmd = "wit x " + wbfsPath.string() + " " + extractedPath.string();
    int result = system(witWbfsCmd.c_str());
    if (result != 0)
    {
      result = system("wit");
      if (result != 0)
      {
        clear();
        cerr << "wit is not installed, please install it from the link in the instructions. On windows you will need to restart your computer after installing it.";
      }
      else
      {
        cerr << "Error occurred during extraction from spm.wbfs\n";
      }
      abort();
      return false;
    }
    else
    {
      return true;
    }
  }
  else if (argc > 1)
  {
    fs::path argPath(argv[1]);
    if (fs::exists(argPath) && (argPath.extension() == ".iso" || argPath.extension() == ".wbfs"))
    {
      string pathStr = argPath.string();
      replace(pathStr.begin(), pathStr.end(), '\\', '/');

      cout << "Extracting from " << pathStr << "...\n";
      string command = "wit x \"" + pathStr + "\" " + extractedPath.string();
      int result = system(command.c_str());
      if (result != 0)
      {
        cerr << "Error occurred during extraction from " << pathStr << '\n';
        string response;
        cout << "Enter 0 to exit\n";
        cin >> response;
        return false;
      }
      else
      {
        return true;
      }
    }
    else
    {
      return false;
    }
  }

  if (fs::exists(extractedPath))
  {
    return true;
  }
  else
  {
    cout << "No extracted folder could be found.\n";
    string response;
    cout << "Enter 0 to exit\n";
    cin >> response;
    return false;
  }
}

string getModName(const string &modFolder, const string &gameVersion)
{
  // Check for the game version folder before considering the mod valid
  if (!filesystem::exists(modFolder + "/" + gameVersion))
  {
    return "";
  }

  string line;
  ifstream modinfo(modFolder + "/modinfo.ini");
  if (modinfo.is_open())
  {
    getline(modinfo, line);
    auto pos = line.find('=');
    if (pos != string::npos)
    {
      return line.substr(pos + 1);
    }
    modinfo.close();
  }
  else
  {
    cout << "Unable to open modinfo.ini file for mod: " << modFolder << endl;
  }
  return "";
}

void installMod(const string &modName)
{
  cout << "Installing " << modName << "...\n";

  filesystem::path backupPath(exePrefix + "/backup");
  filesystem::path extractedPath = getFilesPath();

  if (!filesystem::exists(backupPath))
  {
    filesystem::create_directory(backupPath);
  }

  filesystem::path modPath = modPaths[modName];

  for (const auto &entry : filesystem::recursive_directory_iterator(modPath))
  {
    if (entry.is_regular_file())
    {
      filesystem::path filePath = entry.path();
      if (filePath.filename() != "modinfo.ini")
      {
        filesystem::path relPath = filesystem::relative(filePath, modPath);
        filesystem::path destination = extractedPath / relPath;
        if (filesystem::exists(destination))
        {
          filesystem::path backupDest = backupPath / relPath;
          if (!filesystem::exists(backupDest) && filePath.filename() != "mod.rel")
          { // Check if file already exists in backup, check if its a mod.rel, if either is true then we dont need to backup
            filesystem::create_directories(backupDest.parent_path());
            filesystem::rename(destination, backupDest);
          }
        }
        filesystem::create_directories(destination.parent_path());
        filesystem::copy(filePath, destination, filesystem::copy_options::overwrite_existing);
      }
    }
  }
  applyDolPatch();
  clear();
  cout << "Installed " << modName << "\n";
}

void uninstallAllMods()
{
  clear();
  cout << "Uninstalling all mods...\n";
  filesystem::path backupPath(exePrefix + "/backup");
  filesystem::path extractedPath = getFilesPath();

  if (filesystem::exists(backupPath))
  {
    for (const auto &entry : filesystem::recursive_directory_iterator(backupPath))
    {
      if (entry.is_regular_file())
      {
        filesystem::path filePath = entry.path();
        filesystem::path relPath = filesystem::relative(filePath, backupPath);
        filesystem::path destination = extractedPath / relPath;
        filesystem::create_directories(destination.parent_path());
        filesystem::copy(filePath, destination, filesystem::copy_options::overwrite_existing);
      }
    }
    cout << "All mods uninstalled, original files restored.\n";
  }
  else
  {
    cout << "Backup folder doesn't exist, nothing to restore.\n";
  }

  filesystem::path modFolderPath = extractedPath / "mod";
  if (filesystem::exists(modFolderPath))
  {
    filesystem::remove_all(modFolderPath);
  }
}

void uninstallMod(const string &modName)
{
  cout << "Uninstalling " << modName << "...\n";

  filesystem::path backupPath(exePrefix + "/backup");
  filesystem::path extractedPath = getFilesPath();
  filesystem::path modPath = modPaths[modName];
  filesystem::path modFolderPath = extractedPath / "mod";

  if (filesystem::exists(backupPath))
  {
    for (const auto &entry : filesystem::recursive_directory_iterator(modPath))
    {
      if (entry.is_regular_file())
      {
        filesystem::path filePath = entry.path();
        filesystem::path relPath = filesystem::relative(filePath, modPath);
        filesystem::path backupDest = backupPath / relPath;
        if (filesystem::exists(backupDest))
        {
          filesystem::path destination = extractedPath / relPath;
          // Skip if destination is within modFolderPath
          if (destination.string().find(modFolderPath.string()) != 0)
          {
            filesystem::copy(backupDest, destination, filesystem::copy_options::overwrite_existing);
          }
        }
      }
    }
    clear();
    cout << "Mod " << modName << " uninstalled, original files restored.\n";
    if (filesystem::exists(modFolderPath))
    {
      filesystem::remove_all(modFolderPath);
    }
  }
  else
  {
    cout << "Backup folder doesn't exist, nothing to restore.\n";
  }
}

int main(int argc, char *argv[])
{
  // Set globals
  exeDir = fs::canonical(fs::path(argv[0])).parent_path();
  exePrefix = exeDir.string();
  // readConfigFile uses exePrefix, order matters
  gameVersion = readConfigFile();
  if (gameVersion.empty())
  {
    cout << "Examples of valid versions include US2, US0, JP0, EU0\n";
    cout << "Please enter your SPM version: ";
    cin >> gameVersion;
    for (auto &c : gameVersion)
    {
      c = toupper(c);
    }
    writeConfigFile(gameVersion);
  }
  clear();
  bool extracted = checkExtracted(argc, argv);

  if (extracted == true)
  {
    map<int, string> mods;
    int modIndex = 1;

    for (const auto &entry : filesystem::directory_iterator(fs::path(exePrefix + "/mods")))
    {
      string modName = getModName(entry.path().string(), gameVersion); // Pass game version
      if (!modName.empty())
      { // Only add the mod if the game version folder exists
        mods[modIndex] = modName;
        modPaths[modName] = (entry.path() / gameVersion).string(); // Append game version to mod path
        modIndex++;
      }
    }

    while (true)
    {
      cout << "\nMOD MANAGER\n\n";
      for (const auto &mod : mods)
      {
        cout << mod.first << ": " << mod.second << "\n";
      }
      cout << "Enter the number of the mod to install, -1 to uninstall all mods, -2 followed by the mod number to uninstall a single mod, or 0 to compile: ";
      // sudo install turtle
      int choice;
      cin >> choice;

      if (choice == 0)
      {
        string compileResponse;
        cout << "Do you want to compile your game rom? (yes/no): ";
        cin >> compileResponse;
        if (compileResponse == "yes" || compileResponse == "Yes")
        {
          cout << "Compiling your game rom...\n";
          string witCpyCmd = "wit copy --align-files " + exePrefix + "/extracted " + exePrefix + "/patched.wbfs";
          int result = system(witCpyCmd.c_str());
          if (result != 0)
          {
            result = system("wit");
            if (result != 0)
            {
              clear();
              cerr << "wit is not installed, please install it from the link in the instructions. On windows you will need to restart your computer after installing it.";
            }
            else
            {
              cerr << "Error occurred during compiling of the game rom\n";
            }
            abort();
          }
          return 0;
        }
        break;
      }
      else if (choice == -1)
      {
        uninstallAllMods();
      }
      else if (choice == -2)
      {
        cout << "Enter the number of the mod to uninstall: ";
        cin >> choice;
        if (mods.find(choice) != mods.end())
        {
          uninstallMod(mods[choice]);
        }
        else
        {
          cout << "Invalid mod number.\n";
        }
      }
      else if (mods.find(choice) != mods.end())
      {
        installMod(mods[choice]);
      }
      else
      {
        cout << "Invalid choice.\n";
      }
    }
  }
  else
  {
    cout << "\nextracted != true\n\n";
  }
  return 0;
}
