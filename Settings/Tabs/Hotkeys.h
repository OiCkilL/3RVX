#pragma once

#include "Tab.h"

#include "../../3RVX/HotkeyInfo.h"
#include "../../3RVX/LanguageTranslator.h"

class Hotkeys : public Tab {

public:
    virtual void SaveSettings();

protected:
    virtual void Initialize();
    virtual void LoadSettings();

protected:
    std::vector<HotkeyInfo> _keyInfo;
    LanguageTranslator *_translator;

    HotkeyInfo *CurrentHotkeyInfo();

    void LoadSelection();
    void LoadSelection(int index);
    void LoadAction(int index, HotkeyInfo &selection);
    std::wstring ActionString(HotkeyInfo &selection);
    std::wstring VolumeActionString(HotkeyInfo &selection);

    void DefaultArgControlStates();
    void VolumeArgControlStates(HotkeyInfo &selection);

    void UpdateEditArgument();
    std::wstring OpenFileDialog();

private:
    /* Control Event Handlers */
    void OnKeyListItemChange(NMLISTVIEW *lv);
    void OnKeyListSelectionChange(int index);

    bool OnAddButtonClick();
    bool OnRemoveButtonClick();

    bool OnKeysButtonClick();
    bool OnActionChange();

    bool OnArgButtonClick();
    bool OnArgComboChange();
    bool OnArgCheckChange();
    bool OnArgEditTextChange();

protected:
    /* Controls: */
    ListView _keyList;
    Button _add;
    Button _remove;

    GroupBox _editorGroup;
    Label _keysLabel;
    Button _keys;
    Label _actionLabel;
    ComboBox _action;

    Label _argLabel;
    Checkbox _argCheck;
    ComboBox _argCombo;
    EditBox _argEdit;
    Button _argButton;

protected:
    /* Strings: */
    std::wstring _hotkeysColumnStr = L"Hotkeys";
    std::wstring _actionColumnStr = L"Action";
    std::wstring _amountVolArgStr = L"Amount:";
    std::wstring _unitsVolArgStr = L"Volume Units";
    std::wstring _percentVolArgStr = L"Percent";
    std::wstring _keyArgStr = L"Key:";
    std::wstring _driveArgStr = L"Drive:";
    std::wstring _pathArgStr = L"Path:";
    std::wstring _vkArgStr = L"VK Code (Hex):";

    std::wstring _ejectActionStr = L"Eject Drive: {1}";
    std::wstring _mediaActionStr = L"Media Key: {1}";
    std::wstring _runActionStr = L"Run: {1}";
    std::wstring _vkActionStr = L"Virtual Key: 0x{1}";
};