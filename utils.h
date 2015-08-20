#ifndef _UTILS_H
#define _UTILS_H

#include "includes.h"
#include "gamespec.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.5
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	utils.h: Utility interface
*/

//======================================================================
class CDecoyESP {
public:
	enum de_status_e {
		DES_STARTED,
		DES_FIRING,
		DES_EXPLODED
	};
private:
	struct de_entry_s {
		Vector vPos;
		int iUserId, iEntityId;
		de_status_e eStatus;
		DWORD dwFireColorCur, dwFireColorInit;
	};

	std::vector<de_entry_s> m_vEntries;
	color24 m_sDefColor;
	color24 m_sFiringColor;
public:
	CDecoyESP() {}
	CDecoyESP(color24& sDefaultColor, color24& sFiringColor) { this->SetColor(sDefaultColor, sFiringColor); }
	~CDecoyESP() { this->m_vEntries.clear(); }

	inline void SetColor(const color24& sDefaultColor, const color24& sFiringColor) { this->m_sDefColor = sDefaultColor; this->m_sFiringColor = sFiringColor; }

	bool Add(const Vector& vPos, const int iUserId, const int iEntityId);
	bool UpdateStatus(const int iEntityId, const de_status_e eNewStatus, const Vector& vPos);

	void Think(void);
};

class CCustomTraceFilter : public ITraceFilter {
private:
	IHandleEntity* m_pEntityToSkip;
public:
	CCustomTraceFilter(IHandleEntity* pEntityToSkip)
	{
		this->m_pEntityToSkip = pEntityToSkip;
	}

	virtual bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask)
	{
		return (pEntity != this->m_pEntityToSkip) ? true : false;
	}

	virtual TraceType_t	GetTraceType(void) const
	{
		return TRACE_EVERYTHING;
	}
};
//======================================================================

//======================================================================
bool FileExists(const std::string& szFileName);
bool DeleteFileObject(const std::string& szObject);
bool GetCurrentDateAndTime(std::string& szDateOut, std::string& szTimeOut);
_C_BaseEntity* GetClientBaseEntity(int iEntIndex);
bool PathFree(const Vector& vStart, const Vector& vEnd, const IHandleEntity* pIgnoreThisEnt, const unsigned int uiMask = MASK_ALL);
bool SimulateLeftClick();
//======================================================================

#endif
