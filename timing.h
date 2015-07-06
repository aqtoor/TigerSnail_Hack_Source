#ifndef _TIMING_H
#define _TIMING_H

#include "includes.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	timing.h: Timing related stuff interface
*/

//======================================================================
class CPlayTime {
	struct playtime_s {
		DWORD dwTmrCur, dwTmrLast;
		DWORD dwCurSecs;
		DWORD dwCurMins;
		DWORD dwCurHours;
	};
private:
	playtime_s m_sPlayTime;
public:
	CPlayTime() {}
	~CPlayTime() {}

	void Reset(void);
	void Think(void);

	DWORD GetSeconds(void) { return this->m_sPlayTime.dwCurSecs; }
	DWORD GetMinutes(void) { return this->m_sPlayTime.dwCurMins; }
	DWORD GetHours(void) { return this->m_sPlayTime.dwCurHours; }
};
//======================================================================

#endif
