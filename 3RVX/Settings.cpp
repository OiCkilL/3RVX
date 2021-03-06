#include "Settings.h"

#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <algorithm>

#include "Error.h"
#include "HotkeyInfo.h"
#include "LanguageTranslator.h"
#include "Logger.h"
#include "Monitor.h"
#include "Skin.h"
#include "StringUtils.h"

#define XML_AUDIODEV "audioDeviceID"
#define XML_HIDE_WHENFULL "hideFullscreen"
#define XML_HIDEANIM "hideAnimation"
#define XML_HIDETIME "hideDelay"
#define XML_HIDESPEED "hideSpeed"
#define XML_LANGUAGE "language"
#define XML_MONITOR "monitor"
#define XML_NOTIFYICON "notifyIcon"
#define XML_ONTOP "onTop"
#define XML_OSD_OFFSET "osdEdgeOffset"
#define XML_OSD_POS "osdPosition"
#define XML_OSD_X "osdX"
#define XML_OSD_Y "osdY"
#define XML_SKIN "skin"
#define XML_SOUNDS "soundEffects"

const std::wstring Settings::MAIN_APP = L"3RVX.exe";
const std::wstring Settings::SETTINGS_APP = L"Settings.exe";
const std::wstring Settings::SETTINGS_FILE = L"Settings.xml";
const std::wstring Settings::LANG_DIR = L"Languages";
const std::wstring Settings::SKIN_DIR = L"Skins";

const std::wstring Settings::DefaultLanguage = L"English";
const std::wstring Settings::DefaultSkin = L"Classic";

std::wstring Settings::_appDir(L"");
Settings *Settings::instance;

std::vector<std::wstring> Settings::OSDPosNames = {
    L"Top",
    L"Left",
    L"Right",
    L"Bottom",
    L"Center",
    L"Top-left",
    L"Top-right",
    L"Bottom-left",
    L"Bottom-right",
    L"Custom"
};

Settings *Settings::Instance() {
    if (instance == NULL) {
        instance = new Settings();
    }
    return instance;
}

void Settings::Load() {
    /* First, clean up (if needed) */
    delete _translator;
    _translator = NULL;

    _file = SettingsFile();
    CLOG(L"Loading settings: %s", _file.c_str());

    std::string u8FileName = StringUtils::Narrow(_file);
    tinyxml2::XMLError result = _xml.LoadFile(u8FileName.c_str());
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        LoadEmptySettings();
        return;
    }

    _root = _xml.GetDocument()->FirstChildElement("settings");
    if (_root == NULL) {
        Error::ErrorMessage(GENERR_MISSING_XML, L"<settings>");
        LoadEmptySettings();
        return;
    }
}

void Settings::LoadEmptySettings() {
    _xml.Clear();
    _xml.InsertFirstChild(_xml.NewDeclaration());
    _root = _xml.NewElement("settings");
    _xml.GetDocument()->InsertEndChild(_root);
}

int Settings::Save() {
    CreateSettingsDir();
    FILE *stream;
    errno_t err = _wfopen_s(&stream, _file.c_str(), L"w");
    if (err != 0 || stream == NULL) {
        CLOG(L"Could not open settings file for writing!");
        return 100 + err;
    }
    tinyxml2::XMLError result = _xml.SaveFile(stream);
    fclose(stream);
    return result;
}

std::wstring Settings::SettingsDir() {
    /* First, is this a portable installation? */
    std::wstring portableSettings = AppDir() + L"\\" + SETTINGS_FILE;
    if (PathFileExists(portableSettings.c_str()) == TRUE) {
        return AppDir();
    }

    /* If the install isn't portable, use the roaming appdata directory. */
    wchar_t appData[MAX_PATH];
    HRESULT hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, appData);
    if (FAILED(hr)) {
        HRESULT hr = SHGetFolderPath(
            NULL, CSIDL_LOCAL_APPDATA, NULL, NULL, appData);

        if (FAILED(hr)) {
            // TODO: This probably warrants an error message!
            return AppDir();
        }
    }

    return std::wstring(appData) + L"\\3RVX";
}

void Settings::CreateSettingsDir() {
    std::wstring settingsDir = SettingsDir();
    CLOG(L"Creating settings directory: %s", settingsDir.c_str());

    settingsDir = L"\\\\?\\" + settingsDir; /* Use long file path (\\?\) */
    BOOL result = CreateDirectory(settingsDir.c_str(), NULL);
    if (result == FALSE) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            QCLOG(L"Directory already exists.");
            return;
        }

        if (GetLastError() == ERROR_PATH_NOT_FOUND) {
            QCLOG(L"Path not found!");
            // TODO: error message?
        }
    }
}

std::wstring Settings::SettingsFile() {
    return SettingsDir() + std::wstring(L"\\") + SETTINGS_FILE;
}

std::wstring Settings::AppDir() {
    if (_appDir.empty()) {
        wchar_t path[MAX_PATH] = { 0 };
        if (GetModuleFileName(NULL, path, MAX_PATH)) {
            PathRemoveFileSpec(path);
        }
        _appDir = std::wstring(path);
    }
    return _appDir;
}

std::wstring Settings::SkinDir() {
    return AppDir() + L"\\" + SKIN_DIR;
}

std::wstring Settings::MainApp() {
    return Settings::AppDir() + L"\\" + MAIN_APP;
}

std::wstring Settings::SettingsApp() {
    return Settings::AppDir() + L"\\" + SETTINGS_APP;
}

std::wstring Settings::LanguagesDir() {
    return AppDir() + L"\\" + LANG_DIR;
}

void Settings::LaunchSettingsApp() {
    std::wstring app = SettingsApp();

    CLOG(L"Opening Settings App: %s", app.c_str());
    int exec = (int) ShellExecute(
        NULL, L"open", app.c_str(), NULL, NULL, SW_SHOWNORMAL);

    if (exec <= 32) {
        Error::ErrorMessage(GENERR_NOTFOUND, app);
    }
}

std::wstring Settings::AudioDeviceID() {
    return GetText(XML_AUDIODEV);
}

std::wstring Settings::LanguageName() {
    std::wstring lang = GetText(XML_LANGUAGE);

    if (lang == L"") {
        return DefaultLanguage;
    } else {
        return lang;
    }
}

void Settings::LanguageName(std::wstring name) {
    std::string nName = StringUtils::Narrow(name);
    SetText(XML_LANGUAGE, nName);
}

bool Settings::AlwaysOnTop() {
    return GetEnabled(XML_ONTOP, DefaultOnTop);
}

void Settings::AlwaysOnTop(bool enable) {
    SetEnabled(XML_ONTOP, enable);
}

bool Settings::HideFullscreen() {
    return GetEnabled(XML_HIDE_WHENFULL, DefaultHideFullscreen);
}

void Settings::HideFullscreen(bool enable) {
    SetEnabled(XML_HIDE_WHENFULL, enable);
}

std::wstring Settings::Monitor() {
    return GetText(XML_MONITOR);
}

void Settings::Monitor(std::wstring monitorName) {
    SetText(XML_MONITOR, StringUtils::Narrow(monitorName));
}

int Settings::OSDEdgeOffset() {
    if (HasSetting(XML_OSD_OFFSET)) {
        return GetInt(XML_OSD_OFFSET);
    } else {
        return DefaultOSDOffset;
    }
}

void Settings::OSDEdgeOffset(int offset) {
    SetInt(XML_OSD_OFFSET, offset);
}

Settings::OSDPos Settings::OSDPosition() {
    std::wstring pos = GetText(XML_OSD_POS);
    const wchar_t *posStr = pos.c_str();

    for (unsigned int i = 0; i < OSDPosNames.size(); ++i) {
        if (_wcsicmp(posStr, OSDPosNames[i].c_str()) == 0) {
            return (Settings::OSDPos) i;
        }
    }

    return DefaultOSDPosition;
}

void Settings::OSDPosition(OSDPos pos) {
    std::wstring posStr = OSDPosNames[(int) pos];
    SetText(XML_OSD_POS, StringUtils::Narrow(posStr));
}

int Settings::OSDX() {
    return GetInt(XML_OSD_X);
}

void Settings::OSDX(int x) {
    SetInt(XML_OSD_X, x);
}

int Settings::OSDY() {
    return GetInt(XML_OSD_Y);
}

void Settings::OSDY(int y) {
    SetInt(XML_OSD_Y, y);
}

AnimationTypes::HideAnimation Settings::HideAnim() {
    std::wstring anim = GetText(XML_HIDEANIM);
    const wchar_t *animStr = anim.c_str();

    std::vector<std::wstring> *names = &AnimationTypes::HideAnimationNames;
    for (unsigned int i = 0; i < names->size(); ++i) {
        if (_wcsicmp(animStr, (*names)[i].c_str()) == 0) {
            return (AnimationTypes::HideAnimation) i;
        }
    }

    return DefaultHideAnim;
}

void Settings::HideAnim(AnimationTypes::HideAnimation anim) {
    std::wstring hideStr = AnimationTypes::HideAnimationNames[(int) anim];
    SetText(XML_HIDEANIM, StringUtils::Narrow(hideStr));
}

int Settings::HideDelay() {
    return GetInt(XML_HIDETIME, DefaultHideTime);
}

void Settings::HideDelay(int delay) {
    SetInt(XML_HIDETIME, delay);
}

int Settings::HideSpeed() {
    return GetInt(XML_HIDESPEED, DefaultHideSpeed);
}

void Settings::HideSpeed(int speed) {
    SetInt(XML_HIDESPEED, speed);
}

bool Settings::CurrentSkin(std::wstring skinName) {
    std::string name = StringUtils::Narrow(skinName);
    std::wstring xml = SkinXML(skinName);
    if (PathFileExists(xml.c_str()) == FALSE) {
        return false;
    }

    SetText(XML_SKIN, name);
    return true;
}

std::wstring Settings::CurrentSkin() {
    std::wstring name = GetText("skin");

    if (name == L"") {
        return DefaultSkin;
    } else {
        return name;
    }
}

std::wstring Settings::SkinXML() {
    return SkinXML(CurrentSkin());
}

std::wstring Settings::SkinXML(std::wstring skinName) {
    std::wstring skinXML = Settings::AppDir() + L"\\" + SKINS_DIR L"\\"
        + skinName + L"\\" SKIN_XML;
    return skinXML;
}

std::unordered_map<int, HotkeyInfo> Settings::Hotkeys() {
    std::unordered_map<int, HotkeyInfo> keyMappings;

    if (_root == NULL) {
        return keyMappings;
    }

    tinyxml2::XMLElement *hotkeys = _root->FirstChildElement("hotkeys");
    if (hotkeys == NULL) {
        return keyMappings;
    }

    tinyxml2::XMLElement *hotkey = hotkeys->FirstChildElement("hotkey");
    for (; hotkey != NULL; hotkey = hotkey->NextSiblingElement()) {
        const char *actionStr = hotkey->Attribute("action");
        if (actionStr == NULL) {
            CLOG(L"No action provided for hotkey; skipping");
            continue;
        }

        int action = -1;
        std::wstring wActionStr = StringUtils::Widen(actionStr);
        for (unsigned int i = 0; i < HotkeyInfo::ActionNames.size(); ++i) {
            const wchar_t *currentAction = HotkeyInfo::ActionNames[i].c_str();
            if (_wcsicmp(wActionStr.c_str(), currentAction) == 0) {
                action = i;
                break;
            }
        }

        if (action == -1) {
            CLOG(L"Hotkey action '%s' not recognized; skipping",
                wActionStr.c_str());
            continue;
        }

        int combination = -1;
        hotkey->QueryIntAttribute("combination", &combination);
        if (combination == -1) {
            CLOG(L"No key combination provided for hotkey; skipping");
            continue;
        }

        HotkeyInfo hki;
        hki.action = action;
        hki.keyCombination = combination;

        /* Does this hotkey action have any arguments? */
        tinyxml2::XMLElement *arg = hotkey->FirstChildElement("arg");
        for (; arg != NULL; arg = arg->NextSiblingElement()) {
            const char *argStr = arg->GetText();
            hki.args.push_back(StringUtils::Widen(argStr));
        }

        /* Do a validity check on the finished HKI object */
        if (hki.Valid() == false) {
            continue;
        }

        /* Whew, we made it! */
        CLOG(L"%s", hki.ToString().c_str());
        keyMappings[combination] = hki;
    }

    return keyMappings;
}

void Settings::Hotkeys(std::vector<HotkeyInfo> hotkeys) {
    tinyxml2::XMLElement *hkElem = GetOrCreateElement("hotkeys");
    hkElem->DeleteChildren();

    for (HotkeyInfo hotkey : hotkeys) {
        if (hotkey.Valid() == false) {
            continue;
        }

        tinyxml2::XMLElement *hk = _xml.NewElement("hotkey");

        hk->SetAttribute("combination", hotkey.keyCombination);
        std::string actionStr = StringUtils::Narrow(
            HotkeyInfo::ActionNames[hotkey.action]);
        hk->SetAttribute("action", actionStr.c_str());

        if (hotkey.args.size() > 0) {
            for (std::wstring arg : hotkey.args) {
                tinyxml2::XMLElement *argElem = _xml.NewElement("arg");
                argElem->SetText(StringUtils::Narrow(arg).c_str());
                hk->InsertEndChild(argElem);
            }
        }

        hkElem->InsertEndChild(hk);
    }
}

LanguageTranslator *Settings::Translator() {
    if (_translator == NULL) {
        std::wstring langDir = Settings::LanguagesDir();
        std::wstring lang = Settings::LanguageName();
        std::wstring langFile = langDir + L"\\" + lang + L".xml";
        if (PathFileExists(langFile.c_str()) == FALSE) {
            _translator = new LanguageTranslator();
        } else {
            _translator = new LanguageTranslator(langFile);
            _translator->LoadTranslations();
        }
    }

    return _translator;
}

bool Settings::NotifyIconEnabled() {
    return GetEnabled(XML_NOTIFYICON, DefaultNotifyIcon);
}

void Settings::NotifyIconEnabled(bool enable) {
    SetEnabled(XML_NOTIFYICON, enable);
}

bool Settings::SoundEffectsEnabled() {
    return GetEnabled(XML_SOUNDS, DefaultSoundsEnabled);
}

void Settings::SoundEffectsEnabled(bool enable) {
    SetEnabled(XML_SOUNDS, enable);
}

bool Settings::HasSetting(std::string elementName) {
    if (_root == NULL) {
        return false;
    }

    tinyxml2::XMLElement *el = _root->FirstChildElement(elementName.c_str());
    return (el != NULL);
}

bool Settings::GetEnabled(std::string elementName, const bool defaultSetting) {
    if (_root == NULL) {
        return defaultSetting;
    }

    tinyxml2::XMLElement *el = _root->FirstChildElement(elementName.c_str());
    if (el == NULL) {
        std::wstring elStr = StringUtils::Widen(elementName);
        CLOG(L"Warning: XML element '%s' not found", elStr.c_str());
        return defaultSetting;
    } else {
        bool val = false;
        el->QueryBoolText(&val);
        return val;
    }
}

void Settings::SetEnabled(std::string elementName, bool enabled) {
    tinyxml2::XMLElement *el = GetOrCreateElement(elementName);
    el->SetText(enabled ? "true" : "false");
}

std::wstring Settings::GetText(std::string elementName) {
    if (_root == NULL) {
        return L"";
    }

    tinyxml2::XMLElement *el = _root->FirstChildElement(elementName.c_str());
    if (el == NULL) {
        CLOG(L"Warning: XML element %s not found",
            StringUtils::Widen(elementName).c_str());
        return L"";
    }

    const char* str = el->GetText();
    if (str == NULL) {
        return L"";
    } else {
        return StringUtils::Widen(str);
    }
}

void Settings::SetText(std::string elementName, std::string text) {
    tinyxml2::XMLElement *el = GetOrCreateElement(elementName);
    el->SetText(text.c_str());
}

int Settings::GetInt(std::string elementName, const int defaultValue) {
    if (_root == NULL) {
        return defaultValue;
    }

    tinyxml2::XMLElement *el = _root->FirstChildElement(elementName.c_str());
    if (el == NULL) {
        std::wstring elStr = StringUtils::Widen(elementName);
        CLOG(L"Warning: XML element '%s' not found", elStr.c_str());
        return defaultValue;
    }

    int val = defaultValue;
    el->QueryIntText(&val);
    return val;
}

void Settings::SetInt(std::string elementName, int value) {
    tinyxml2::XMLElement *el = GetOrCreateElement(elementName);
    el->SetText(value);
}

tinyxml2::XMLElement *Settings::GetOrCreateElement(std::string elementName) {
    tinyxml2::XMLElement *el = _root->FirstChildElement(elementName.c_str());
    if (el == NULL) {
        el = _xml.NewElement(elementName.c_str());
        _root->InsertEndChild(el);
    }
    return el;
}