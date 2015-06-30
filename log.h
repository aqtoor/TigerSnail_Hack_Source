#ifndef _LOG_H
#define _LOG_H

#include "includes.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: log.h: Log interface 
*/

//======================================================================
class CLog {
private:
	std::ofstream m_hFile;
	std::string m_szPrefix;

	void Close(void);
public:
	CLog(const std::string& szPrefix) { this->m_szPrefix = szPrefix; }
	CLog(const std::string& szPrefix, const std::string& szFile);

	~CLog() { this->Close(); }

	bool Open(const std::string& szFile);
	void SetPrefix(const std::string& szPrefix) { this->m_szPrefix = szPrefix; }

	bool Msg(const std::string& szMessage);
	bool MsgFmt(const char* szMessage, ...);
	bool Warning(const std::string& szMessage);
	bool Error(const std::string& szMessage);

	inline bool Opened(void) { return this->m_hFile.is_open(); }
};
//======================================================================

#endif
