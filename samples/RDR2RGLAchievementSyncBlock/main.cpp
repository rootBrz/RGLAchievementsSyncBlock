/*
	THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2019
*/

#include "..\..\inc\main.h"
#include "script.h"

static void PatchSync()
{
	uintptr_t gameBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
	void* pJnz = reinterpret_cast<void*>(gameBase + 0x2340AE1);

	DWORD oldProt;
	VirtualProtect(pJnz, 1, PAGE_EXECUTE_READWRITE, &oldProt);
	*reinterpret_cast<unsigned char*>(pJnz) = 0xEB;
	VirtualProtect(pJnz, 1, oldProt, &oldProt);
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		PatchSync();
		scriptRegister(hInstance, ScriptMain);
		break;
	case DLL_PROCESS_DETACH:
		scriptUnregister(hInstance);
		break;
	}		
	return TRUE;
}