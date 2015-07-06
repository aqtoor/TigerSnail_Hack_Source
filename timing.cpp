#include "timing.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	timing.h: Timing related implementations
*/

//======================================================================
void CPlayTime::Reset(void)
{
	//Reset timing

	//Clear bytes of structure
	memset(&this->m_sPlayTime, 0x00, sizeof(playtime_s));

	//Initialize timer
	this->m_sPlayTime.dwTmrCur = this->m_sPlayTime.dwTmrLast = GetTickCount();
}
//======================================================================

//======================================================================
void CPlayTime::Think(void)
{
	//Process time calculations

	this->m_sPlayTime.dwTmrCur = GetTickCount(); //Update current timer
	if (this->m_sPlayTime.dwTmrCur >= this->m_sPlayTime.dwTmrLast + 1000) { //Check if one second has elapsed
		//Refresh last timer
		this->m_sPlayTime.dwTmrLast = GetTickCount();

		this->m_sPlayTime.dwCurSecs++; //Increment seconds
		if (this->m_sPlayTime.dwCurSecs >= 60) { //One minute is over
			this->m_sPlayTime.dwCurSecs = 0; //Count from zero again
			this->m_sPlayTime.dwCurMins++; //Increment minutes
			if (this->m_sPlayTime.dwCurMins >= 60) { //One hour is over
				this->m_sPlayTime.dwCurMins = 0; //Count from zero again
				this->m_sPlayTime.dwCurHours++; //Increment hours
			}
		}
	}
}
//======================================================================
