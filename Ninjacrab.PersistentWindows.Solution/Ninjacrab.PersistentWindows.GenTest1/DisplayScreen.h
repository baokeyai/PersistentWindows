#pragma once
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <unordered_map>

class DisplayScreen
{
private:
    int screenWidth;
    int screenHeight;
    int left;
    int top;
    short flags;
    std::string deviceName;
public:
    DisplayScreen();
    ~DisplayScreen();

    static std::unordered_map<std::string, MONITORINFOEX> AllDisplays;
    static void RefreshDisplays();
};

