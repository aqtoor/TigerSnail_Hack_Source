#include "utils.h"
#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	utils.cpp: Utility method/function implementations
*/

//======================================================================
bool CDecoyESP::Add(const Vector& vPos, const int iUserId, const int iEntityId)
{
	//Add decoy entry data

	if ((!iUserId) || (!iEntityId))
		return false;

	//Setup data
	de_entry_s sEntry;
	sEntry.vPos = vPos;
	sEntry.iUserId = iUserId;
	sEntry.iEntityId = iEntityId;
	sEntry.eStatus = DES_STARTED;

	//Add to list
	this->m_vEntries.push_back(sEntry);

	return true;
}
//======================================================================

//======================================================================
bool CDecoyESP::UpdateStatus(const int iEntityId, const de_status_e eNewStatus, const Vector& vPos)
{
	//Update status of decoy entry

	if ((!iEntityId) || (eNewStatus == DES_STARTED))
		return false;

	for (std::size_t i = 0; i < this->m_vEntries.size(); i++) { //Loop through entries
		if (this->m_vEntries[i].iEntityId == iEntityId) { //If decoy with entity id has been found
			//Update data
			this->m_vEntries[i].eStatus = eNewStatus;
			this->m_vEntries[i].vPos = vPos;

			//If in firing mode then update timing values
			if (eNewStatus == DES_FIRING) {
				this->m_vEntries[i].dwFireColorCur = this->m_vEntries[i].dwFireColorInit = GetTickCount();
			}

			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
void CDecoyESP::Think(void)
{
	//Process entries

	for (std::size_t i = 0; i < this->m_vEntries.size(); i++) { //Loop through entries
		_player_info_t sPlayerInfo;
		bool bResult = g_pEngineClient->GetPlayerInfo(this->m_vEntries[i].iUserId-1, (player_info_t*)&sPlayerInfo); //Attempt to get player info

		bool bInFiringColorRange = false; //Indicator whether inside firing event time range
		if (this->m_vEntries[i].eStatus == DES_FIRING) { //If decoy status is in firing mode
			this->m_vEntries[i].dwFireColorCur = GetTickCount(); //Update current timer value
			if (this->m_vEntries[i].dwFireColorCur <= this->m_vEntries[i].dwFireColorInit + 170) //If the given time has not elapsed
				bInFiringColorRange = true; //Indicate that inside range
		}

		//Draw decoy info
		g_pDebugOverlay->AddTextOverlayRGB(this->m_vEntries[i].vPos, 0, 1.0f, (bInFiringColorRange) ? this->m_sFiringColor.r : this->m_sDefColor.r, (bInFiringColorRange) ? this->m_sFiringColor.g : this->m_sDefColor.g, (bInFiringColorRange) ? this->m_sFiringColor.b : this->m_sDefColor.b, 255, "Decoy");
		g_pDebugOverlay->AddTextOverlayRGB(this->m_vEntries[i].vPos, 1, 1.0f, (bInFiringColorRange) ? this->m_sFiringColor.r : this->m_sDefColor.r, (bInFiringColorRange) ? this->m_sFiringColor.g : this->m_sDefColor.g, (bInFiringColorRange) ? this->m_sFiringColor.b : this->m_sDefColor.b, 255, (bResult) ? sPlayerInfo.name : "Unknown");
		
		//If status has changed to exploded then remove decoy entry
		if (this->m_vEntries[i].eStatus == DES_EXPLODED) {
			this->m_vEntries.erase(this->m_vEntries.begin() + i);
			break;
		}
	}
}
//======================================================================

//======================================================================
bool FileExists(const std::string& szFileName)
{
	//Check if a file exists on disk

	std::ifstream hInputFile(szFileName);
	if (hInputFile.is_open()) {
		hInputFile.close();
		return true;
	}

	return false;
}
//======================================================================

//======================================================================
bool DeleteFileObject(const std::string& szObject)
{
	//Delete file or folder

	char szFolder[MAX_PATH];
	
	//Setup string with folder name
	strcpy_s(szFolder, szObject.c_str());
	memset(&szFolder[MAX_PATH-2], 0x00, 2); //To ensure the string is double-zero-terminated

	//Setup SH operation structure
	SHFILEOPSTRUCTA shOpStruct;
	
	memset(&shOpStruct, 0x00, sizeof(shOpStruct));

	shOpStruct.wFunc = FO_DELETE; //Delete operation
	shOpStruct.pFrom = szFolder; //Name string of object to delete
	shOpStruct.fFlags = FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION; //Don't show error GUI, deletion operation GUI and don't ask for confirmation

	return SHFileOperationA(&shOpStruct) == ERROR_SUCCESS; //Perform operation
}
//======================================================================

//======================================================================
bool GetCurrentDateAndTime(std::string& szDateOut, std::string& szTimeOut)
{
	//Get current date and time

	time_t tmrTime = time(NULL);
	tm* pTime = localtime(&tmrTime);
	if (!pTime)
		return false;

	static char szString[1024];

	strftime(szString, sizeof(szString), "%Y-%m-%d", pTime);
	szDateOut = std::string(szString);

	strftime(szString, sizeof(szString), "%X", pTime);
	szTimeOut = std::string(szString);

	return true;
}
//======================================================================

//======================================================================
_C_BaseEntity* GetClientBaseEntity(int iEntIndex)
{
	//Get pointer to instance of _C_BaseEntity via the given entity index

	//Get pointer to client networkable instance
	IClientNetworkable* pClientNetworkable = g_pClientEntityList->GetClientNetworkable(iEntIndex);
	if (!pClientNetworkable)
		return NULL;

	//Get pointer to client unknown instance
	IClientUnknown* pClientUnknown = pClientNetworkable->GetIClientUnknown();
	if (!pClientUnknown)
		return NULL;

	//Return instance pointer
	return (_C_BaseEntity*)pClientUnknown->GetBaseEntity();
}
//======================================================================
