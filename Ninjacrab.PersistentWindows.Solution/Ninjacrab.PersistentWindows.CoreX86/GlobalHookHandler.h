#pragma once
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

// Totally butchering header files and OOP, I know, but right now is about getting stuff done.
HMODULE globalHookLibraryHandle;

void(*InstallCoreHook)(void);
void(*DisposeCoreHook)(void);

int LoadHookLibrary()
{
	HMODULE globalHookLibraryHandle = LoadLibrary("Ninjacrab.PersistentWindows.CoreX86.dll");
	if (globalHookLibraryHandle == NULL) {
		printf("The DLL could not be found.\n");
		return -1;
	}
	printf("Ninjacrab.PersistentWindows.CoreX86.dll successfully loaded\n");
	InstallCoreHook = (void(*)(void))GetProcAddress(globalHookLibraryHandle, "InstallCoreHook");
	DisposeCoreHook = (void(*)(void))GetProcAddress(globalHookLibraryHandle, "DisposeCoreHook");
	InstallCoreHook();
	return 0;
}

void CleanGlobalHook()
{
	if (DisposeCoreHook != NULL)
	{
		DisposeCoreHook();
		DisposeCoreHook = NULL;
	}

	InstallCoreHook = NULL;
	
	FreeLibrary(globalHookLibraryHandle);
}