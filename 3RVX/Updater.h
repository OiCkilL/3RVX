#pragma once

#include <string>
#include <utility>

/// <summary>
/// Handles update functionality: checking for updates online, and downloading
/// new versions of the software.
/// <p>
/// The latest version number of 3RVX can be retrieved from:
/// http://matthew.malensek.net/projects/3RVX/latest_version
/// <p>
/// Versions can be downloaded from:
/// http://matthew.malensek.net/projects/3RVX/3RVX-X.Y.{zip, msi}
/// Where X is the major version, Y is the minor version, and the type of
/// installation is determined by the package extension:
/// <ul>
///   <li>.msi - Installer based version</li>
///   <li>.zip - Portable (non-installer) version</li>
/// </ul>
/// </summary>
class Updater {
public:
    /// <summary>
    /// Determines whether a new version of the program is available online
    /// or not.
    /// </summary>
    static bool NewerVersionAvailable();

    /// <summary>
    /// Retrieves the version number of the main application (3RVX.exe).
    /// </summary>
    static std::pair<int, int> MainAppVersion();

    /// <summary>
    /// Retrieves the version number of the main application (3RVX.exe) as a 
    /// string in the format X.Y (where X is the major version number and Y
    /// is the minor version number).
    /// </summary>
    static std::wstring MainAppVersionString();

private:
    /// <summary>
    /// Retrieves the latest version of the program availabile online.
    /// </summary>
    static std::pair<int, int> RemoteVersion();

};