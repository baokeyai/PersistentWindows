using System.Runtime.InteropServices;

namespace Ninjacrab.PersistentWindows.Common.WinApiBridge
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    public struct MonitorInfo
    {
        // size of a device name string
        private const int CCHDEVICENAME = 32;

        public int StructureSize;
        public Rect Monitor;
        public Rect WorkArea;
        public uint Flags;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = CCHDEVICENAME)]
        public string DeviceName;
    }
}
