#ifndef KEYLOGGER_UTILS_H
#define KEYLOGGER_UTILS_H

#include <windows.h>
#include <string>
#include <unordered_map>

// Déclaration des fonctions
std::wstring GetSpecialKey(int key);
bool IsSpecialKey(int key);
wchar_t GetCharacterFromKey(int key, bool isShiftPressed, bool isCapsLockOn);

#endif // KEYLOGGER_UTILS_H
