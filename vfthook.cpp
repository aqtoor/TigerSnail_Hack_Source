#include "vfthook.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.3
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: vfthook.cpp: VFT Hook manager implementations
*/

//======================================================================
void CVFTHookMgr::Release(void)
{
	//Release resources

	//Free allocated memory
	for (size_t i = 0; i < this->m_vObjects.size(); i++) {
		delete [] this->m_vObjects[i].pTableBackup;
	}

	//Empty list
	this->m_vObjects.clear();
}
//======================================================================

//======================================================================
vft_class_s* CVFTHookMgr::FindClassContext(const std::string& szIdent)
{
	//Get pointer of class context data if exists

	for (size_t i = 0; i < this->m_vObjects.size(); i++) {
		if (this->m_vObjects[i].szIdent == szIdent)
			return &this->m_vObjects[i];
	}

	return NULL;
}
//======================================================================

//======================================================================
bool CVFTHookMgr::Add(const std::string& szIdent, const VFTPOINTER pClassBase, const size_t uiVFTSize)
{
	//Add new class context

	if ((!szIdent.length()) || (!pClassBase) || (!uiVFTSize))
		return false;

	//Check if already exists
	if (this->FindClassContext(szIdent))
		return false;

	//Create data struct on stack
	vft_class_s vftClass;

	//Allocate memory for vft backup
	vftClass.pTableBackup = new VFTPOINTER[uiVFTSize];
	if (!vftClass.pTableBackup)
		return false;

	//Copy vft
	memcpy(vftClass.pTableBackup, (const void*)*(VFTPOINTER*)pClassBase, uiVFTSize * sizeof(VFTPOINTER));

	//Save further data
	vftClass.szIdent = szIdent;
	vftClass.pClassBase = pClassBase;
	vftClass.uiTableSize = uiVFTSize;
	vftClass.pVirtualFunctionTable = *(VFTPOINTER**)pClassBase;

	//Add to list
	this->m_vObjects.push_back(vftClass);

	return true;
}
//======================================================================

//======================================================================
VFTPOINTER CVFTHookMgr::Hook(const std::string& szIdent, const size_t uiMethodId, const VFTPOINTER pfnNewMethod)
{
	//Hook vft method
	
	//Find context data pointer
	vft_class_s* pData = this->FindClassContext(szIdent);
	if (!pData)
		return NULL;
	
	DWORD dwOldProtect;
	
	//Change memory protection rights
	if (!VirtualProtect((LPVOID)(pData->pVirtualFunctionTable + uiMethodId), sizeof(VFTPOINTER), PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
		return false;
	}
	
	//Hook method
	pData->pVirtualFunctionTable[uiMethodId] = pfnNewMethod;

	//Reset memory protection rights
	if (!VirtualProtect((LPVOID)(pData->pVirtualFunctionTable + uiMethodId), sizeof(VFTPOINTER), dwOldProtect, &dwOldProtect)) {
		return false;
	}
	
	//Return original method address
	return pData->pTableBackup[uiMethodId];
}
//======================================================================

//======================================================================
bool CVFTHookMgr::Unhook(const std::string& szIdent, const size_t uiMethodId)
{
	//Unhook vft method

	//Find context data pointer
	vft_class_s* pData = this->FindClassContext(szIdent);
	if (!pData)
		return false;

	DWORD dwOldProtect;

	//Change memory protection rights
	if (!VirtualProtect((LPVOID)(pData->pVirtualFunctionTable + uiMethodId), sizeof(VFTPOINTER), PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
		return false;
	}

	//Restore original method address
	pData->pVirtualFunctionTable[uiMethodId] = pData->pTableBackup[uiMethodId];

	//Reset memory protection rights
	if (!VirtualProtect((LPVOID)(pData->pVirtualFunctionTable + uiMethodId), sizeof(VFTPOINTER), dwOldProtect, &dwOldProtect)) {
		return false;
	}

	return true;
}
//======================================================================

//======================================================================
VFTPOINTER CVFTHookMgr::GetClass(const std::string& szIdent)
{
	//Get class instance address

	//Find context data pointer
	vft_class_s* pData = this->FindClassContext(szIdent);
	if (!pData)
		return false;

	//Return address
	return pData->pClassBase;
}
//======================================================================

//======================================================================
VFTPOINTER CVFTHookMgr::GetOrigAddr(const std::string& szIdent, const size_t uiMethodId)
{
	//Get original method address

	//Find context data pointer
	vft_class_s* pData = this->FindClassContext(szIdent);
	if (!pData)
		return NULL;

	//Return address
	return pData->pTableBackup[uiMethodId];
}
//======================================================================
