using System;
using System.Collections.Generic;

namespace Ninjacrab.PersistentWindows.Common.WinApiBridge
{
    public class AppWindow
    {
        public IntPtr Handle { get; set; }
        public WindowInfo WindowInfo { get; private set; }

        public static List<AppWindow> GetWindows()
        {
            List<AppWindow> windows = new List<AppWindow>();

            User32.EnumWindows(
                delegate (IntPtr hWnd, IntPtr lParam)
                {
                    WindowInfo windowInfo = new WindowInfo();
                    bool success = User32.GetWindowInfo(hWnd, ref windowInfo);
                    if (success)
                    {
                        AppWindow appWindow = new AppWindow();
                        appWindow.Handle = hWnd;
                        appWindow.WindowInfo = windowInfo;

                        windows.Add(appWindow);
                    }
                    return true;
                }, IntPtr.Zero);
            return windows;
        }
    }
}
