#ifndef _HOOKS_H
#define _HOOKS_H

#include "gamespec.h"
#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.1
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: hooks.h: Hook Interface
*/

//======================================================================
#define HOOK_METHOD(n, id, addr) if (!g_oHookMgr.Hook(n, id, (VFTPOINTER)addr)) return false;

#define SETUP_STATIC_OBJECTS_CHLCLIENT(fn) static DWORD dwOrigFunction; \
	static DWORD dwClassBase; \
	dwOrigFunction = g_oHookMgr.GetOrigAddr("CHLClient", fn); \
	dwClassBase = (DWORD)g_oHookMgr.GetClass("CHLClient"); \

#define SETUP_STATIC_OBJECTS_IPANEL(fn) static DWORD dwOrigFunction; \
	static DWORD dwClassBase; \
	dwOrigFunction = g_oHookMgr.GetOrigAddr("IPanel", fn); \
	dwClassBase = (DWORD)g_oHookMgr.GetClass("IPanel"); \

#define SETUP_STATIC_OBJECTS_ISTUDIORENDER(fn) static DWORD dwOrigFunction; \
	static DWORD dwClassBase; \
	dwOrigFunction = g_oHookMgr.GetOrigAddr("IStudioRender", fn); \
	dwClassBase = (DWORD)g_oHookMgr.GetClass("IStudioRender"); \

#define SETUP_STATIC_OBJECTS_IVENGINECLIENT(fn) static DWORD dwOrigFunction; \
	static DWORD dwClassBase; \
	dwOrigFunction = g_oHookMgr.GetOrigAddr("IVEngineClient", fn); \
	dwClassBase = (DWORD)g_oHookMgr.GetClass("IVEngineClient");
//======================================================================

//======================================================================
bool HookCHLClient(void);
bool HookIPanel(void);
bool HookIStudioRender(void);
bool HookIVEngineClient(void);
//======================================================================

#endif
