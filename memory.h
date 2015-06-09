#ifndef _MEMORY_H
#define _MEMORY_H

#include "includes.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.1
	Visit: http://sk0r.sytes.net

	File: memory.h: Memory interface 
*/

//======================================================================
struct header_info_s {
	HMODULE dwModuleBase;
    DWORD dwCodeAddr;
    DWORD dwCodeSize;
    DWORD dwDataAddr;
    DWORD dwDataSize;
    DWORD dwBSSSize;
};
//======================================================================

//======================================================================
DWORD FindString(const char *lpszStringToFind, DWORD dwBase, DWORD dwEnd, DWORD *dwContinueBase);
DWORD FindSignature(const LPVOID lpvBuffer, const DWORD dwBufLen, DWORD dwBase, DWORD dwEnd, DWORD* dwContinueBase);
bool GetModuleSegmentInfo(const char* lpszModule, header_info_s* pHdrInfo);
LPVOID Detour(LPVOID pOrigAddr, LPVOID pNewAddr, DWORD uiSize);
//======================================================================

#endif
