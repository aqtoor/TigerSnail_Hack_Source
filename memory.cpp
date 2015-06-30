#include "memory.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: memory.cpp: Memory related stuff 
*/

//======================================================================
DWORD FindString(const char *lpszStringToFind, DWORD dwBase, DWORD dwEnd, DWORD *dwContinueBase)
{
    //Return the start address of a string. If it's neccessary to continue searching the next, then specifiy the dwContinueBase pointer.

    DWORD dwResult = 0; //Result is 0

    //Check parameter
    if (!lpszStringToFind)
        return dwResult;

    //If no base address specified, then use the main modulse base address
    if (!dwBase)
        dwBase = (DWORD)GetModuleHandle(NULL);

    for (DWORD i = dwBase; i < dwEnd; i++) { //Walk trough section
        if (strncmp((char*)i, lpszStringToFind, strlen(lpszStringToFind))==0) { //If string has been found, save and break
            dwResult = i;

            //Only in case to continue searching the next
            if (dwContinueBase)
                *dwContinueBase = i + strlen(lpszStringToFind);

            break;
        }
    }

    return dwResult;
}
//======================================================================

//======================================================================
DWORD FindSignature(const LPVOID lpvBuffer, const DWORD dwBufLen, DWORD dwBase, DWORD dwEnd, DWORD* dwContinueBase)
{
	//Search for a memory signature and return its address

	if ((!lpvBuffer) || (!dwBufLen) || (!dwBase) || (!dwEnd))
		return 0;

	for (unsigned int i = dwBase; i < dwEnd; i++) { //Loop to specified memory section
		if (!memcmp((LPVOID)i, lpvBuffer, dwBufLen)) { //Check if buffer matches the signature
			return i; //If so return address
		}
	}

	return 0;
}
//======================================================================

//======================================================================
bool GetModuleSegmentInfo(const char* lpszModule, header_info_s* pHdrInfo)
{
	//Get code and data address and size infos

	if ((!lpszModule) || (!pHdrInfo))
		return false;

	HMODULE hModule = GetModuleHandleA(lpszModule); //Get module handle
	if (!hModule)
		return false;

	pHdrInfo->dwModuleBase = hModule; //Save module base

	IMAGE_DOS_HEADER *pDosHdr = (IMAGE_DOS_HEADER*)pHdrInfo->dwModuleBase; //Get DOS header pointer
	if (!pDosHdr)
		return false;

	IMAGE_NT_HEADERS* pNtHdr = (IMAGE_NT_HEADERS*)((DWORD)pDosHdr + pDosHdr->e_lfanew); //Save address to NT header
	if (pNtHdr->Signature == IMAGE_NT_SIGNATURE) { //Check if NT header is valid
		pHdrInfo->dwCodeAddr = (DWORD)pDosHdr + pNtHdr->OptionalHeader.BaseOfCode;    //Save absolute address to code segment address
		pHdrInfo->dwCodeSize = pNtHdr->OptionalHeader.SizeOfCode;    //Save size of code segment
		pHdrInfo->dwDataAddr = (DWORD)pDosHdr + pNtHdr->OptionalHeader.BaseOfData; //Save absolute address to data segment address
		pHdrInfo->dwDataSize = pNtHdr->OptionalHeader.SizeOfInitializedData; //Save size of data segment
		pHdrInfo->dwBSSSize = pNtHdr->OptionalHeader.SizeOfUninitializedData; //Save size of bss segment

		return true;
	}

	return false;
}
//======================================================================

//======================================================================
LPVOID Detour(LPVOID pOrigAddr, LPVOID pNewAddr, DWORD uiSize)
{
    //Basic JMP detour function

    #ifndef NOP
    #define NOP 0x90
    #endif

    #ifndef JMP
    #define JMP 0xE9
    #endif

	#ifndef JMP_INSTRUCTION_LEN
	#define JMP_INSTRUCTION_LEN 0x05
	#endif

    if ((!pOrigAddr) || (!pNewAddr) || (!uiSize) || (uiSize < JMP_INSTRUCTION_LEN))
		return NULL;

	//Setup jump instruction
	static BYTE ucJmpInstr[JMP_INSTRUCTION_LEN] = {JMP, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)&ucJmpInstr[1] = (DWORD)pNewAddr - (DWORD)pOrigAddr - JMP_INSTRUCTION_LEN;
	
	DWORD dwProtect;

	//Change access protection rights
	if (!VirtualProtect(pOrigAddr, uiSize, PAGE_EXECUTE_READWRITE, &dwProtect))
		return NULL;

	//Write NOP if target area is greater than a jump instruction
	if (uiSize > JMP_INSTRUCTION_LEN) {
		for (DWORD i = 0; i < uiSize; i++) {
			*(BYTE*)((DWORD)pOrigAddr + i) = NOP;
		}
	}

	//Write jump instruction to target address
	memcpy(pOrigAddr, ucJmpInstr, sizeof(ucJmpInstr));

	//Restore old protection rights
	if (!VirtualProtect(pOrigAddr, uiSize, dwProtect, &dwProtect))
		return NULL;

	return (LPVOID)((DWORD)pOrigAddr + uiSize); //Return address to overjump overwritten original code
}
//======================================================================
