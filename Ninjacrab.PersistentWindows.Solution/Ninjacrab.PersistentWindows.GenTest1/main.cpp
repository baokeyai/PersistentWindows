#pragma region include and define
#include <windows.h>
#include <shellapi.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "GlobalHookHandler.h"
#include "DisplayScreen.h"

using namespace System;
#ifdef UNICODE
#define stringcopy wcscpy
#else
#define stringcopy strcpy_s
#endif

#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define WM_TRAYICON ( WM_USER + 1 )
#pragma endregion

#pragma region constants and globals
UINT WM_TASKBARCREATED = 0;

HHOOK globalHook;
HWND g_hwnd;
HMENU g_menu;
HMODULE hInstance = 0;

NOTIFYICONDATA g_notifyIconData;
#pragma endregion

#pragma region helper funcs
// These next 2 functions are the STARS of this example.
// They perform the "minimization" function and "restore"
// functions for our window.  Notice how when you "minimize"
// the app, it doesn't really "minimize" at all.  Instead,
// you simply HIDE the window, so it doesn't display, and
// at the same time, stick in a little icon in the system tray,
// so the user can still access the application.
void Minimize()
{
    // add the icon to the system tray
    Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);
}

// Basically bring back the window (SHOW IT again)
// and remove the little icon in the system tray.
void Restore()
{
    // Remove the icon from the system tray
    // Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
}

// Initialize the NOTIFYICONDATA structure.
// See MSDN docs http://msdn.microsoft.com/en-us/library/bb773352(VS.85).aspx
// for details on the NOTIFYICONDATA structure.
void InitNotifyIconData()
{
    memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));
    g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);

    /////
    // Tie the NOTIFYICONDATA struct to our
    // global HWND (that will have been initialized
    // before calling this function)
    g_notifyIconData.hWnd = g_hwnd;
    // Now GIVE the NOTIFYICON.. the thing that
    // will sit in the system tray, an ID.
    g_notifyIconData.uID = ID_TRAY_APP_ICON;
    // The COMBINATION of HWND and uID form
    // a UNIQUE identifier for EACH ITEM in the
    // system tray.  Windows knows which application
    // each icon in the system tray belongs to
    // by the HWND parameter.
    /////

    /////
    // Set up flags.
    g_notifyIconData.uFlags = NIF_ICON | // promise that the hIcon member WILL BE A VALID ICON!!
        NIF_MESSAGE | // when someone clicks on the system tray icon,
                      // we want a WM_ type message to be sent to our WNDPROC
        NIF_TIP;      // we're gonna provide a tooltip as well, son.

    g_notifyIconData.uCallbackMessage = WM_TRAYICON; //this message must be handled in hwnd's window procedure. more info below.

                                                     // Load da icon.  Be sure to include an icon "green_man.ico" .. get one
                                                     // from the internet if you don't have an icon
    g_notifyIconData.hIcon = (HICON)LoadImage(NULL, TEXT("../Resources/Oxygen-Icons.org-Oxygen-Status-media-playlist-repeat.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    // set the tooltip text.  must be LESS THAN 64 chars
    stringcopy(g_notifyIconData.szTip, TEXT("Persistent Windows 1.1"));
}
#pragma endregion

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd))
    {
        Minimize();
        return 0;
    }

    switch (message)
    {
    case WM_CREATE:

        // create the menu once.
        // oddly, you don't seem to have to explicitly attach
        // the menu to the HWND at all.  This seems so ODD.
        g_menu = CreatePopupMenu();

        AppendMenu(g_menu, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, TEXT("Exit"));

        break;

    case WM_SYSCOMMAND:
        switch (wParam & 0xfff0)  // (filter out reserved lower 4 bits:  see msdn remarks http://msdn.microsoft.com/en-us/library/ms646360(VS.85).aspx)
        {
        case SC_MINIMIZE:
        case SC_CLOSE:  // redundant to WM_CLOSE, it appears
            Minimize();
            return 0;
            break;
        }
        break;


        // Our user defined WM_TRAYICON message.
        // We made this message up, and we told
        // 
    case WM_TRAYICON:
    {
        switch (wParam)
        {
        case ID_TRAY_APP_ICON:
            //printf("Its the ID_TRAY_APP_ICON.. one app can have several tray icons, ya know..\n");
            break;
        }

        // the mouse button has been released.

        // I'd LIKE TO do this on WM_LBUTTONDOWN, it makes
        // for a more responsive-feeling app but actually
        // the guy who made the original post is right.
        // Most apps DO respond to WM_LBUTTONUP, so if you
        // restore your window on WM_LBUTTONDOWN, then some
        // other icon will scroll in under your mouse so when
        // the user releases the mouse, THAT OTHER ICON will
        // get the WM_LBUTTONUP command and that's quite annoying.
        switch (lParam)
        {
            case WM_LBUTTONUP:
                printf("You have restored me!\n");
                Restore();
                break;

            case WM_RBUTTONDOWN:
                // I'm using WM_RBUTTONDOWN here because
                // it gives the app a more responsive feel.  Some apps
                // DO use this trick as well.  Right clicks won't make
                // the icon disappear, so you don't get any annoying behavior
                // with this (try it out!)

                // Get current mouse position.
                POINT curPoint;
                GetCursorPos(&curPoint);

                // should SetForegroundWindow according
                // to original poster so the popup shows on top
                SetForegroundWindow(hwnd);

                // TrackPopupMenu blocks the app until TrackPopupMenu returns
                printf("calling track\n");
                UINT clicked = TrackPopupMenu(
                    g_menu,
                    TPM_RETURNCMD | TPM_NONOTIFY, // don't send me WM_COMMAND messages about this window, instead return the identifier of the clicked menu item
                    curPoint.x,
                    curPoint.y,
                    0,
                    hwnd,
                    NULL
                );
                printf("returned from call to track\n");

                // Original poster's line of code.  Haven't deleted it,
                // but haven't seen a need for it.
                //SendMessage(hwnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
                if (clicked == ID_TRAY_EXIT_CONTEXT_MENU_ITEM)
                {
                    // quit the application.
                    PostQuitMessage(0);
                }
                break;
        }
    }
    break;

    // intercept the hittest message.. making full body of
    // window draggable.
    case WM_NCHITTEST:
    {
        // http://www.catch22.net/tuts/tips
        // this tests if you're on the non client area hit test
        UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
        if (uHitTest == HTCLIENT)
            return HTCAPTION;
        else
            return uHitTest;
    }

    case WM_CLOSE:
        printf("Got an actual WM_CLOSE Message!  Woo hoo!\n");
        Minimize();
        return 0;
        break;

    case WM_DESTROY:
        printf("DESTROY!!\n");
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void AttachToConsole()
{
    FILE *consoleOutStream;
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen_s(&consoleOutStream, "CON", "w", stdout);
}

void CreateParentWindow()
{
    TCHAR className[] = TEXT("Ninjacrab.PersistentWindows");
    WNDCLASSEX wnd = { 0 };

    wnd.hInstance = hInstance;
    wnd.lpszClassName = className;
    wnd.lpfnWndProc = WndProc;
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.cbSize = sizeof(WNDCLASSEX);

    wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    wnd.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;

    if (!RegisterClassEx(&wnd))
    {
        FatalAppExit(0, TEXT("Couldn't register window class!"));
    }

    g_hwnd = CreateWindowEx(
        0,
        className,
        TEXT("Ninjacrab PersistentWindows"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow)
{

    // I want to be notified when windows explorer
    // crashes and re-launches the taskbar.  the WM_TASKBARCREATED
    // event will be sent to my WndProc() AUTOMATICALLY whenever
    // explorer.exe starts up and fires up the taskbar again.
    // So its great, because now, even if explorer crashes,
    // I have a way to re-add my system tray icon in case
    // the app is already in the "minimized" (hidden) state.
    // if we did not do this an explorer crashed, the application
    // would remain inaccessible!!
    WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

#if _DEBUG
    AttachToConsole();
#endif

    printf("Starting\n");
    CreateParentWindow();
    LoadHookLibrary();
    // Initialize the NOTIFYICONDATA structure once
    InitNotifyIconData();
    Minimize();

    DisplayScreen::RefreshDisplays();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Once you get the quit message, before exiting the app,
    // clean up and remove the tray icon
    if (!IsWindowVisible(g_hwnd))
    {
        Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
    }

    CleanGlobalHook();
    return msg.wParam;
}
