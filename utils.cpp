#include "utils.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	utils.cpp: Utility function implementations
*/

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
