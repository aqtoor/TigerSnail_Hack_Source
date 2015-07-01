#include "inputmgr.h"
#include "vars.h"
#include "viscomps.h"
#include "d3d.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.3
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	inputmgr.h: Input manager implementations
*/

//======================================================================
CInputMgr g_InputMgr; //Singleton instance of input manager class
//======================================================================

//======================================================================
CInputMgr* CInputMgr::Instance(void)
{
	//Singleton

	return &g_InputMgr;
}
//======================================================================

//======================================================================
bool CInputMgr::Initialize(const float fSensitivity, const std::string& szWindowName)
{
	//Initialize component

	if (this->m_bReady)
		return false;

	//Get handle to window
	this->m_hWindow = FindWindowA(NULL, szWindowName.c_str());
	if (!this->m_hWindow)
		return false;

	//Get pointer to window procedure
	this->m_wpOrigProc = (WNDPROC)GetWindowLongPtr(this->m_hWindow, GWL_WNDPROC);
	if (!this->m_wpOrigProc)
		return false;

	//Replace window procedure address
	if (!SetWindowLongPtr(this->m_hWindow, GWL_WNDPROC, (LONG)&IPMGR_WindowProc))
		return false;

	//Save/init data
	this->m_fSensitivity = fSensitivity;
	this->m_ptLastMousePos.x = g_ScreenSize.x / 2;
	this->m_ptLastMousePos.y = g_ScreenSize.y / 2;

	return this->m_bReady = true;
}
//======================================================================

//======================================================================
void CInputMgr::Free(bool bDoUnhook)
{
	//Free resources

	//Unhook game window procedure if requested
	if (bDoUnhook)
		SetWindowLongPtr(this->m_hWindow, GWL_WNDPROC, (LONG)this->m_wpOrigProc);

	//Reset data
	this->m_hWindow = NULL;
	this->m_wpOrigProc = NULL;
	this->m_bReady = false;
}
//======================================================================

//======================================================================
void CInputMgr::SetBlockMode(const bool bMode, const bool bHUDToggle)
{
	//Update blocking mode

	this->m_bDoBlock = bMode;

	if (bHUDToggle) {
		switch (this->m_bDoBlock) {
		case true:
			g_pEngineClient->ClientCmd_Unrestricted("cl_drawhud 0");
			break;
		case false:
			g_pEngineClient->ClientCmd_Unrestricted("cl_drawhud 1");
			break;
		}
	}
}
//======================================================================

//======================================================================
bool CInputMgr::Event_SetViewAngles(QAngle& vViewAngles)
{
	//Event method for IVEngineClient::SetViewAngles

	if (this->m_bDoBlock) { //If mouse input to game shall be blocked
		//Get current viewangles on blocking startup and also restore cursor position
		if (!this->m_bGetViewAnglesOnce) {
			g_pEngineClient->GetViewAngles(this->m_vStaticSessionViewangles);
			this->m_ptCurrentMousePos = this->m_ptLastMousePos;
			this->m_bGetViewAnglesOnce = true;
		}
		
		//Get current viewangles
		QAngle vCurAngles;
		g_pEngineClient->GetViewAngles(vCurAngles);

		//Extract mouse direction movement

		float x = (vViewAngles[1] - vCurAngles[1]) * this->m_fSensitivity;
		float y = (vViewAngles[0] - vCurAngles[0]) * this->m_fSensitivity;
		
		this->m_ptCurrentMousePos.x += (int)x * -1;
		this->m_ptCurrentMousePos.y += (int)y;

		//Correct if out of screen
			
		if (this->m_ptCurrentMousePos.x < 0)
			this->m_ptCurrentMousePos.x = 0;

		if (this->m_ptCurrentMousePos.y < 0)
			this->m_ptCurrentMousePos.y = 0;

		if (this->m_ptCurrentMousePos.x > g_ScreenSize.x)
			this->m_ptCurrentMousePos.x = g_ScreenSize.x;

		if (this->m_ptCurrentMousePos.y > g_ScreenSize.y)
			this->m_ptCurrentMousePos.y = g_ScreenSize.y;

		//Inform form about event
		g_pMainForm->MouseEvent(this->m_ptCurrentMousePos.x, this->m_ptCurrentMousePos.y, -1, false);

		return false; //Indicate to block setting viewangles
	} else {
		//Reset flag after blocking and also save last cursor pos
		if (this->m_bGetViewAnglesOnce) {
			this->m_bGetViewAnglesOnce = false;
			this->m_ptLastMousePos = this->m_ptCurrentMousePos;
		}
	}

	//Indicate to call original function
	return true;
}
//======================================================================

//======================================================================
LRESULT CALLBACK IPMGR_WindowProc(HWND hWnd, UINT nCode, WPARAM wParam, LPARAM lParam)
{
	//Game window procedure hook implementation

	if (g_InputMgr.m_bDoBlock) { //If mouse input to game shall be blocked
		if (nCode == WM_MOUSEMOVE) { //If mouse movement window event occured
			//Prevent original window procedure from handling the event
			return 1L;
		} else if (nCode == WM_LBUTTONDBLCLK) { //Left mouse button event shall also get blocked if they occur
			//Todo: Inform of mouse double click
			
			//Prevent original window procedure from handling the event
			return 1L;
		} else if (nCode == WM_LBUTTONDOWN) { //Left mouse button event shall also get blocked if they occur
			//Inform form about event
			g_pMainForm->MouseEvent(g_InputMgr.m_ptCurrentMousePos.x, g_InputMgr.m_ptCurrentMousePos.y, VK_LBUTTON, true);

			//Prevent original window procedure from handling the event
			return 1L;
		} else if (nCode == WM_LBUTTONUP) { //Left mouse button event shall also get blocked if they occur
			//Inform form about event
			g_pMainForm->MouseEvent(g_InputMgr.m_ptCurrentMousePos.x, g_InputMgr.m_ptCurrentMousePos.y, VK_LBUTTON, false);

			//Prevent original window procedure from handling the event
			return 1L;
		}
	}

	//Let original window procedure handle the event
	return g_InputMgr.m_wpOrigProc(hWnd, nCode, wParam, lParam);
}
//======================================================================
