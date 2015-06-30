#ifndef _INPUTMGR_H
#define _INPUTMGR_H

#include "includes.h"
#include "gamespec.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	inputmgr.h: Input manager interface
*/

//======================================================================
class CInputMgr {
private:
	bool m_bReady;
	HWND m_hWindow;
	WNDPROC m_wpOrigProc;
	POINT m_ptCurrentMousePos;
	POINT m_ptLastMousePos;
	bool m_bDoBlock;
	bool m_bGetViewAnglesOnce;
	QAngle m_vStaticSessionViewangles;
	float m_fSensitivity;

	friend LRESULT CALLBACK IPMGR_WindowProc(HWND hWnd, UINT nCode, WPARAM wParam, LPARAM lParam);
public:
	CInputMgr() : m_bReady(false), m_hWindow(NULL), m_wpOrigProc(NULL), m_bGetViewAnglesOnce(false), m_fSensitivity(1.0f) {}
	~CInputMgr() { this->Free(false); }

	bool Initialize(const float fSensitivity, const std::string& szWindowName);
	void Free(bool bDoUnhook);

	bool Event_SetViewAngles(QAngle& vViewAngles);

	void SetBlockMode(const bool bMode, const bool bHUDToggle);
	bool IsBlocking(void) { return this->m_bDoBlock; }

	int GetMouseX(void) { return this->m_ptCurrentMousePos.x; }
	int GetMouseY(void) { return this->m_ptCurrentMousePos.y; }

	static CInputMgr* Instance(void);
};
//======================================================================

//======================================================================
extern CInputMgr g_InputMgr;
//======================================================================

#endif
