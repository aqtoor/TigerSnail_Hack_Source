#ifndef _VARS_H
#define _VARS_H

#include "includes.h"
#include "memory.h"
#include "gamespec.h"
#include "log.h"
#include "vfthook.h"
#include "delayedclientcmd.h"
#include "inputmgr.h"
#include "czyconfigmgr.h"


/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.1
	Visit: http://sk0r.sytes.net

	File: vars.h: Data interface 
*/

//======================================================================
#define PROGRAM_NAME "TigerSnail Hack: Source"
#define PROGRAM_SHORTCUT "TSH"
#define PROGRAM_VERSION "0.1"
#define PROGRAM_AUTHOR "sk0r / Czybik"
#define PROGRAM_CONTACT "Czybik_Stylez<0x40>gmx<0x2E>de"

#define MAX_CHAR 250

#define IMPORT_IFACE_AMOUNT 4

#define GAME_TARGET_WINDOW "Counter-Strike: Global Offensive"

#define DEFAULT_SCRIPT_FILE "settings.cfg"
//======================================================================

//======================================================================
//Indices for own table
enum {
	ID_VEngineClient,
	ID_VStudioRender,
	ID_VDebugOverlay,
	ID_VGUI_Surface
};
//======================================================================

//======================================================================
struct CHLClient_Context_s {
	class CHLClient* pCHLClient; //Pointer to class object
	DWORD* pdwCHLClientVFTable; //Pointer to the class VFT
	DWORD dwVFTBackup[CHLCLIENT_VFT_SIZE]; //Backup of the original VFT
};

struct ICvar_Context_s {
	class ICvar* pICvar; //Pointer to class object
	DWORD* pdwICvarVFTable; //Pointer to the class VFT
	DWORD dwVFTBackup[ICVAR_VFT_SIZE]; //Backup of the original VFT
};

struct IPanel_Context_s {
	class vgui::IPanel* pIPanel; //Pointer to class object
	DWORD* pdwIPanelVFTable; //Pointer to the class VFT
	DWORD dwVFTBackup[IPANEL_VFT_SIZE / sizeof(DWORD)]; //Backup of the original VFT
};

struct client_import_s {
	char szIfName[MAX_CHAR]; //Interface Name
	void* pInterfaceObject; //Pointer to interface class object
	DWORD* pdwVFTPointer; //Pointer to VFT
};

struct screensize_s {
	int x, y;
};
//======================================================================

//======================================================================
extern std::string g_szModuleDir;
extern std::string g_szLogFile;

extern HANDLE g_hSearchThread;
extern CreateInterfaceFn g_appSystemFactory;
extern client_import_s g_ClientImports[IMPORT_IFACE_AMOUNT];
extern CHLClient_Context_s g_CHLClient_Ctx;
extern IPanel_Context_s g_IPanel_Ctx;
extern ICvar_Context_s g_ICvar_Ctx;
extern _IVEngineClient* g_pEngineClient;
extern _CClientEntityList* g_pClientEntityList;
extern _IVDebugOverlay* g_pDebugOverlay;

extern HWND g_hGameWindow;
extern screensize_s g_ScreenSize;

extern bool g_bMapInit;
extern bool g_bOnInitialize;
extern bool g_bMenuToggle;

extern CLog* g_pLog;
extern CVFTHookMgr g_oHookMgr;
extern CDelayedClientCmd g_oDelayedClientCmd;
extern CzyConfigMgr::CConfigInt g_oConfigInt;

extern CzyConfigMgr::CCVar::cvar_s* g_pcvNameESP;
extern CzyConfigMgr::CCVar::cvar_s* g_pcvSteamIDESP;
extern CzyConfigMgr::CCVar::cvar_s* g_pcvMenuKey;

extern std::string g_szCurrentMapName;
//======================================================================

//======================================================================
void ExitError(const char* szMessage, ...);
//======================================================================

#endif
