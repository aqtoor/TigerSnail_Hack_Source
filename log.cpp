#include "log.h"
#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.3
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: log.cpp: Log implementations
*/

//======================================================================
CLog::CLog(const std::string& szPrefix, const std::string& szFile)
{
	//Construct and open file

	this->m_szPrefix = szPrefix;

	this->m_hFile.open(szFile);
}
//======================================================================

//======================================================================
bool CLog::Open(const std::string& szFile)
{
	//Open file

	this->m_hFile.open(szFile);

	return this->Opened();
}
//======================================================================

//======================================================================
bool CLog::Msg(const std::string& szMessage)
{
	//Write message text

	if (!this->Opened())
		return false;

	this->m_hFile << "[" << this->m_szPrefix << "] [Message] " << szMessage << std::endl;

	return true;
}
//======================================================================

//======================================================================
bool CLog::MsgFmt(const char* szMessage, ...)
{
	//Write formatted text

	if (!this->Opened())
		return false;

	char szBuffer[MAX_PATH];

	va_list vaList;
	va_start(vaList, szMessage);
	vsprintf_s(szBuffer, szMessage, vaList);
	va_end(vaList);
	
	return this->Msg(std::string(szBuffer));
}
//======================================================================

//======================================================================
bool CLog::Warning(const std::string& szMessage)
{
	//Write warning text

	if (!this->Opened())
		return false;

	this->m_hFile << "[" << this->m_szPrefix << "] [Warning] " << szMessage << std::endl;

	return true;
}
//======================================================================

//======================================================================
bool CLog::Error(const std::string& szMessage)
{
	//Write error text

	if (!this->Opened())
		return false;

	this->m_hFile << "[" << this->m_szPrefix << "] [Error] " << szMessage << std::endl;

	return true;
}
//======================================================================

//======================================================================
void CLog::Close(void)
{
	//Close file

	this->m_hFile.close();
}
//======================================================================
