#pragma region include and define
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>

using namespace std;

#pragma comment(linker, "/SECTION:.SHARED,RWS")
#pragma data_seg(".SHARED")
HHOOK globalHook = 0;
#pragma data_seg()

//
// instance specific data
//
HMODULE hInstance = 0;

//
// DLL load/unload entry point
//
BOOL APIENTRY DllMain(HANDLE hModule,
    DWORD  dwReason,
    LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        hInstance = (HINSTANCE)hModule;
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void CurrentTime(char* timeString)
{
    time_t rawtime;
    struct tm timeinfo;

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    strftime(timeString, 64, "%H:%M:%S", &timeinfo);
}

// our global hook
LRESULT CALLBACK GlobalWndRetProc(int code, WPARAM wParam, LPARAM lParam)
{
    char timeString[64];
    CurrentTime(timeString);
    printf("[%s] code: %d wparam: %d lparam: %d \n", timeString, code, (int)wParam, (int)lParam);
    return CallNextHookEx(globalHook, code, wParam, lParam);
}

extern "C"
{
    __declspec(dllexport) void InstallCoreHook(void)
    {
        printf("Installing the core hook\n");
        globalHook = SetWindowsHookEx(WH_CALLWNDPROC, GlobalWndRetProc, hInstance, 0);
    }

    __declspec(dllexport) void DisposeCoreHook(void)
    {
        printf("Disposing the core hook");
        UnhookWindowsHookEx(globalHook);
        globalHook = 0;
    }
}
