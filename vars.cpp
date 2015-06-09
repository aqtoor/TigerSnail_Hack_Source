#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.1
	Visit: http://sk0r.sytes.net

	File: vars.cpp: Data objects 
*/

//======================================================================
std::string g_szModuleDir(""); //Path to module file
std::string g_szLogFile(""); //Full file name of log output file

HANDLE g_hSearchThread; //Handle to the search thread

CreateInterfaceFn g_appSystemFactory; //Pointer to appSystemFactory function from engine.dll

client_import_s g_ClientImports[IMPORT_IFACE_AMOUNT] = { //List of some importable interfaces
	{"VEngineClient013", NULL},
	{"VStudioRender026", NULL},
	{"VDebugOverlay004", NULL},
	{"VGUI_Surface031", NULL}
	//Add more here
};

CHLClient_Context_s g_CHLClient_Ctx = {0}; //CHLClient context
IPanel_Context_s g_IPanel_Ctx; //IPanel context
ICvar_Context_s g_ICvar_Ctx; //ICvar context
_IVEngineClient* g_pEngineClient = NULL; //Pointer to an instance of IVEngineClient
_CClientEntityList* g_pClientEntityList = NULL; //Pointer to an instance of CClientEntityList
_IVDebugOverlay* g_pDebugOverlay = NULL; //Pointer to an instance of IVDebugOverlay

HWND g_hGameWindow = NULL; //Handle to game window
screensize_s g_ScreenSize; //Screen resolution

bool g_bMapInit = false; //Whether map has been initialized
bool g_bOnInitialize = false; //Whether OnInitialize() has already been called
bool g_bMenuToggle = false; //Menu toggle indicator

CLog* g_pLog = NULL; //Log manager instance
CVFTHookMgr g_oHookMgr; //Hook manager instance
CDelayedClientCmd g_oDelayedClientCmd; //Delayed client cmd manager instance
CzyConfigMgr::CConfigInt g_oConfigInt; //Configuration manager instance

CzyConfigMgr::CCVar::cvar_s* g_pcvNameESP = NULL; //CVar for name esp
CzyConfigMgr::CCVar::cvar_s* g_pcvSteamIDESP = NULL; //CVar for SteamID esp
CzyConfigMgr::CCVar::cvar_s* g_pcvMenuKey = NULL; //CVar for menu key

std::string g_szCurrentMapName(""); //Current map name
//======================================================================
