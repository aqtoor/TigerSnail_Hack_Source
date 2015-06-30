#include "gamespec.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	gamespec.cpp: Game specific implementations
*/

//======================================================================
client_interface_export* FindExportInterfaceRegs(HMODULE hClientDll)
{
	//Get start address to the value of InterfaceReg::s_pInterfaceRegs.
	//It points to the first entry in the linked list of client.dll exports

	if (!hClientDll)
		return NULL;

	FARPROC pfnCreateInterface = GetProcAddress(hClientDll, "CreateInterface");
	if (!pfnCreateInterface)
		return NULL;

	DWORD dwRelJumpAddr = *(DWORD*)((DWORD)pfnCreateInterface + 0x05); //Get to address of jump instruction

	DWORD dwDestAddr = ((DWORD)pfnCreateInterface + 0x04) + dwRelJumpAddr + 0x05; //Calculate absolute destination address

	DWORD dwStartAddrPointer = *(DWORD*)(dwDestAddr + 0x06); //Get pointer to InterfaceReg::s_pInterfaceRegs

	return *(client_interface_export**)dwStartAddrPointer; //Copy the address value of InterfaceReg::s_pInterfaceRegs
}
//======================================================================

//======================================================================
LPVOID FindClientExport(HMODULE hClientDll, LPCSTR lpszExportName)
{
	//Walk trough the linked list of client.dll exports and find the desired one's function pointer.
	//The function the pointer points to can be called to get the pointer of a client.dll class which
	//is exported to the engine to call the functions (as I think)

	if (!lpszExportName)
		return NULL;

	static client_interface_export* pClientExport = NULL; //Pointer to the first entry in list

	if (!pClientExport) {
		if (!hClientDll)
			return NULL;

		//Get pointer to the first entry in the linked list
		pClientExport = FindExportInterfaceRegs(hClientDll);
		if (!pClientExport)
			return NULL;
	} 

	if (pClientExport) {
		client_interface_export* pCurrentEntry = pClientExport; //Get first entry in linked list

		while (pCurrentEntry) { //While the end is not reached
			if (strcmp(pCurrentEntry->pszExportName, lpszExportName)==0) //Return the function pointer if entry has been found
				return pCurrentEntry->lpvFunction;

			pCurrentEntry = pCurrentEntry->pNextExport; //Goto next entry
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
CreateInterfaceFn FindAppSystemFactory(header_info_s* pEngineDllHeader)
{
	//Get the function address of appSystemFactory which can be used to get many usefull interfaces from engine

	if (!pEngineDllHeader)
		return NULL;

	//Find string in data segment
	DWORD dwStringPush = FindString("Client.dll Init() in library client failed.", pEngineDllHeader->dwDataAddr, pEngineDllHeader->dwDataAddr + pEngineDllHeader->dwDataSize, NULL);
	if (!dwStringPush)
		return NULL;

	//Setup opcode with argument
	unsigned char ucPushOp[] = {0x68, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)&ucPushOp[1] = dwStringPush;
	
	//Find CPU command in code segment
	DWORD dwPushOp = FindSignature(ucPushOp, sizeof(ucPushOp), pEngineDllHeader->dwCodeAddr, pEngineDllHeader->dwCodeAddr + pEngineDllHeader->dwCodeSize, NULL);
	if (!dwPushOp)
		return NULL;

	//Get pointer value
	DWORD dwPointer = *(DWORD*)(dwPushOp - 0xB);

	return *(CreateInterfaceFn*)dwPointer; //Get function start address
}
//======================================================================

//======================================================================
ICvar* FindICvarClass(header_info_s* pClientDllHeader)
{
	//Find pointer to the ICvar interface via the client.dll CHLClient Init function

	if (!pClientDllHeader)
		return NULL;

	//Find string in data segment
	DWORD dwStringPush = FindString("host_thread_mode", pClientDllHeader->dwDataAddr, pClientDllHeader->dwDataAddr + pClientDllHeader->dwDataSize, NULL);
	if (!dwStringPush)
		return NULL;

	//Setup opcode with argument
	unsigned char ucPushOp[] = {0x68, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)&ucPushOp[1] = dwStringPush;

	//Find CPU command in code segment
	DWORD dwPushOp = FindSignature(ucPushOp, sizeof(ucPushOp), pClientDllHeader->dwCodeAddr, pClientDllHeader->dwCodeAddr + pClientDllHeader->dwCodeSize, NULL);
	if (!dwPushOp)
		return NULL;

	return *(ICvar**)(*(DWORD*)(dwPushOp - 0x04)); //Return pointer to class object
}
//======================================================================
