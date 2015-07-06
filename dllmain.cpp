#include "includes.h"
#include "vars.h"
#include "log.h"
#include "memory.h"
#include "gamespec.h"
#include "hooks.h"
#include "drawing.h"
#include "inputmgr.h"
#include "utils.h"
#include "d3d.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	dllmain.cpp: DLL interface, Search Thread and error handler
*/

//======================================================================
void OnShutdown(void)
{
	//Do stuff when job is done

	//Write information message
	g_pLog->Msg("Shutting down");
	
	//Delete log object
	delete g_pLog;

	//Delete form object if exists
	if (g_pMainForm) {
		g_pMainForm->Free();
		delete g_pMainForm;
	}
}
//======================================================================

//======================================================================
void ExitError(const char* szMessage, ...)
{
	//Show an error message and exit process

	if (!szMessage)
		return;

	char szFmtBuffer[1024];

	va_list list;
	va_start(list, szMessage);
	vsprintf_s(szFmtBuffer, szMessage, list);
	va_end(list);

	MessageBoxA(0, szFmtBuffer, PROGRAM_NAME, MB_ICONERROR);

	OnShutdown();

	ExitProcess(EXIT_FAILURE);
}
//======================================================================

//======================================================================
bool InitImportableObjects(CreateInterfaceFn pfnappSystemFactory)
{
	//Initialize importable object pointers

	if (!pfnappSystemFactory)
		return false;

	for (BYTE i = 0; i < IMPORT_IFACE_AMOUNT; i++) { //Loop trough entries
		g_ClientImports[i].pInterfaceObject = pfnappSystemFactory(g_ClientImports[i].szIfName, NULL); //Get interface object pointer
		if (!g_ClientImports[i].pInterfaceObject)
			return false;
		else
			g_ClientImports[i].pdwVFTPointer = *(PDWORD*)g_ClientImports[i].pInterfaceObject; //Get VFT address
	}

	return true;
}
//======================================================================

//======================================================================
void LogImportedInterfaces(void)
{
	//Just log all imported interfaces

	for (BYTE i = 0; i < IMPORT_IFACE_AMOUNT; i++) { //Loop trough entries
		if (g_ClientImports[i].pInterfaceObject)
			g_pLog->MsgFmt("%s object instance: %p (VFT: %p)", g_ClientImports[i].szIfName, g_ClientImports[i].pInterfaceObject, g_ClientImports[i].pdwVFTPointer);
	}
}
//======================================================================

//======================================================================
CreateInterfaceFn GetFactory(LPCSTR lpszModuleName)
{
	//Get interface factory of a module

	if (!lpszModuleName)
		return NULL;

	HMODULE hModule = GetModuleHandleA(lpszModuleName);
	if (!hModule)
		return NULL;
	
	return (CreateInterfaceFn)GetProcAddress(hModule, CREATEINTERFACE_PROCNAME);
}
//======================================================================

//======================================================================
DWORD WINAPI SearchThread(LPVOID lpvArguments)
{
	//Thread to search for both engine imports to client dll and client dll exports (here one example to CHLClient)

	//Wait for client.dll to be loaded
	HMODULE hClientDll = NULL;
	while (!hClientDll) {
		hClientDll = GetModuleHandleA("client.dll");

		Sleep(10);
	}
	
	g_pLog->MsgFmt("Client.dll module base: %p", hClientDll);

	//Get segment information of client.dll
	header_info_s hdrClient;
	if (!GetModuleSegmentInfo("client.dll", &hdrClient)) {
		ExitError("Failed to get header information of client.dll");
	}

	//Get segment information of engine.dll
	header_info_s hdrEngine;
	if (!GetModuleSegmentInfo("engine.dll", &hdrEngine)) {
		ExitError("Failed to get header information of engine.dll");
	}
	
	g_pLog->MsgFmt("Client.dll code segment: %p (%d bytes in size)", hdrClient.dwCodeAddr, hdrClient.dwCodeSize);
	g_pLog->MsgFmt("Client.dll data segment: %p (%d bytes in size)", hdrClient.dwDataAddr, hdrClient.dwCodeSize);
	g_pLog->MsgFmt("Engine.dll code segment: %p (%d bytes in size)", hdrEngine.dwCodeAddr, hdrEngine.dwCodeSize);
	g_pLog->MsgFmt("Engine.dll data segment: %p (%d bytes in size)", hdrEngine.dwDataAddr, hdrEngine.dwCodeSize);

	//Wait a bit to let the game initialize
	Sleep(5000);

	//Wait until game window exists and save handle to it
	while (!g_hGameWindow) {
		g_hGameWindow = FindWindow(NULL, TEXT(GAME_TARGET_WINDOW));
	}
	
	//Find appSystemFactory function
	g_appSystemFactory = FindAppSystemFactory(&hdrEngine);
	if (!g_appSystemFactory)
		ExitError("Failed to find appSystemFactory() function");
	
	//Initialize client imports
	if (!InitImportableObjects(g_appSystemFactory))
		ExitError("Failed to initialize importable interface objects");

	//Save IVEngineClient instance pointer
	g_pEngineClient = (_IVEngineClient*)g_ClientImports[ID_VEngineClient].pInterfaceObject;

	//Save IVDebugOverlay instance pointer
	g_pDebugOverlay = (_IVDebugOverlay*)g_ClientImports[ID_VDebugOverlay].pInterfaceObject;

	//Save IGameEventManager instance pointer
	g_pGameEventManager = (_IGameEventManager2*)g_ClientImports[ID_GameEventManager].pInterfaceObject;
	
	//Initialize drawing
	if (!SetupDrawingInterface(&g_ClientImports[ID_VGUI_Surface], "Verdana", 15))
		ExitError("Failed to initialize drawing interface");
	
	g_pLog->Msg("Drawing initialized");

	//Get pointer to VClient016 interface function. The function must be called to get the start address to the class. (See EXPOSE_SINGLE_INTERFACE_GLOBALVAR macro for details)
	TGetExportObject pfnCHLClient = (TGetExportObject)FindClientExport(hClientDll, CHLCLIENT);
	if (!pfnCHLClient)
		ExitError("Failed to get " CHLCLIENT " interface function pointer");
	
	//Call function to get CHLClient pointer
	g_CHLClient_Ctx.pCHLClient = (class CHLClient*)pfnCHLClient();
	if (!g_CHLClient_Ctx.pCHLClient)
		ExitError("pfnCHLClient() failed");

	//We do this also for the VClientEntityList
	TGetExportObject pfnClientEntityList = (TGetExportObject)FindClientExport(hClientDll, ICLIENTENTITYLIST);
	if (!pfnClientEntityList)
		ExitError("Failed to get " ICLIENTENTITYLIST " interface function pointer");
	
	//Call function to get IClientEntityList pointer
	g_pClientEntityList = (_CClientEntityList*)pfnClientEntityList();
	if (!g_pClientEntityList)
		ExitError("pfnIClientEntityList() failed");
	
	//Get pointer to the CHLClient VFT, which only exists because there are virtual functions and it's the first DWORD at the class instance
	g_CHLClient_Ctx.pdwCHLClientVFTable = (DWORD*)(*(DWORD*)g_CHLClient_Ctx.pCHLClient);
	//Backup VFT
	memcpy(g_CHLClient_Ctx.dwVFTBackup, g_CHLClient_Ctx.pdwCHLClientVFTable, CHLCLIENT_VFT_SIZE * sizeof(DWORD));
	
	//Find ICvar class
	g_ICvar_Ctx.pICvar = FindICvarClass(&hdrClient);
	if (!g_ICvar_Ctx.pICvar)
		ExitError("Failed to find ICvar class object");

	//Get pointer to the ICvar VFT, which only exists because there are virtual functions and it's the first DWORD at the class instance
	g_ICvar_Ctx.pdwICvarVFTable = *(PDWORD*)(g_ICvar_Ctx.pICvar);
	//Backup VFT
	memcpy(&g_ICvar_Ctx.dwVFTBackup, g_ICvar_Ctx.pdwICvarVFTable, ICVAR_VFT_SIZE * sizeof(DWORD));

	//Get VGUI2 factory
	CreateInterfaceFn pfnVGUI2 = GetFactory("vgui2.dll");
	if (!pfnVGUI2)
		ExitError("Failed to get factory of vgui2.dll");

	//Get interface function for VGUI_Panel009
	g_IPanel_Ctx.pIPanel = (vgui::IPanel*)pfnVGUI2(VGUI_PANEL_INTERFACE_VERSION, NULL);
	if (!g_IPanel_Ctx.pIPanel)
		ExitError("Failed to get IPanel object");
	
	//Access VFT and backup it
	g_IPanel_Ctx.pdwIPanelVFTable = *(PDWORD*)g_IPanel_Ctx.pIPanel;
	memcpy(g_IPanel_Ctx.dwVFTBackup, g_IPanel_Ctx.pdwIPanelVFTable, IPANEL_VFT_SIZE);

	//Log all found objects into the log-file
	g_pLog->MsgFmt("appSystemFactory function: %p", g_appSystemFactory);
	g_pLog->MsgFmt("CHLClient class instance: %p", g_CHLClient_Ctx.pCHLClient);
	g_pLog->MsgFmt("IClientEntityList class instance: %p", g_pClientEntityList);
	g_pLog->MsgFmt("ICvar class instance: %p", g_ICvar_Ctx.pICvar);
	g_pLog->MsgFmt("IPanel class instance: %p", g_IPanel_Ctx.pIPanel);
	LogImportedInterfaces();

	//Add hook classes
	#define ADD_HOOK_CLASS(n, ptr, size) if (!g_oHookMgr.Add(n, ptr, size)) ExitError("CVFTHookMgr::Add() failed for %s", n);
	ADD_HOOK_CLASS("CHLClient", (VFTPOINTER)g_CHLClient_Ctx.pCHLClient, CHLCLIENT_VFT_SIZE);
	ADD_HOOK_CLASS("IVEngineClient", (VFTPOINTER)g_ClientImports[ID_VEngineClient].pInterfaceObject, IVENGINECLIENT_VFT_SIZE);
	ADD_HOOK_CLASS("IPanel", (VFTPOINTER)g_IPanel_Ctx.pIPanel, IPANEL_VFT_SIZE);
	ADD_HOOK_CLASS("IStudioRender", (VFTPOINTER)g_ClientImports[ID_VStudioRender].pInterfaceObject, ISTUDIORENDER_VFT_SIZE);
	
	//Hook EndScene()
	if (!HookEndScene()) {
		ExitError("HookEndScene() failed");
	} else {
		g_pLog->Msg("Hooked IDirect3DDevice9::EndScene()");
	}
	
	//Hook CHLClient methods
	if (!HookCHLClient()) {
		ExitError("HookCHLClient() failed");
	} else {
		g_pLog->Msg("Hooked CHLClient method(s)");
	}

	//Hook IPanel methods
	if (!HookIPanel()) {
		ExitError("HookIPanel() failed");
	} else {
		g_pLog->Msg("Hooked IPanel method(s)");
	}
	
	//Hook IStudioRender methods
	if (!HookIStudioRender()) {
		ExitError("HookIStudioRender() failed");
	} else {
		g_pLog->Msg("Hooked IStudioRender method(s)");
	}

	//Hook IVEngineClient methods
	if (!HookIVEngineClient()) {
		ExitError("HookIVEngineClient() failed");
	} else {
		g_pLog->Msg("Hooked IVEngineClient method(s)");
	}
	
	return TRUE;
}
//======================================================================

//======================================================================
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason) {
	case DLL_PROCESS_ATTACH: { //DLL is attached to process
		//Get module directory

		char szModuleDir[MAX_PATH];
		DWORD dwCopiedLength = GetModuleFileNameA(hInstance, szModuleDir, sizeof(szModuleDir));
		if (!dwCopiedLength) {
			ExitError("DllMain: GetModuleFileNameA failed (%d)", GetLastError());
		}

		for (DWORD i = dwCopiedLength; i > 3; i--) {
			if (szModuleDir[i] == '\\')
				break;

			szModuleDir[i] = 0;
		}

		g_szModuleDir = szModuleDir;

		//Create log
		g_szLogFile = g_szModuleDir + PROGRAM_SHORTCUT "_Session.log";
		g_pLog = new CLog(PROGRAM_SHORTCUT, g_szLogFile);
		if ((!g_pLog) || (!g_pLog->Opened())) {
			ExitError("DllMain: Could not create log file: %s", g_szLogFile);
		}

		g_pLog->Msg(PROGRAM_NAME " v" PROGRAM_VERSION " developed by " PROGRAM_AUTHOR " (" PROGRAM_CONTACT ")");
		g_pLog->Msg("DllMain: DLL attached to process");

		//Disable thread related calls of DllMain 
		if (!DisableThreadLibraryCalls(hInstance))
			g_pLog->Warning("Failed to disable thread library calls");

		g_hSearchThread = CreateThread(NULL, 0, &SearchThread, NULL, 0, NULL);
		if (g_hSearchThread == NULL) {
			ExitError("DllMain: Failed to create hook thread");
		}

		break;
	}

	case DLL_PROCESS_DETACH: { //DLL shall get detached from process
		if (g_hSearchThread)
			CloseHandle(g_hSearchThread);

		OnShutdown();

		break;
	}

	case DLL_THREAD_ATTACH: { //DLL is informed about thread attach
		break;
	}

	case DLL_THREAD_DETACH: { //DLL is informed about thread detach
		break;
	}
	default:
		break;
	}

	return TRUE;
}
//======================================================================
