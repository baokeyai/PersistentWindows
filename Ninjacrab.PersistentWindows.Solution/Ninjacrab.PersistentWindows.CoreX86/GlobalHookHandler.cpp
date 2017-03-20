#pragma region include and define
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <string>
#include <vector>

using namespace System;
using namespace Ninjacrab::Logging;

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

// our global hook
LRESULT CALLBACK GlobalWndRetProc(int code, WPARAM wParam, LPARAM lParam)
{
    Logger::Info(String::Format("Code: {0} Wparam: {1} Lparam: {2}", code, wParam, lParam));
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
