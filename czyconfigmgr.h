#pragma once

/*
	CzyConfigMgr - Config manager context

	Developed by sk0r / Czybik
	Version: 0.3
	Contact: Czybik_Stylez<0x40>gmx<0x2E>de

	Description:
		This product offers the possibility to
		manage program configurations via scripts

	Features:
		- CVars (types: bool, int, float, double and string)
		- Commands (with event function)
		- Commandline handler
		- Script expression
			- Any amount of spaces/tabs between each argument
			- Separate arguments via quotes and/or brackets(nested)
			- Separate item expressions via delimiter
			- Use comments
		- Config interface

	Script examples:
		- expression value1 value2 value3
		- expression "value1 value2" "value3" # This is a comment
		- expression "argument 1" [argument 2]
		- expression     "this is"      [a]       "test" # :-)
		- expression "arg1"; expression arg2; expression arg3
	Commandline examples:
		- +cl_variable1 1 +cl_variable2 2
		- -argument1 -argument2 "value" -argument3 1 2 3
		- +cl_variable 1 -argument

*/

//======================================================================
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
//======================================================================

//======================================================================
#define CCM_PRODUCT_NAME "CzyConfigMgr"
#define CCM_PRODUCT_VERSION "0.3"
#define CCM_PRODUCT_AUTHOR "sk0r / Czybik"
#define CCM_PRODUCT_CONTACT "Czybik_Stylez<0x40>gmx<0x2E>de"

#define CCM_CVAR_MAX_STRING_LEN 256

#if defined(_WIN32) || defined(_WIN64)
	#define CCM_DIR_CHAR '\\'
#elif defined(__linux__)
	#define CCM_DIR_CHAR '/'
#endif
//======================================================================

//======================================================================
namespace CzyConfigMgr {
#if 0
	const std::string About(void)
	{
		return CCM_PRODUCT_NAME " v" CCM_PRODUCT_VERSION " developed by " CCM_PRODUCT_AUTHOR " (" CCM_PRODUCT_CONTACT ")";
	}
#endif

	class CCVar {
	public: 
		enum cvar_type_e {
			CVAR_TYPE_BOOL,
			CVAR_TYPE_INT,
			CVAR_TYPE_FLOAT,
			CVAR_TYPE_DOUBLE,
			CVAR_TYPE_STRING
		};

		struct cvar_s {
			std::string szName;
			cvar_type_e eType;
			union {
				bool bValue;
				int iValue;
				float fValue;
				double dblValue;
				char szValue[CCM_CVAR_MAX_STRING_LEN];
			};
		};
	private:
		std::vector<cvar_s*> m_vCVars;

		void Release(void)
		{
			//Release all cvars

			//Free memory
			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				delete this->m_vCVars[i];
			}

			//Clear list
			this->m_vCVars.clear();
		}
	public:
		CCVar() {}
		~CCVar() { this->Release(); }

		cvar_s* Add(const std::string& szName, const cvar_type_e eType, const std::string& szInitialValue)
		{
			//Add CVar

			if (!szName.length())
				return NULL;

			//Check if already exists
			if (this->Find(szName))
				return NULL;

			//Allocate memory
			cvar_s* pCVar = new cvar_s;
			if (!pCVar)
				return NULL;

			//Save data
			pCVar->szName = szName;
			pCVar->eType = eType;

			//Set initial value
			if (szInitialValue.length()) {
				switch (eType) {
				case CVAR_TYPE_BOOL:
					pCVar->bValue = atoi(szInitialValue.c_str()) != 0;
					break;
				case CVAR_TYPE_INT:
					pCVar->iValue = atoi(szInitialValue.c_str());
					break;
				case CVAR_TYPE_FLOAT:
					pCVar->fValue = (float)atof(szInitialValue.c_str());
					break;
				case CVAR_TYPE_DOUBLE:
					pCVar->dblValue = atof(szInitialValue.c_str());
					break;
				case CVAR_TYPE_STRING:
					strcpy_s(pCVar->szValue, szInitialValue.c_str());
					break;
				default:
					break;
				}
			}

			//Add to list
			this->m_vCVars.push_back(pCVar);

			//Return pointer to data
			return pCVar;
		}

		cvar_s* Find(const std::string& szName)
		{
			//Find CVar in list

			if (!szName.length())
				return NULL;

			//Search in list
			for (size_t i = 0; i < this->m_vCVars.size(); i++) {
				if ((this->m_vCVars[i]) && (this->m_vCVars[i]->szName == szName)) {
					return this->m_vCVars[i];
				}
			}

			return NULL;
		}

		bool Set(const std::string& szName, const bool bValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->bValue = bValue;

			return true;
		}

		bool Set(const std::string& szName, const int iValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->iValue = iValue;

			return true;
		}

		bool Set(const std::string& szName, const float fValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->fValue = fValue;

			return true;
		}

		bool Set(const std::string& szName, const double dblValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			pCVar->dblValue = dblValue;

			return true;
		}

		bool Set(const std::string& szName, const std::string& szValue)
		{
			//Set value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			strcpy_s(pCVar->szValue, szValue.c_str());

			return true;
		}

		bool SetCast(const std::string& szName, const std::string& szValue)
		{
			//Set casted value

			cvar_s* pCVar = this->Find(szName);
			if (!pCVar)
				return false;

			switch (pCVar->eType) {
				case CVAR_TYPE_BOOL:
					pCVar->bValue = atoi(szValue.c_str()) != 0;
					break;
				case CVAR_TYPE_INT:
					pCVar->iValue = atoi(szValue.c_str());
					break;
				case CVAR_TYPE_FLOAT:
					pCVar->fValue = (float)atof(szValue.c_str());
					break;
				case CVAR_TYPE_DOUBLE:
					pCVar->dblValue = atof(szValue.c_str());
					break;
				case CVAR_TYPE_STRING:
					strcpy_s(pCVar->szValue, szValue.c_str());
					break;
				default:
					return false;
					break;
			}

			return true;
		}
	};

	class CCommand {
	public:
		typedef void (*TCommandCallback)(void);
	private:
		struct command_s {
			std::string szName;
			TCommandCallback pCallbackRoutine;
		};

		std::vector<command_s> m_vCmds;

	protected:
		bool Find(const std::string& szName, size_t* pEntryId)
		{
			//Check if a command exists

			for (size_t i = 0; i < this->m_vCmds.size(); i++) {
				if (this->m_vCmds[i].szName == szName) {
					if (pEntryId)
						*pEntryId = i;

					return true;
				}
			}

			return false;
		}
	public:
		CCommand() {}
		~CCommand() { this->m_vCmds.clear(); }

		bool Add(const std::string& szName, const TCommandCallback pRoutine)
		{
			//Add new command

			if ((!szName.length()) || (!pRoutine))
				return false;

			//Check if already exists
			if (this->Find(szName, NULL))
				return false;

			//Setup data
			command_s sData;
			sData.szName = szName;
			sData.pCallbackRoutine = pRoutine;

			//Add to list
			this->m_vCmds.push_back(sData);
			
			return true;
		}

		bool Handle(const std::string& szName)
		{
			//Check if command exists and if so call event function

			size_t uiCmdId;

			//Attempt to find entry and retrieve id
			if (!this->Find(szName, &uiCmdId))
				return false;

			//Check pointer
			if (!this->m_vCmds[uiCmdId].pCallbackRoutine)
				return false;

			//Call event function
			this->m_vCmds[uiCmdId].pCallbackRoutine();

			return true;
		}
	};

	class CCmdLine : public CCVar {
	private:
		static const char C_CVAR_CHAR = '+';
		static const char C_ARG_CHAR = '-';
	public:
		struct cmd_arg_s {
			std::string szName;
			std::vector<std::string> vArgs;
		};
	private:
		std::vector<cmd_arg_s*> m_vArgs;
		char** m_ppArgs;
		int m_iCount;

		int HandleArg(const std::string& szName, int iArgStart)
		{
			//Handle current argument

			int iIgnoreCount = 0; //How much arg strings have to be ignored

			//Allocate memory
			cmd_arg_s* pArg = new cmd_arg_s;
			if (!pArg)
				return -1;

			pArg->szName = szName;
			
			for (int i = iArgStart; i < this->m_iCount; i++) { //Loop through args
				//Check if current arg string is a new item and if so break out
				if ((this->m_ppArgs[i][0] == C_CVAR_CHAR) || (this->m_ppArgs[i][0] == C_ARG_CHAR)) {
					break;
				}
				
				//Add item arg to list
				pArg->vArgs.push_back(std::string(this->m_ppArgs[i]));

				//Increment counter so that this item arg is ignored
				iIgnoreCount++;
			}

			//Add to list
			this->m_vArgs.push_back(pArg);

			return iIgnoreCount;
		}

		void Release(void)
		{
			//Release resources
			
			//Free memory
			for (size_t i = 0; i < this->m_vArgs.size(); i++) {
				delete this->m_vArgs[i];
			}

			//Clear list
			this->m_vArgs.clear();
		}
	public:
		CCmdLine() : m_ppArgs(NULL), m_iCount(0) {}
		CCmdLine(char** ppArgs, const int iArgCount) : m_ppArgs(ppArgs), m_iCount(iArgCount) { this->Collect(); }
		~CCmdLine() { this->Release(); }

		void SetCmdArgs(char** ppArgs, const int iArgCount) { this->m_ppArgs = ppArgs; this->m_iCount = iArgCount; }

		bool Collect(void)
		{
			//Collect all items

			if ((!this->m_ppArgs) || (!this->m_iCount))
				return false;

			//Check if arguments have been provided
			if (this->m_iCount == 1)
				return true;

			for (int i = 1; i < this->m_iCount; i++) { //Loop through args
				//There shouldn't be a null-pointer but if so this is a failure
				if (!this->m_ppArgs[i])
					return false;

				if (this->m_ppArgs[i][0] == C_CVAR_CHAR) { //Check for cvar char
					//Check if we would exceed the limit
					if (i + 1 >= this->m_iCount)
						return false;

					//Attempt to get pointer to value string
					const char* pszCVarValue = this->m_ppArgs[i + 1];
					if (!pszCVarValue)
						return false;

					//Set cvar value
					this->SetCast(std::string(&this->m_ppArgs[i][1]), std::string(pszCVarValue));

					//Ignore cvar value iteration
					i++;

					//Continue with next iteration
					continue;
				} else if (this->m_ppArgs[i][0] == C_ARG_CHAR) { //Check for arg char
					int iIgnoreCount = HandleArg(std::string(&this->m_ppArgs[i][1]), i + 1);
					if (iIgnoreCount == -1)
						return false;

					//Increment according to ignore count
					i += iIgnoreCount;
					
					//Continue with next iteration
					continue;
				} else {
					//Unknown item
					return false;
				}
			}
			
			return true;
		}

		const size_t Count(void)
		{
			//Return amount of items

			return this->m_vArgs.size();
		}

		cmd_arg_s* Item(const size_t uiId)
		{
			//Get item by ID

			if (uiId >= this->m_vArgs.size())
				return NULL;

			return this->m_vArgs[uiId];
		}
	};

	class CScriptParser : public CCmdLine, public CCommand {
	private:
		//Syntax char elements
		static const char C_SPACE_CHAR = ' ';
		static const char C_TAB_CHAR = '\t';
		static const char C_QUOTE_CHAR = '\"';
		static const char C_BRACKET_START_CHAR = '[';
		static const char C_BRACKET_END_CHAR = ']';
		static const char C_COMMENT_CHAR = '#';
		static const char C_ITEMS_DELIM = ';';

		std::vector<std::string> m_vExpressionItems; //Items for current handled expression

		TCommandCallback pfnUnknownExpressionInform; //Function to call when encountered an unknown expression

		void HandleCurrentItem(bool bClear = false)
		{
			if (this->m_vExpressionItems.size()) { //Check if not empty
				//Handle cvar if it is one
				if ((this->m_vExpressionItems.size() > 1) && (CCVar::SetCast(this->m_vExpressionItems[0], this->m_vExpressionItems[1]))) {
					//Clear list if desired
					if (bClear)
						this->m_vExpressionItems.clear();

					return;
				}

				//Handle command if it is one
				if (CCommand::Handle(this->m_vExpressionItems[0])) {
					//Clear list if desired
					if (bClear)
						this->m_vExpressionItems.clear();

					return;
				}

				//Inform about unknown expression
				if (this->pfnUnknownExpressionInform)
					this->pfnUnknownExpressionInform();

				//Clear list if desired
				if (bClear)
					this->m_vExpressionItems.clear();
			}
		}
	public:
		CScriptParser() : pfnUnknownExpressionInform(NULL) {}

		CScriptParser(const TCommandCallback pfnFunction) : pfnUnknownExpressionInform(pfnFunction) {}

		CScriptParser(const TCommandCallback pfnFunction, const std::string& szExpression) : pfnUnknownExpressionInform(pfnFunction)
		{
			this->Parse(szExpression);
		}

		~CScriptParser() { this->m_vExpressionItems.clear(); }

		bool SetUnknownExpressionInformer(const TCommandCallback pfnFunction)
		{
			//Set event function for unknown expressions

			if (!pfnFunction)
				return false;

			this->pfnUnknownExpressionInform = pfnFunction;

			return true;
		}

		bool Parse(const std::string szExpression)
		{
			//Parse expression

			if (!szExpression.length())
				return false;

			bool bInQuotes = false; //Whether in-quote context is true or false
			bool bGotNonSpaceChar = false; //Whether if a non-space char has been encountered
			short swBracketCounter = 0; //Bracket counter
			std::string szTemp(""); //Temporary string
			
			this->m_vExpressionItems.clear();
			
			for (size_t i = 0; i <= szExpression.length(); i++) { //Loop through string chars
				//If current char is a quote char then toggle indicator value
				if (szExpression[i] == C_QUOTE_CHAR) {
					bInQuotes = !bInQuotes;

					//Ignore only if not in brackets
					if (!swBracketCounter)
						continue;
				}

				//Set indicator to true if case is true
				if (((!bGotNonSpaceChar) && ((szExpression[i] != C_SPACE_CHAR) || (szExpression[i] != C_TAB_CHAR)))) {
					bGotNonSpaceChar = true;
				}

				//Ignore leading spaces
				if (!bGotNonSpaceChar)
					continue;

				if (!bInQuotes) { //Outside of an expression item
					//If current char is a space/tab char and also the next char then ignore current char
					if (((szExpression[i] == C_SPACE_CHAR) || (szExpression[i] == C_TAB_CHAR)) && ((szExpression[i + 1] == C_SPACE_CHAR) || (szExpression[i + 1] == C_TAB_CHAR))) {
						continue;
					}

					//Check for brackets
					if (szExpression[i] == C_BRACKET_START_CHAR) {
						swBracketCounter++; //Increment counter

						//Ignore only if this is the first one
						if (swBracketCounter == 1)
							continue;
					} else if (szExpression[i] == C_BRACKET_END_CHAR) {
						swBracketCounter--; //Decrement counter

						//Ignore only if this is the last one
						if (swBracketCounter == 0)
							continue;
					}

					//Check if a delimiter char has been found or comment or end of string reached
					if  ((!swBracketCounter) && ((szExpression[i] == C_SPACE_CHAR) || (szExpression[i] == C_TAB_CHAR) || (szExpression[i] == C_BRACKET_END_CHAR) || (szExpression[i] == C_COMMENT_CHAR) || (szExpression[i] == C_ITEMS_DELIM) || (!szExpression[i]))) {
						//The first expression may not be empty
						if ((!this->m_vExpressionItems.size()) && ((i > 0) && (!szTemp.length()))) {
							szTemp.clear(); //Clear string
							continue;
						}

						this->m_vExpressionItems.push_back(szTemp); //Add item to list

						//Break out if end reached
						if (szExpression[i] == C_COMMENT_CHAR)
							break;

						szTemp.clear(); //Clear temp string
						//Handle expression now if item delimiter has been found
						if (szExpression[i] == C_ITEMS_DELIM) {
							this->HandleCurrentItem(true); //Handle current item
							bGotNonSpaceChar = false; //Reset indicator
						}

						continue; //Ignore current char
					}
				}

				//Append current char
				szTemp += szExpression[i];
			}

			this->HandleCurrentItem(); //Handle current item

			return true;
		}

		const size_t ExpressionItemCount(void)
		{
			//Return count

			return this->m_vExpressionItems.size();
		}

		std::string ExpressionItemValue(const size_t uiId)
		{
			//Get expression item by id

			std::string szResult("");

			if (uiId < this->m_vExpressionItems.size()) {
				szResult = this->m_vExpressionItems[uiId];
			}

			return szResult;
		}
	};

	class CConfigInt : public CScriptParser {
	private:
		std::string m_szScriptDir; //Full path of script directory
	public:
		CConfigInt() {}
		CConfigInt(const std::string& szScriptDir) : m_szScriptDir(szScriptDir) {}
		CConfigInt(const std::string& szScriptDir, const std::string szScript) : m_szScriptDir(szScriptDir)
		{
			this->Execute(szScript);
		}

		~CConfigInt() {}

		void SetScriptDirectory(const std::string& szScriptDir)
		{
			//Save script directory

			this->m_szScriptDir = szScriptDir;

			if (this->m_szScriptDir[this->m_szScriptDir.length()-1] != CCM_DIR_CHAR)
				this->m_szScriptDir += CCM_DIR_CHAR;
		}

		bool Execute(const std::string& szScriptFile)
		{
			//Execute script file

			std::ifstream hFile(this->m_szScriptDir + szScriptFile); //Open file for reading
			if (hFile.is_open()) { //If file could be opened
				std::string szCurrentLine("");

				while (!hFile.eof()) { //While not at end of file
					std::getline(hFile, szCurrentLine, '\n'); //Read current line
					
					//Ignore empty lines
					if (!szCurrentLine.length())
						continue;

					//Parse current line
					CScriptParser::Parse(szCurrentLine);
				}

				hFile.close(); //Close file

				return true;
			}

			return false;
		}
	};
}
//======================================================================
