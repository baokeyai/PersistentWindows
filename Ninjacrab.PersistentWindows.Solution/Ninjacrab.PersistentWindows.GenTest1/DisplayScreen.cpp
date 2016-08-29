#include "DisplayScreen.h"
#include <string>
#include <unordered_map>
#include <windows.h>
#include <shellapi.h>

std::unordered_map<std::string, MONITORINFOEX> DisplayScreen::AllDisplays;

DisplayScreen::DisplayScreen()
{
}


DisplayScreen::~DisplayScreen()
{
}



void DisplayScreen::RefreshDisplays()
{
    EnumDisplayMonitors(NULL, NULL, 
        [](HMONITOR monitorHandle, HDC monitorDeviceContextHandle, LPRECT monitorRect, LPARAM data)->BOOL 
        {
            MONITORINFOEX monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            if (GetMonitorInfo(monitorHandle, &monitorInfo))
            {
                DisplayScreen::AllDisplays.insert_or_assign(monitorInfo.szDevice, monitorInfo);
            }
            return true;
        }, 
        NULL);
}
