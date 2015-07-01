#ifndef _VFT_HOOK
#define _VFT_HOOK

#include "includes.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.3
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: vfthook.h: VFT Hook manager interface 
*/

//======================================================================
typedef DWORD_PTR VFTPOINTER;
//======================================================================

//======================================================================
struct vft_class_s {
	std::string	szIdent;
	VFTPOINTER pClassBase;
	VFTPOINTER* pVirtualFunctionTable;
	size_t uiTableSize;
	VFTPOINTER* pTableBackup;
};
//======================================================================

//======================================================================
class CVFTHookMgr {
private:
	std::vector<vft_class_s> m_vObjects;

	void Release(void);

	vft_class_s* FindClassContext(const std::string& szIdent);
public:
	CVFTHookMgr() {}
	~CVFTHookMgr() { this->Release(); }

	bool Add(const std::string& szIdent, const VFTPOINTER pClassBase, const size_t uiVFTSize);
	VFTPOINTER Hook(const std::string& szIdent, const size_t uiMethodId, const VFTPOINTER pfnNewMethod);
	bool Unhook(const std::string& szIdent, const size_t uiMethodId);

	VFTPOINTER GetClass(const std::string& szIdent);
	VFTPOINTER GetOrigAddr(const std::string& szIdent, const size_t uiMethodId);
};
//======================================================================

#endif
