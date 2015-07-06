#ifndef _DELAYED_CLIENTCMD_H
#define _DELAYED_CLIENTCMD_H

#include "includes.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: delayedclientcmd.h: Delayed client cmd interface
*/

//======================================================================
struct dcc_data_s {
	std::string szExpression;
	unsigned int dwDelay;
	unsigned int dwTmrInit, dwTmrCur;
};
//======================================================================

//======================================================================
class CDelayedClientCmd {
private:
	std::vector<dcc_data_s> m_vItems;

	void Clear(void);
public:
	CDelayedClientCmd() {}
	~CDelayedClientCmd() { this->Clear(); }

	bool AddClientCmd(const std::string& szCommandString, const unsigned int uiDelay);

	void Process(void);
};
//======================================================================

#endif
