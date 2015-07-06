#ifndef _VISCOMPS_H
#define _VISCOMPS_H

#include "includes.h"
#include "czyconfigmgr.h"
#include <inputsystem\buttoncode.h>

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	viscomps.h: GUI menu system interface and implementations
*/

namespace CzyVisualComponents { //Visual components namespace
	#define abstract_class class
	#define VISCOMP_MAX_STRING_LEN 2048

	extern CzyConfigMgr::CConfigInt* nm_pConfigInt;

	enum value_type_e {
		VT_BOOL,
		VT_BYTE,
		VT_SHORT,
		VT_INT,
		VT_INT64,
		VT_FLOAT,
		VT_DOUBLE,
		VT_CHAR,
		VT_STRING
	};

	/*enum {
		CM_KEY_LEFT,
		CM_KEY_RIGHT,
		CM_KEY_UP,
		CM_KEY_DOWN,
		CM_KEY_ENTER,
		CM_KEY_BACKWARD
	};*/

	struct drawinginterface_s {
		BOOL (WINAPI *TpfnPrintText)(LPCSTR lpszMessage);
		BOOL (WINAPI *TpfnDrawText)(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a);
		BOOL (WINAPI *TpfnDrawBox)(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a);
		BOOL (WINAPI *TpfnDrawFilledBox)(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a);
		BOOL (WINAPI *TpfnDrawLine)(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a);
		size_t (WINAPI *TpfnLoadImage)(LPCSTR lpszFileName);
		BOOL (WINAPI *TpfnDrawImage)(const size_t uiImageId, INT x, INT y);
	};

	struct color32_s { //Color struct
		unsigned char r, g, b, a; //Red, green, blue and alpha channel
	};

	struct menu_color_s {
		color32_s borders; //Color of borders
		color32_s closebox; //Color of close box
		color32_s headfill; //Color of head fill
		color32_s bodyfill; //Color of body fill
		color32_s cube; //Cube color
		color32_s title; //Title color
	};

	struct windowinfo_s { //Window information, colors are used as a unite scheme related to a single form
		int x, y; //XY-coordinates
		int w, h; //Width and height
		menu_color_s colors; //Menu colors
		int borderSize; //Size of border
		int fontSizeW; //Width of a font
		int fontSizeH; //Height of a font
		int fontCharDist; //Distance between each font character
		int fontLineDist; //Distance between each font line
		int boxDist; //Box distance
		int cubeWidth; //Cube width
		int cubeHeight; //Cube height
		char szFont[MAX_PATH]; //Used font
	};

	struct component_size_s {
		int a, b;
	};

	typedef void (*TpfnEventFunc)(void); //Event function for specific component events

	abstract_class CBaseComponent { //Base component class
	private:
		char m_szIdentifier[MAX_PATH]; //Base identifier name
		char m_szComponentName[MAX_PATH]; //Name of this component
		bool m_bInitialized; //Indicator whether Initialize() method has been called successfully
	protected:
		drawinginterface_s m_diInterface; //Drawing interface table
		windowinfo_s m_wndInfo; //Window information
		component_size_s m_lsOwnerLocation; //Owner location if any
		component_size_s m_lsLocation; //Location
		component_size_s m_lsResolution; //Width and height
		CzyConfigMgr::CCVar::cvar_s* m_pCVar; //Pointer to cvar data if any
		bool m_bShow; //Indicator whether this component is shown or not
		bool m_bGotFocus; //Indicator whether this component has currently focus
		char m_szText[VISCOMP_MAX_STRING_LEN]; //Text field of the component
		TpfnEventFunc m_pEventFunction; //Affected event function if any
	public:
		CBaseComponent() { memset(&this->m_wndInfo, 0x00, sizeof(windowinfo_s)); memset(this->m_szIdentifier, 0x00, sizeof(this->m_szIdentifier)); this->m_bInitialized = false; this->m_pCVar = NULL; m_bGotFocus = false; this->m_szText[0] = 0; m_pEventFunction = NULL; }
		~CBaseComponent() { }

		virtual bool IsReady(void)
		{
			//Return indicator flag

			return this->m_bInitialized;
		}

		virtual bool IsVisible(void)
		{
			//Return indicator flag

			return this->m_bShow;
		}

		virtual bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
		{
			//Initialize base data

			if ((!lpszIdent) || (!lpszName) || (!pWindowInfo) || (!pDrawingInterface))
				return false;

			//Copy window info
			memcpy(&this->m_wndInfo, pWindowInfo, sizeof(windowinfo_s));

			//Copy component identifier
			strcpy_s(this->m_szIdentifier, lpszIdent);

			//Copy component name
			strcpy_s(this->m_szComponentName, lpszName);

			//Copy drawing interface function table
			memcpy(&this->m_diInterface, pDrawingInterface, sizeof(drawinginterface_s));

			return this->m_bInitialized = true;
		}

		virtual const char* GetIdentifier(void)
		{
			//Get string address of identifier string

			return this->m_szIdentifier;
		}

		virtual const char* GetName(void)
		{
			//Get string address of name string

			return this->m_szComponentName;
		}

		virtual void SetColors(const menu_color_s* pColors)
		{
			//Set new menu colors

			//Check if component is ready
			if (!IsReady())
				return;

			//Copy color data
			memcpy(&this->m_wndInfo.colors, pColors, sizeof(menu_color_s));
		}

		virtual void SetFont(const char* lpszFont)
		{
			//Set font

			//Check if component is ready
			if (!IsReady())
				return;

			//Copy font
			strcpy_s(this->m_wndInfo.szFont, lpszFont);
		}

		virtual bool SetText(const char* lpszText)
		{
			//Set component text

			if (!lpszText)
				return false;

			//Check if component is ready
			if (!IsReady())
				return false;

			//Copy string
			strcpy_s(this->m_szText, lpszText);

			return true;
		}

		virtual const char* GetText(void)
		{
			//Get component text

			//Check if component is ready
			if (!IsReady())
				return NULL;

			return this->m_szText; //Return string address
		}

		virtual bool SetCVar(const char* lpszVarName)
		{
			//Set cvar

			if (!lpszVarName)
				return false;

			//Check if component is ready
			if (!IsReady())
				return false;

			//Get pointer to cvar if exists
			this->m_pCVar = nm_pConfigInt->CCVar::Find(lpszVarName);

			return this->m_pCVar != NULL;
		}

		virtual bool SetEventFunc(const TpfnEventFunc pfnEventFunc)
		{
			//Set event func

			if (!pfnEventFunc)
				return false;

			//Check if component is ready
			if (!IsReady())
				return false;

			//Copy address
			this->m_pEventFunction = pfnEventFunc;

			return true;
		}

		virtual void ResetWindowInfo(const windowinfo_s* pWindowInfo)
		{
			//Reset window info

			//Check if component is ready
			if (!IsReady())
				return;

			//Copy window info
			memcpy(&this->m_wndInfo, pWindowInfo, sizeof(windowinfo_s));
		}
		
		virtual void Free(void)
		{
			//Free allocated data if any and reset flag

			//Check if component is ready
			if (!IsReady())
				return;

			this->m_bInitialized = false;
		}

		//Process data
		virtual void Process(void) = 0;

		void DrawDeepBorder(int x, int y, int w, int h)
		{
			//Draw a deep border

			//Check if component is ready
			if (!IsReady())
				return;

			//Draw border entities
			m_diInterface.TpfnDrawFilledBox(x, y, w + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize / 2, 100, 100, 100, 255); //Box (top outer horizontal line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2, y + (this->m_wndInfo.borderSize / 2), w, this->m_wndInfo.borderSize / 2, 0, 0, 0, 255); //Box (top inner horizontal line)
			m_diInterface.TpfnDrawFilledBox(x, y + this->m_wndInfo.borderSize / 2, this->m_wndInfo.borderSize / 2, h + this->m_wndInfo.borderSize / 2, 100, 100, 100, 255); //Box (left outer vertical line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2, y + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize / 2, h - this->m_wndInfo.borderSize / 2, 0, 0, 0, 255); //Box (left inner vertical line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2, y + h + this->m_wndInfo.borderSize / 2, w, this->m_wndInfo.borderSize / 2, 200, 200, 200, 255); //Box (bottom inner horizontal line)
			m_diInterface.TpfnDrawFilledBox(x, y + h + this->m_wndInfo.borderSize, w + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize / 2, 255, 255, 255, 255); //Box (bottom outer horizontal line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2 + w, y + (this->m_wndInfo.borderSize / 2), this->m_wndInfo.borderSize / 2, h + this->m_wndInfo.borderSize / 2, 200, 200, 200, 255); //Box (right inner vertical line)
			m_diInterface.TpfnDrawFilledBox(x + w + this->m_wndInfo.borderSize, y, this->m_wndInfo.borderSize / 2, h + this->m_wndInfo.borderSize + this->m_wndInfo.borderSize / 2, 255, 255, 255, 255); //Box (right outer vertical line)
		}

		void DrawSalientBorder(int x, int y, int w, int h)
		{
			//Draw a salient border

			//Check if component is ready
			if (!IsReady())
				return;

			//Draw border entities
			m_diInterface.TpfnDrawFilledBox(x, y, w + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize / 2, 255, 255, 255, 255); //Box (top outer horizontal line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2, y + (this->m_wndInfo.borderSize / 2), w, this->m_wndInfo.borderSize / 2, 200, 200, 200, 255); //Box (top inner horizontal line)
			m_diInterface.TpfnDrawFilledBox(x, y + this->m_wndInfo.borderSize / 2, this->m_wndInfo.borderSize / 2, h + this->m_wndInfo.borderSize / 2, 255, 255, 255, 255); //Box (left outer vertical line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2, y + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize / 2, h - this->m_wndInfo.borderSize / 2, 200, 200, 200, 255); //Box (left inner vertical line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2, y + h + this->m_wndInfo.borderSize / 2, w, this->m_wndInfo.borderSize / 2, 0, 0, 0, 255); //Box (bottom inner horizontal line)
			m_diInterface.TpfnDrawFilledBox(x, y + h + this->m_wndInfo.borderSize, w + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize / 2, 100, 100, 100, 255); //Box (bottom outer horizontal line)
			m_diInterface.TpfnDrawFilledBox(x + this->m_wndInfo.borderSize / 2 + w, y + (this->m_wndInfo.borderSize / 2), this->m_wndInfo.borderSize / 2, h + this->m_wndInfo.borderSize / 2, 0, 0, 0, 255); //Box (right inner vertical line)
			m_diInterface.TpfnDrawFilledBox(x + w + this->m_wndInfo.borderSize, y, this->m_wndInfo.borderSize / 2, h + this->m_wndInfo.borderSize + this->m_wndInfo.borderSize / 2, 100, 100, 100, 100); //Box (right outer vertical line)
		}

		//Draw component
		virtual void Draw(void) = 0;

		//Handle key events. This method is called regardless if this component has focus or not
		virtual void KeyEvent(int iKey, bool bDown) = 0;

		//Handle mouse events. This method is called regardless if this component has focus or not
		virtual void MouseEvent(int x, int y, int iMouseKey, bool bDown) = 0;

		//Mouse button event. This method is only called if this component has currently focus
		virtual void OnMouseButton(int iMouseKey, bool bDown) = 0;

		//Mouse move event. This method is only called if this component has currently focus
		virtual void OnMouseMove(int x, int y) = 0;

		//Key button event. This method is only called if this component has currently focus
		virtual void OnKeyButton(int iKey, bool bDown) = 0;

		virtual void OnMove(int x, int y)
		{
			//Movement event called if the owner of this component moves

			m_lsOwnerLocation.a = x;
			m_lsOwnerLocation.b = y;
		}

		virtual void OnFocus(bool bStatus)
		{
			//Event for recieving/losing focus

			this->m_bGotFocus = bStatus;
		}

		virtual bool SetLocation(int x, int y, bool bCheckWithOwner)
		{
			//Set component location
			
			//Check if component is ready
			if (!IsReady())
				return false;
			
			//If desired check if would be drawn out of the area of the owner
			if (bCheckWithOwner) {
				//Horizontal
				if (x + this->m_lsResolution.a > this->m_wndInfo.x + this->m_wndInfo.w)
					return false;
				
				//Vertical
				if (y + this->m_lsResolution.b > this->m_wndInfo.y + this->m_wndInfo.h)
					return false;
			}
			
			//Save location
			this->m_lsLocation.a = x;
			this->m_lsLocation.b = y;

			//Save location to owner info if this is the owner
			if (!bCheckWithOwner) {
				this->m_wndInfo.x = x;
				this->m_wndInfo.y = y;
			}

			return true;
		}

		virtual const component_size_s* GetLocation(void)
		{
			//Get component location

			//Check if component is ready
			if (!IsReady())
				return NULL;

			return &this->m_lsLocation;
		}

		virtual bool SetSize(int w, int h, bool bCheckWithOwner)
		{
			//Set component size

			//Check if component is ready
			if (!IsReady())
				return false;

			//If desired check if would be drawn out of the area of the owner
			if (bCheckWithOwner) {
				//Horizontal
				if (this->m_lsLocation.a + w > this->m_wndInfo.x + this->m_wndInfo.w)
					return false;

				//Vertical
				if (this->m_lsLocation.b + h > this->m_wndInfo.y + this->m_wndInfo.h)
					return false;
			}

			//Save size
			this->m_lsResolution.a = w;
			this->m_lsResolution.b = h;

			//Save size to owner info if this is the owner
			if (!bCheckWithOwner) {
				this->m_wndInfo.w = w;
				this->m_wndInfo.h = h;
			}

			return true;
		}

		virtual const component_size_s* GetSize(void)
		{
			//Get component size

			//Check if component is ready
			if (!IsReady())
				return NULL;

			return &this->m_lsResolution;
		}

		virtual bool InComponentRange(int x, int y)
		{
			//Indicates whether the given position ins in the range of this component

			//Check if component is ready
			if (!IsReady())
				return false;

			return (x >= this->m_lsLocation.a) && (x <= this->m_lsLocation.a + this->m_lsResolution.a) && (y >= this->m_lsLocation.b) && (y <= this->m_lsLocation.b + this->m_lsResolution.b);
		}

		//Returns an indicator whether this component handles a script variable or not
		virtual bool NeedVariable(void) = 0;

		//Returns an indicator whether this component handles an event function
		virtual bool NeedEventFunction(void) = 0;

		virtual bool SetValue(void* pValuePtr, const value_type_e eValueType)
		{
			//Set value of cvar

			if (!pValuePtr)
				return false;

			//Check if component is ready
			if (!IsReady())
				return false;

			//Check if component doesn't handle a variable
			if (!this->NeedVariable())
				return false;

			//Set value according to type
			switch (eValueType) {
				case VT_BOOL: {
					this->m_pCVar->bValue = *(bool*)pValuePtr;
					break;
				}
				case VT_BYTE: {
					this->m_pCVar->iValue = *(int*)pValuePtr;
					break;
				}
				case VT_SHORT: {
					this->m_pCVar->iValue = *(int*)pValuePtr;
					break;
				}
				case VT_INT: {
					this->m_pCVar->iValue = *(int*)pValuePtr;
					break;
				}
				case VT_INT64: {
					return false; //Currently unsupported
					break;
				}
				case VT_FLOAT: {
					this->m_pCVar->fValue = *(float*)pValuePtr;
					break;
				}
				case VT_DOUBLE: {
					return false; //Currently unsupported
					break;
				}
				case VT_CHAR: {
					return false; //Currently unsupported
					break;
				}
				case VT_STRING: {
					strcpy_s(this->m_pCVar->szValue, (char*)pValuePtr);
					break;
				}
				default: {
					return false;
					break;
				}
			}

			return true;
		}

		virtual void Show(void)
		{
			//Show this component

			//Check if component is ready
			if (!IsReady())
				return;

			this->m_bShow = true; //Set to false
		}

		virtual void Hide(void)
		{
			//Hide this component

			//Check if component is ready
			if (!IsReady())
				return;

			this->m_bShow = false; //Set to true
		}

		virtual void ToggleVisibility(void)
		{
			//Toggle visibility of this component

			//Check if component is ready
			if (!IsReady())
				return;

			//Toggle flag
			this->m_bShow = !this->m_bShow;
		}
	};

	#define FORM_IDENT "form"
	#define SHELL_TOP_HEIGHT 31
	#define SHELL_LRB_SIZE 6
	#define SHELL_TOP_CLOSERECT_WIDTH 30
	class CForm : public CBaseComponent { //Main form component class
	private:
		struct form_component_s { //Form components
			abstract_class CBaseComponent* pComponent; //Pointer to component instance
			bool bHasFocus; //Indicator whether this component has current focus
		};

		struct rectangle_s {
			component_size_s pos;
			component_size_s res;
		};

		std::vector<form_component_s> m_vFormComponents; //List of related components

		rectangle_s m_raToggleBox; //Rectangle info of toggle box
		rectangle_s m_raTitleBar; //Rectangle info of title bar

		bool m_bToggleBoxHover; //Whether mouse hovers over the toggle box

		bool m_bFormMove; //If form shall get moved

		void ResetFocus(size_t uiIdentOfFocusedComponent);
	public:
		CForm() : m_bToggleBoxHover(false) { this->m_bFormMove = false; }
		~CForm() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface);
		void Free(void);

		bool IsVisible(void) { return CBaseComponent::IsVisible(); }

		void Process(void);
		void Draw(void);

		void KeyEvent(int iKey, bool bDown);
		void MouseEvent(int x, int y, int iMouseKey, bool bDown);

		bool AddVisualComponent(CBaseComponent* pComponent, const char* lpszAffectedVar, const TpfnEventFunc pfnEventFunc);

		CBaseComponent* GetComponent(const char* szComponentName);

		windowinfo_s* GetWindowInfo(void) { return &this->m_wndInfo; }
		drawinginterface_s* GetDrawingInterface(void) { return &this->m_diInterface; }

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, false); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return false; } //Form doesn't need a variable
		bool NeedEventFunction(void) { return false; } //Form doesn't need an alias

		void Show(void) { CBaseComponent::Show(); }
		void Hide(void) { CBaseComponent::Hide(); }
		void ToggleVisibility(void) { CBaseComponent::ToggleVisibility(); }

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void ResetWindowInfo(const windowinfo_s* pWindowInfo)
		{ 
			//Reset window information

			if (!pWindowInfo)
				return;

			CBaseComponent::ResetWindowInfo(pWindowInfo); 

			//Save rectangle info
			SetRectangleInfo(pWindowInfo);
		}

		void SetRectangleInfo(const windowinfo_s* pWindowInfo)
		{
			//Set rectangle info

			if (!pWindowInfo)
				return;

			this->m_raTitleBar.pos.a = this->m_wndInfo.x + this->m_wndInfo.borderSize;
			this->m_raTitleBar.pos.b = this->m_wndInfo.y + this->m_wndInfo.borderSize;
			this->m_raTitleBar.res.a = this->m_wndInfo.w - this->m_wndInfo.borderSize - (this->m_wndInfo.borderSize * 2 + this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist);
			this->m_raTitleBar.res.b = this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist * 2;

			this->m_raToggleBox.pos.a = this->m_wndInfo.x + this->m_wndInfo.w - (SHELL_TOP_CLOSERECT_WIDTH + SHELL_LRB_SIZE + SHELL_LRB_SIZE + this->m_wndInfo.borderSize);
			this->m_raToggleBox.pos.b = this->m_wndInfo.y + 1;
			this->m_raToggleBox.res.a = SHELL_TOP_CLOSERECT_WIDTH;
			this->m_raToggleBox.res.b = SHELL_TOP_HEIGHT - SHELL_LRB_SIZE;
		}

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown) {} //Unused
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) {} //Unused
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return false; } //The form doesn't need a script variable
		bool SetEventFunc(const char* lpszAliasName) { return false; } //The form doesn't need an event alias

		const char* SelectedTextInput(void);
	};

	#define LABEL_IDENT "label"
	class CLabel : public CBaseComponent { //Label component class
	private:
	public:
		CLabel() { }
		~CLabel() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface);
		void Free(void);

		void Process(void) {} //Unused
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown) {} //Unused

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return true; } //Read-only
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return false; } //Label doesn't need a variable
		bool NeedEventFunction(void) { return false; } //Label doesn't need an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown) {} //Unused
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return false; } //The label doesn't need a script variable
		bool SetEventFunc(const char* lpszAliasName) { return false; } //The label doesn't need an event alias
	};

	#define BUTTON_IDENT "button"
	class CButton : public CBaseComponent { //Button component class
	private:
		int m_iTextOffset; //Text offset related to button mouse key press/release
		bool m_bHover; //Indicator whether the mouse is above this component
		bool m_bMouseDown; //Indicator wheter the mouse is held down on the button
	public:
		CButton() { this->m_iTextOffset = 0; this->m_bHover = false; this->m_bMouseDown = false; }
		~CButton() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface);
		void Free(void);

		void Process(void) {} //Unused
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown);

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, false); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return false; } //Button doesn't need a variable
		bool NeedEventFunction(void) { return true; } //Button needs an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown);
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return false; } //The button doesn't need a script variable
	};

	#define CHECKBOX_IDENT "checkbox"
	class CCheckbox : public CBaseComponent { //Checkbox component class
	private:
		bool m_bValue; //Current value
		bool m_bHover; //Indicator whether the mouse is above this component
	public:
		CCheckbox() { this->m_bHover = false; }
		~CCheckbox() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface);
		void Free(void);

		void Process(void) {} //Unused
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown);

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, false); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr);

		bool NeedVariable(void) { return true; } //Checkbox needs a variable
		bool NeedEventFunction(void) { return false; } //Checkbox doesn't need an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown);
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return CBaseComponent::SetCVar(lpszVarName); }
		bool SetEventFunc(const char* lpszAliasName) { return false; }
	};

	#define TEXTBOX_IDENT "textbox"
	#define TEXTBOX_DEFAULT_LENGTH 35
	#define TEXTBOX_DELAY 597
	class CTextbox : public CBaseComponent { //Textbox component class
	private:
		char m_szString[VISCOMP_MAX_STRING_LEN]; //Current string
		unsigned int m_uiTextLen; //Text length of input text, width is calculated according to it
		DWORD m_dwTimerCur, m_dwTimerInit; //Timer members
		bool m_bDrawUnderline; //Indicator whether to draw underline
		bool m_bHover; //Indicator whether the mouse is above this component
	public:
		CTextbox() { this->m_uiTextLen = TEXTBOX_DEFAULT_LENGTH; this->m_bDrawUnderline = false; this->m_bHover = false; }
		~CTextbox() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface);
		void Free(void);

		void Process(void);
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown);

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, true); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return true; } //Textbox needs a variable
		bool NeedEventFunction(void) { return false; } //Textbox doesn't need an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown) {} //Unused
		void OnKeyButton(int iKey, bool bDown);
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus);

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return CBaseComponent::SetCVar(lpszVarName); }
		bool SetEventFunc(const char* lpszAliasName) { return false; }

		void SetTextLen(unsigned int uiTextLen) { this->m_uiTextLen = uiTextLen; }
		unsigned int GetTextLen(void) { return this->m_uiTextLen; }

		void UpdateText(void);
	};

	#define GROUPBOX_IDENT "groupbox"
	#define GROUPBOX_LINETOTEXTLEN 20
	class CGroupbox : public CBaseComponent { //Groupbox component class
	public:
		CGroupbox() { }
		~CGroupbox() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface) { return CBaseComponent::Initialize(lpszIdent, lpszName, pWindowInfo, pDrawingInterface); } //Unused
		void Free(void) { CBaseComponent::Free(); } //Unused

		void Process(void) {} //Unused
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown) {} //Unused

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, false); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return false; } //Groupbox doesn't need a variable
		bool NeedEventFunction(void) { return false; } //Groupbox doesn't need an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown) {} //Unused
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return false; } //The groupbox doesn't need a script variable
		bool SetEventFunc(const char* lpszAliasName) { return false; } //The groupbox doesn't need a script alias
	};

	#define LISTBOX_IDENT "listbox"
	#define LISTBOX_INVALID_ITEM_ID std::string::npos
	#define LISTBOX_NAVIGATOR_WIDTH 20
	class CListbox : public CBaseComponent { //Listbox component class
		struct lb_item_s {
			std::string szExpression;
			color32_s clOverriddenColor;
		};
	private:
		std::vector<lb_item_s> m_vItems; //Item list
		size_t m_uiSelectedItem; //ID of current selected item if any
		size_t m_uiDrawingIndex; //Drawing index of items
		int m_iCurMousePosX, m_iCurMousePosY; //Current mouse position

		size_t FindItemByCoord(int x, int y);
	public:
		CListbox() : m_uiSelectedItem(LISTBOX_INVALID_ITEM_ID), m_iCurMousePosX(-1), m_iCurMousePosY(-1), m_uiDrawingIndex(0) { }
		~CListbox() { this->m_vItems.clear(); }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface) { return CBaseComponent::Initialize(lpszIdent, lpszName, pWindowInfo, pDrawingInterface); }
		void Free(void) { CBaseComponent::Free(); }

		void Process(void) {} //Unused
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown);

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, false); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return false; } //Listbox doesn't need a variable
		bool NeedEventFunction(void) { return false; } //Listbox doesn't need an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y);
		void OnMouseButton(int iMouseKey, bool bDown);
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return false; } //The listbox doesn't need a script variable
		bool SetEventFunc(const char* lpszAliasName) { return false; } //The listbox doesn't need a script alias

		size_t ItemCount(void);
		const char* GetSelectedItem(void);
		const char* GetItem(const size_t uiItemId);
		const size_t FindItem(const char* szExpression);
		bool AddItem(const char* szExpression);
		bool AddItem(const char* szExpression, const color32_s* pColor);
		bool DeleteItem(const size_t uiItemId);
		bool AlterItem(const size_t uiItemId, const char* szNewExpression);
		bool AlterItem(const size_t uiItemId, const color32_s* pColor);
		void ClearItems(void);

		void ScrollToEnd(void);
	};

	#define IMAGEBOX_IDENT "imagebox"
	#define IMAGEBOX_INVALID_IMAGE_ID std::string::npos
	class CImagebox : public CBaseComponent { //Imagebox component class
	private:
		size_t m_uiImageId;
	public:
		CImagebox() : m_uiImageId(IMAGEBOX_INVALID_IMAGE_ID) { }
		~CImagebox() { }

		bool Initialize(LPCSTR lpszIdent, LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface) { return CBaseComponent::Initialize(lpszIdent, lpszName, pWindowInfo, pDrawingInterface); }
		void Free(void) { CBaseComponent::Free(); }

		void Process(void) {} //Unused
		void Draw(void);

		void KeyEvent(int iKey, bool bDown) {} //Unused
		void MouseEvent(int x, int y, int iMouseKey, bool bDown) {} //Unused

		bool SetLocation(int x, int y) { return CBaseComponent::SetLocation(x, y, false); }
		const component_size_s* GetLocation(void) { return CBaseComponent::GetLocation(); }
		bool SetSize(int w, int h) { return CBaseComponent::SetSize(w, h, false); }
		const component_size_s* GetSize(void) { return CBaseComponent::GetSize(); }

		bool InComponentRange(int x, int y) { return CBaseComponent::InComponentRange(x, y); }

		bool SetValue(void* pValuePtr) { return true; } //Unused

		bool NeedVariable(void) { return false; } //Listbox doesn't need a variable
		bool NeedEventFunction(void) { return false; } //Listbox doesn't need an alias

		void Show(void) {} //Unused
		void Hide(void) {} //Unused
		void ToggleVisibility(void) {} //Unused

		void SetFont(const char* lpszFont) { CBaseComponent::SetFont(lpszFont); }

		void OnMouseMove(int x, int y) {} //Unused
		void OnMouseButton(int iMouseKey, bool bDown) {} //Unused
		void OnKeyButton(int iKey, bool bDown) {} //Unused
		void OnMove(int x, int y) { CBaseComponent::OnMove(x, y); }
		void OnFocus(bool bStatus) { CBaseComponent::OnFocus(bStatus); }

		bool SetText(const char* lpszText) { return CBaseComponent::SetText(lpszText); }
		const char* GetText(void) { return CBaseComponent::GetText(); }
		bool SetCVar(const char* lpszVarName) { return false; } //The listbox doesn't need a script variable
		bool SetEventFunc(const char* lpszAliasName) { return false; } //The listbox doesn't need a script alias

		void SetImage(const size_t uiImageId) { this->m_uiImageId = uiImageId; }
	};

	bool SetConfigInt(const CzyConfigMgr::CConfigInt* pConfigInt);
	CForm* CreateForm(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface);
	bool AttachLabel(CForm* pForm, LPCSTR lpszName, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, const char* lpszText);
	bool AttachButton(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pTextColor, color32_s* pBoxColor, color32_s* pHoverColor, const char* lpszText, const char* lpszAffectedAlias);
	bool AttachCheckbox(CForm* pForm, LPCSTR lpszName, int x, int y, color32_s* pTextColor, color32_s* pBoxColor, color32_s* pHoverColor, const char* lpszText, const char* lpszAffectedVar, bool bInitialValue);
	bool AttachTextBox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pTextColor, color32_s* pBoxColor, color32_s* pHoverColor, const char* lpszText, const char* lpszAffectedVar, unsigned int uiTextLen);
	bool AttachGroupbox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pTextColor, color32_s* pFrameColor, const char* lpszText);
	bool AttachListbox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pItemColor, color32_s* pBackgroundColor, color32_s* pSelectedItemColor);
	bool AttachImagebox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, const size_t uiImageId);
	void FreeForm(CForm* pForm);
};
#endif
