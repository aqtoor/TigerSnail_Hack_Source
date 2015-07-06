#ifndef _HOOKS_H
#define _HOOKS_H

#include "gamespec.h"
#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
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
#define CEL_PROCEED_EVENT_HANDLING 0x2A

typedef void (*TpfnGEL_Eventfunction)(_IGameEvent* pEvent);

class CGameEventListener : public _IGameEventListener2 {
private:
	std::string m_szEventName;
	TpfnGEL_Eventfunction m_pfnEventFunc;
public:
	CGameEventListener() : m_szEventName("") {}
	CGameEventListener(const std::string& szGameEventName, const TpfnGEL_Eventfunction pfnEventFunc) : m_szEventName(szGameEventName), m_pfnEventFunc(pfnEventFunc)
	{
		//Register game event listener

		if (!this->RegListener())
			ExitError("CGameEventListener::RegListener() failed");
	}

	virtual void HandleGameEvent(_IGameEvent *pEvent)
	{
		//Handle game event specific data here

		if (pEvent) {
			//Call event function
			this->m_pfnEventFunc(pEvent);
		}
	}

	virtual int IndicateEventHandling(void)
	{
		//Indicate whether 'HandleGameEvent()' shall get called or not
		
		return CEL_PROCEED_EVENT_HANDLING;
	}

	bool RegListener(void)
	{
		//Register game event listener

		if (!g_pGameEventManager)
			return false;

		if (!this->m_pfnEventFunc)
			return false;

		if (!this->m_szEventName.length())
			return false;

		return g_pGameEventManager->AddListener(this, this->m_szEventName.c_str(), false);
	}
};
//======================================================================

//======================================================================
bool HookCHLClient(void);
bool HookIPanel(void);
bool HookIStudioRender(void);
bool HookIVEngineClient(void);
//======================================================================

#endif
