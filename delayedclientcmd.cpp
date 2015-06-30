#include "delayedclientcmd.h"
#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: delayedclientcmd.cpp: Delayed client cmd implementations
*/

//======================================================================
void CDelayedClientCmd::Clear(void)
{
	//Clear all

	this->m_vItems.clear();
}
//======================================================================

//======================================================================
bool CDelayedClientCmd::AddClientCmd(const std::string& szCommandString, const unsigned int uiDelay)
{
	//Add new item

	if ((!szCommandString.length()) || (!uiDelay))
		return false;

	//Setup data
	dcc_data_s sData;
	sData.szExpression = szCommandString;
	sData.dwDelay = uiDelay;
	sData.dwTmrInit = sData.dwTmrCur = GetTickCount();

	//Add to list
	this->m_vItems.push_back(sData);

	return true;
}
//======================================================================

//======================================================================
void CDelayedClientCmd::Process(void)
{
	//Process all existing items

	for (size_t i = 0; i < this->m_vItems.size(); i++) { //Loop through items
		this->m_vItems[i].dwTmrCur = GetTickCount(); //Update timer of current item
		if (this->m_vItems[i].dwTmrCur > this->m_vItems[i].dwTmrInit + this->m_vItems[i].dwDelay) { //If delay time has elapsed
			//Execute command expression
			g_pEngineClient->ClientCmd_Unrestricted(this->m_vItems[i].szExpression.c_str());

			//Remove from list
			this->m_vItems.erase(this->m_vItems.begin() + i);
		}
	}
}
//======================================================================
