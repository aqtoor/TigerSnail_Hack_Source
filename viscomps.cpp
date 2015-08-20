#include "viscomps.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.5
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	viscomps.cpp: GUI menu system implementations and declarations
*/

namespace CzyVisualComponents {

	/* CForm methods */

	bool CForm::Initialize(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize base data

		//Check if component is ready
		if (this->IsReady())
			return true;

		//Call base component Initialize() method
		if (!CBaseComponent::Initialize(CZYVC_FORM_IDENT, lpszName, pWindowInfo, pDrawingInterface))
			return false;

		//Save rectangle info
		SetRectangleInfo(pWindowInfo);

		return true;
	}

	void CForm::Free(void)
	{
		//Free data if any

		//Check if component is ready
		if (!this->IsReady())
			return;

		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) { //Loop through components list
			if (this->m_vFormComponents[i].pComponent) {
				this->m_vFormComponents[i].pComponent->Free(); //Call Free() method
				delete this->m_vFormComponents[i].pComponent; //Free memory of component
			}
		}

		//Clear list
		this->m_vFormComponents.clear();

		//Call base component Free() method
		CBaseComponent::Free();
	}

	void CForm::Process(void)
	{
		//Process form stuff

		//Check if component is ready
		if (!this->IsReady())
			return;
		
		//Signal attached components
		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) { //Loop through list
			if (this->m_vFormComponents[i].pComponent) //Check pointer
				this->m_vFormComponents[i].pComponent->Process(); //Call Process() method of component
		}
	}

	void CForm::Draw(void)
	{
		//Draw form

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Check if form is hidden
		if (!this->m_bShow)
			return;
		
		#define DRAW_TEXT this->m_diInterface.TpfnDrawText
		#define DRAW_BOX this->m_diInterface.TpfnDrawBox
		#define DRAW_AREA this->m_diInterface.TpfnDrawFilledBox
		#define DRAW_LINE this->m_diInterface.TpfnDrawLine
		#define BORDER_SIZE this->m_wndInfo.borderSize
		#define TEXT_TO_BORDER_DIST this->m_wndInfo.textToBorderDist
		
		//Draw borders and the areas
		//DRAW_BOX(this->m_wndInfo.x, this->m_wndInfo.y, this->m_wndInfo.w, this->m_wndInfo.borderSize * 2 + this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist * 2, this->m_wndInfo.borderSize, this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a); //Title box
		//DRAW_BOX(this->m_wndInfo.x, this->m_wndInfo.y + this->m_wndInfo.borderSize * 2 + this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist * 2 + this->m_wndInfo.boxDist, this->m_wndInfo.w, this->m_wndInfo.h - (this->m_wndInfo.borderSize * 2 + this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist * 2 + this->m_wndInfo.boxDist), this->m_wndInfo.borderSize, this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a); //Body box
		DRAW_AREA(this->m_wndInfo.x - (CZYVC_SHELL_LRB_SIZE + this->m_wndInfo.borderSize), this->m_wndInfo.y, this->m_wndInfo.w, CZYVC_SHELL_TOP_HEIGHT, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a); //Shell top
		DRAW_AREA(this->m_wndInfo.x - (CZYVC_SHELL_LRB_SIZE + this->m_wndInfo.borderSize), this->m_wndInfo.y, CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.h, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a); //Shell left
		DRAW_AREA(this->m_wndInfo.x - (CZYVC_SHELL_LRB_SIZE + this->m_wndInfo.borderSize) + this->m_wndInfo.w - CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.y, CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.h, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a); //Shell right
		DRAW_AREA(this->m_wndInfo.x - (CZYVC_SHELL_LRB_SIZE + this->m_wndInfo.borderSize), this->m_wndInfo.y + this->m_wndInfo.h - CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.w, CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a); //Shell bottom
		DRAW_AREA(this->m_wndInfo.x - (CZYVC_SHELL_LRB_SIZE + this->m_wndInfo.borderSize) + CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.y + CZYVC_SHELL_TOP_HEIGHT, this->m_wndInfo.w - CZYVC_SHELL_LRB_SIZE * 2, this->m_wndInfo.h - CZYVC_SHELL_TOP_HEIGHT - CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.colors.bodyfill.r, this->m_wndInfo.colors.bodyfill.g, this->m_wndInfo.colors.bodyfill.b, this->m_wndInfo.colors.bodyfill.a); //Body fill
		
		//Draw show/hide toggle box
		//DRAW_BOX(this->m_raToggleBox.pos.a, this->m_raToggleBox.pos.b, this->m_raToggleBox.res.a, this->m_raToggleBox.res.b, this->m_wndInfo.borderSize, this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a);
		//Draw show/hide toggle cross
		//DRAW_LINE(this->m_raToggleBox.pos.a, this->m_raToggleBox.pos.b, this->m_raToggleBox.pos.a + this->m_raToggleBox.res.a, this->m_raToggleBox.pos.b + this->m_raToggleBox.res.b, this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a);
		//DRAW_LINE(this->m_raToggleBox.pos.a, this->m_raToggleBox.pos.b + this->m_raToggleBox.res.b, this->m_raToggleBox.pos.a + this->m_raToggleBox.res.a, this->m_raToggleBox.pos.b, this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a);
		DRAW_AREA(this->m_raToggleBox.pos.a, this->m_raToggleBox.pos.b, this->m_raToggleBox.res.a, this->m_raToggleBox.res.b, this->m_wndInfo.colors.closebox.r, this->m_wndInfo.colors.closebox.g, this->m_wndInfo.colors.closebox.b, (this->m_bToggleBoxHover) ? this->m_wndInfo.colors.closebox.a - 125: this->m_wndInfo.colors.closebox.a);
		DRAW_TEXT("X", this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_raToggleBox.pos.a + (this->m_raToggleBox.res.a / 2 - this->m_wndInfo.fontSizeW / 2), this->m_raToggleBox.pos.b + (this->m_raToggleBox.res.b / 2 -  this->m_wndInfo.fontSizeH / 2), this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a);

		#define CUBE_WIDTH this->m_wndInfo.cubeWidth
		#define CUBE_HEIGHT this->m_wndInfo.cubeHeight

		//Draw title cube
		//DRAW_AREA(this->m_wndInfo.x + CUBE_WIDTH / 2, this->m_wndInfo.y + this->m_wndInfo.borderSize + this->m_wndInfo.fontLineDist + ((this->m_wndInfo.fontSizeH + BORDER_SIZE * 2) / 2) - (CUBE_HEIGHT / 2), CUBE_WIDTH, CUBE_HEIGHT, this->m_wndInfo.colors.cube.r, this->m_wndInfo.colors.cube.g, this->m_wndInfo.colors.cube.b, this->m_wndInfo.colors.cube.a);

		//Draw box title
		DRAW_TEXT(this->m_szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_wndInfo.x - (CZYVC_SHELL_LRB_SIZE + this->m_wndInfo.borderSize) + CZYVC_SHELL_LRB_SIZE, this->m_wndInfo.y + (CZYVC_SHELL_TOP_HEIGHT / 2 - this->m_wndInfo.fontSizeH / 2) + 1, this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
		
		//Signal attached components
		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) { //Loop through list
			if (this->m_vFormComponents[i].pComponent)
				this->m_vFormComponents[i].pComponent->Draw(); //Call Draw() method of component
		}
	}

	void CForm::KeyEvent(int iKey, bool bDown)
	{
		//Handle key events

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Check if form is hidden
		if (!this->m_bShow)
			return;
		
		//Signal attached components
		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) { //Loop through list
			if (this->m_vFormComponents[i].pComponent) { //Check pointer
				this->m_vFormComponents[i].pComponent->KeyEvent(iKey, bDown); //Call KeyEvent() method of component

				if (this->m_vFormComponents[i].bHasFocus) { //Check if this component has focus
					this->m_vFormComponents[i].pComponent->OnKeyButton(iKey, bDown); //Call OnKeyButton() event method of this component
				}
			}
		}
	}

	void CForm::MouseEvent(int x, int y, int iMouseKey, bool bDown)
	{
		//Handle mouse events

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Check if form is hidden
		if (!this->m_bShow)
			return;
		
		if (iMouseKey != -1) { //Check if a mouse key event shall get handled
			if (!bDown) { //Check if the mouse key got released
				//Clear indicator if not already
				if (this->m_bFormMove)
					this->m_bFormMove = false;

				if ((x >= this->m_raToggleBox.pos.a) && (x <= this->m_raToggleBox.pos.a + this->m_raToggleBox.res.a) && (y >= this->m_raToggleBox.pos.b) && (y <= this->m_raToggleBox.pos.b + this->m_raToggleBox.res.b)) { //Check if inside toggle-button range
					this->ToggleVisibility(); //Toggle visibility
				}
			} else { //Mouse key is held down
				if ((x >= this->m_raTitleBar.pos.a) && (x <= this->m_raTitleBar.pos.a + this->m_raTitleBar.res.a) && (y >= this->m_raTitleBar.pos.b) && (y <= this->m_raTitleBar.pos.b + this->m_raTitleBar.res.b)) { //Check if inside title bar range
					//Set indicator that form shall be moved if not already
					if (!this->m_bFormMove)
						this->m_bFormMove = true;
				}
			}
		} else { //Mouse move event shall get handled
			if ((x >= this->m_raToggleBox.pos.a) && (x <= this->m_raToggleBox.pos.a + this->m_raToggleBox.res.a) && (y >= this->m_raToggleBox.pos.b) && (y <= this->m_raToggleBox.pos.b + this->m_raToggleBox.res.b)) { //Check if inside toggle-button range
				this->m_bToggleBoxHover = true;
			} else {
				this->m_bToggleBoxHover = false;
			}

			if (this->m_bFormMove) { //If form move event is triggered
				this->SetLocation(x, y); //Reset location

				//Inform attached components if any
				for (size_t i = 0; i < this->m_vFormComponents.size(); i++) {
					if (this->m_vFormComponents[i].pComponent) {
						this->m_vFormComponents[i].pComponent->OnMove(x, y);
					}
				}

				//Reset rectangle info
				SetRectangleInfo(&this->m_wndInfo);
			}
		}

		bool bClickedOnAttachedComponent = false;

		//Signal attached components
		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) { //Loop through list
			if (this->m_vFormComponents[i].pComponent) {
				this->m_vFormComponents[i].pComponent->MouseEvent(x, y, iMouseKey, bDown); //Call MouseEvent() method of component

				const component_size_s* pLocation = this->m_vFormComponents[i].pComponent->GetLocation(); //Get pointer to location data
				if (pLocation) {
					const component_size_s* pSize = this->m_vFormComponents[i].pComponent->GetSize(); //Get pointer to size data
					if (pSize) {
						if ((x >= this->m_lsLocation.a + pLocation->a) && (x <= this->m_lsLocation.a + pLocation->a + pSize->a) && (y >= this->m_lsLocation.b + pLocation->b) && (y <= this->m_lsLocation.b + pLocation->b + pSize->b)) { //If mouse position is in range of this component
							if (iMouseKey != -1) { //Key shall be handled
								if (!this->m_vFormComponents[i].bHasFocus) { //This component didn't have focus previously
									ResetFocus(i); //Reset focus

									this->m_vFormComponents[i].bHasFocus = true; //Set indicator value to true, this component has now focus

									this->m_vFormComponents[i].pComponent->OnFocus(true); //Signal component that it has now focus
								}

								if (!bDown) {
									bClickedOnAttachedComponent = true; //User has clicked one of the attached components
								}

								this->m_vFormComponents[i].pComponent->OnMouseButton(iMouseKey, bDown); //Call OnMouseButton() event method of component
							} else { //Movement shall be handled
								if (this->m_vFormComponents[i].bHasFocus) { //Check if component has focus
									this->m_vFormComponents[i].pComponent->OnMouseMove(x, y); //Call OnMouseMove() event method of component
								}
							}
						}
					}
				}
			}
		}

		//Reset any focus if the user has clicked onto the form but didn't hit any component
		if ((iMouseKey != -1) && (!bDown) && (!bClickedOnAttachedComponent) && (x >= this->m_wndInfo.x) && (x <= this->m_wndInfo.x + this->m_wndInfo.w) && (y >= this->m_wndInfo.y) && (y <= this->m_wndInfo.y + this->m_wndInfo.h))
			ResetFocus(-1);
	}

	bool CForm::AddVisualComponent(CBaseComponent* pComponent, const char* lpszAffectedVar, const TpfnEventFunc pfnEventFunc)
	{
		//Add allocated component to form

		//Check if component is ready
		if (!this->IsReady())
			return false;

		if (!pComponent)
			return false;

		//Check if component is ready
		if (!pComponent->IsReady())
			return false;

		//Setup data
		form_component_s fcComponent;
		fcComponent.bHasFocus = false;
		fcComponent.pComponent = pComponent;

		//Set script variable if needed
		if ((fcComponent.pComponent->NeedVariable()) && (!fcComponent.pComponent->SetCVar(lpszAffectedVar)))
			return false;

		//Set event function if needed
		if ((fcComponent.pComponent->NeedEventFunction()) && (!fcComponent.pComponent->SetEventFunc(pfnEventFunc)))
			return false;

		//Add to list
		this->m_vFormComponents.push_back(fcComponent);

		return true;
	}

	CBaseComponent* CForm::GetComponent(const char* szComponentName)
	{
		//Get component instance pointer by instance name

		if ((!szComponentName) || (!szComponentName[0]))
			return NULL;

		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) {
			if ((this->m_vFormComponents[i].pComponent) && (strcmp(this->m_vFormComponents[i].pComponent->GetName(), szComponentName)==0)) {
				return this->m_vFormComponents[i].pComponent;
			}
		}

		return NULL;
	}

	void CForm::ResetFocus(size_t uiIdentOfFocusedComponent)
	{
		//Reset focus status

		//Check if component is ready
		if (!this->IsReady())
			return;

		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) { //Loop through components
			if (i != uiIdentOfFocusedComponent) { //If this component is not the new focused one
				this->m_vFormComponents[i].pComponent->OnFocus(false); //Signal component that it has lost the focus
				this->m_vFormComponents[i].bHasFocus = false; //Set focus indicator value to false
			}
		}
	}

	const char* CForm::SelectedTextInput(void)
	{
		//If text input is focused this method returns the string address to its value otherwise it returns NULL.

		for (size_t i = 0; i < this->m_vFormComponents.size(); i++) {
			if ((strcmp(this->m_vFormComponents[i].pComponent->GetIdentifier(), CZYVC_TEXTBOX_IDENT) == 0) && (this->m_vFormComponents[i].bHasFocus)) {
				return this->m_vFormComponents[i].pComponent->GetText();
			}
		}

		return NULL;
	}


	/* CLabel methods */

	bool CLabel::Initialize(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize component

		//Check if component is ready
		if (this->IsReady())
			return true;

		//Initialize base data
		if (!CBaseComponent::Initialize(CZYVC_LABEL_IDENT, lpszName, pWindowInfo, pDrawingInterface))
			return false;

		//Reset text
		this->m_szText[0] = 0;

		return true;
	}

	void CLabel::Free(void)
	{
		//Free data

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Call base component Free() method
		CBaseComponent::Free();
	}

	void CLabel::Draw(void)
	{
		//Draw component

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Draw the text if not empty
		if (this->m_szText[0])
			this->m_diInterface.TpfnDrawText(this->m_szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
	}

	/* CButton methods */

	bool CButton::Initialize(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize component

		//Check if component is ready
		if (this->IsReady())
			return true;

		//Initialize base data
		if (!CBaseComponent::Initialize(CZYVC_BUTTON_IDENT, lpszName, pWindowInfo, pDrawingInterface))
			return false;

		//Reset strings
		this->m_szText[0] = 0;

		return true;
	}
	
	void CButton::Free(void)
	{
		//Free data

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Call base component Free() method
		CBaseComponent::Free();
	}

	void CButton::Draw(void)
	{
		//Draw button
		
		//Check if component is ready
		if (!this->IsReady())
			return;
		
		//Draw box
		
		if (!this->m_bMouseDown)
			DrawSalientBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_lsResolution.a, this->m_lsResolution.b);
		else
			DrawDeepBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_lsResolution.a, this->m_lsResolution.b);

		DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_lsResolution.a - this->m_wndInfo.borderSize / 2, this->m_lsResolution.b - this->m_wndInfo.borderSize / 2, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.r : this->m_wndInfo.colors.headfill.r, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.g : this->m_wndInfo.colors.headfill.g, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.b : this->m_wndInfo.colors.headfill.b, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.a : this->m_wndInfo.colors.headfill.a); //Fill

		//Draw text
		DRAW_TEXT(this->m_szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + (this->m_lsResolution.a / 2 - ((int)strlen(this->m_szText) * this->m_wndInfo.fontSizeW) / 2) + this->m_wndInfo.borderSize * 2 - this->m_iTextOffset, this->m_lsOwnerLocation.b + this->m_lsLocation.b + (this->m_lsResolution.b / 2 - this->m_wndInfo.fontSizeH / 2) + this->m_wndInfo.borderSize / 2 - this->m_iTextOffset, this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
	}

	void CButton::MouseEvent(int x, int y, int iMouseKey, bool bDown)
	{
		//Handle mouse events

		//Check if component is ready
		if (!this->IsReady())
			return;

		if (iMouseKey == -1) { //If move event occured
			if ((x >= this->m_lsOwnerLocation.a + this->m_lsLocation.a) && (x <= this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a) && (y >= this->m_lsOwnerLocation.b + this->m_lsLocation.b) && (y <= this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_lsResolution.b)) { //If mouse is in range of the button
				if (!this->m_bHover)
					this->m_bHover = true;
			} else {
				if (this->m_bHover)
					this->m_bHover = false;
			}
		}
	}

	void CButton::OnMouseButton(int iMouseKey, bool bDown)
	{
		//Handle mouse button event

		//Check if component is ready
		if (!this->IsReady())
			return;

		if (!bDown) { //If key is released
			//Call event function
			this->m_pEventFunction();

			//Update indicator
			this->m_bMouseDown = false;

			//Clear text offset
			m_iTextOffset = 0;
		} else { //Key is held down
			this->m_bMouseDown = true; //Update indicator
			m_iTextOffset = 1; //Set text offset to n pixels
		}
	}

	/* CCheckbox methods */

	bool CCheckbox::Initialize(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize component

		//Check if component is ready
		if (this->IsReady())
			return true;

		//Initialize base data
		if (!CBaseComponent::Initialize(CZYVC_CHECKBOX_IDENT, lpszName, pWindowInfo, pDrawingInterface))
			return false;

		//Reset text
		this->m_szText[0] = 0;

		//Clear value
		this->m_bValue = false;

		return true;
	}

	bool CCheckbox::SetValue(void* pValuePtr)
	{
		//Set cvar value

		if (CBaseComponent::SetValue(pValuePtr, VT_BOOL)) {
			this->m_bValue = this->m_pCVar->bValue;
			return true;
		}

		return false;
	}

	void CCheckbox::Free(void)
	{
		//Free data

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Call base component Free() method
		CBaseComponent::Free();
	}

	void CCheckbox::Draw(void)
	{
		//Draw component

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Draw box
		DRAW_BOX(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_wndInfo.fontSizeH, this->m_wndInfo.fontSizeH, this->m_wndInfo.borderSize, this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a); //Box
		DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_wndInfo.fontSizeH - this->m_wndInfo.borderSize, this->m_wndInfo.fontSizeH - this->m_wndInfo.borderSize, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.r : this->m_wndInfo.colors.headfill.r, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.g : this->m_wndInfo.colors.headfill.g, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.b : this->m_wndInfo.colors.headfill.b, (this->m_bHover) ? this->m_wndInfo.colors.bodyfill.a : this->m_wndInfo.colors.headfill.a); //Fill

		//Draw check value if set
		if (this->m_bValue) {
			DRAW_TEXT("X", this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + (this->m_wndInfo.fontSizeH / 2 - this->m_wndInfo.fontSizeW / 2), this->m_lsOwnerLocation.b + this->m_lsLocation.b + (this->m_wndInfo.fontSizeH / 2 - this->m_wndInfo.fontSizeH / 2), this->m_wndInfo.colors.borders.r, this->m_wndInfo.colors.borders.g, this->m_wndInfo.colors.borders.b, this->m_wndInfo.colors.borders.a);
		}

		//Draw text
		DRAW_TEXT(this->m_szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist, this->m_lsOwnerLocation.b + this->m_lsLocation.b + (this->m_wndInfo.fontSizeH / 2 - this->m_wndInfo.fontSizeH / 2), this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
	}

	void CCheckbox::MouseEvent(int x, int y, int iMouseKey, bool bDown)
	{
		//Handle mouse events

		//Check if component is ready
		if (!this->IsReady())
			return;

		if (iMouseKey == -1) { //If move event occured
			if ((x >= this->m_lsOwnerLocation.a + this->m_lsLocation.a) && (x <= this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a) && (y >= this->m_lsOwnerLocation.b + this->m_lsLocation.b) && (y <= this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_lsResolution.b)) { //If mouse is in range of the checkbox
				if (!this->m_bHover)
					this->m_bHover = true;
			} else {
				if (this->m_bHover)
					this->m_bHover = false;
			}
		}
	}

	void CCheckbox::OnMouseButton(int iMouseKey, bool bDown)
	{
		//Handle mouse button events

		//Check if component is ready
		if (!this->IsReady())
			return;
		
		if (!bDown) { //If key is released
			//Toggle value
			this->m_bValue = !this->m_bValue;

			//Assign new value
			this->m_pCVar->bValue = this->m_bValue;
		}
	}

	/* CTextbox methods */

	bool CTextbox::Initialize(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize component

		//Check if component is ready
		if (this->IsReady())
			return true;

		//Initialize base data
		if (!CBaseComponent::Initialize(CZYVC_TEXTBOX_IDENT, lpszName, pWindowInfo, pDrawingInterface))
			return false;

		//Reset text
		this->m_szText[0] = 0;

		return true;
	}

	void CTextbox::Free(void)
	{
		//Free data

		//Check if component is ready
		if (!this->IsReady())
			return;

		//Call base component Free() method
		CBaseComponent::Free();
	}

	void CTextbox::Process()
	{
		//Process stuff

		//Update timers if gained focus
		if (this->m_bGotFocus) {
			this->m_dwTimerCur = GetTickCount(); //Get current elapsed milliseconds
			if (this->m_dwTimerCur > this->m_dwTimerInit + CZYVC_TEXTBOX_DELAY) { //Check if time exceeded
				this->m_dwTimerInit = GetTickCount(); //Refresh initial timer

				this->m_bDrawUnderline = !this->m_bDrawUnderline; //Toggle indicator
			}
		}

		//Update component width
		this->m_lsResolution.a = ((this->m_uiTextLen + 1) * (this->m_wndInfo.fontSizeW + this->m_wndInfo.fontCharDist)) + this->m_wndInfo.borderSize * 2 + this->m_wndInfo.boxDist * 2;
		this->m_lsResolution.b = this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist + this->m_wndInfo.borderSize;
	}

	void CTextbox::Draw(void)
	{
		//Draw component

		//Draw box
		DrawDeepBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, ((this->m_uiTextLen + 1) * (this->m_wndInfo.fontSizeW + this->m_wndInfo.fontCharDist)) + this->m_wndInfo.boxDist * 2 + this->m_wndInfo.borderSize, this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist);
		DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, ((this->m_uiTextLen + 1) * (this->m_wndInfo.fontSizeW + this->m_wndInfo.fontCharDist)) + this->m_wndInfo.boxDist * 2 + this->m_wndInfo.borderSize / 2, this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist - this->m_wndInfo.borderSize / 2, (this->m_bHover || this->m_bGotFocus) ? this->m_wndInfo.colors.bodyfill.r : this->m_wndInfo.colors.headfill.r, (this->m_bHover || this->m_bGotFocus) ? this->m_wndInfo.colors.bodyfill.g : this->m_wndInfo.colors.headfill.g, (this->m_bHover || this->m_bGotFocus) ? this->m_wndInfo.colors.bodyfill.b : this->m_wndInfo.colors.headfill.b, (this->m_bHover || this->m_bGotFocus) ? this->m_wndInfo.colors.bodyfill.a : this->m_wndInfo.colors.headfill.a); //Fill

		//Draw text
		if ((this->m_bGotFocus) && (this->m_bDrawUnderline)) {
			char szText[CZYVC_MAX_STRING_LEN];
			sprintf_s(szText, "%s_", this->m_szText);
			DRAW_TEXT(szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.boxDist + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + (this->m_wndInfo.fontSizeH / 2) - (this->m_wndInfo.borderSize + this->m_wndInfo.borderSize / 2), this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
		} else {
			DRAW_TEXT(this->m_szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.boxDist + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + (this->m_wndInfo.fontSizeH / 2) - (this->m_wndInfo.borderSize + this->m_wndInfo.borderSize / 2), this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
		}
	}

	void CTextbox::MouseEvent(int x, int y, int iMouseKey, bool bDown)
	{
		//Handle mouse events

		//Check if component is ready
		if (!this->IsReady())
			return;

		if (iMouseKey == -1) { //If move event occured
			if ((x >= this->m_lsOwnerLocation.a + this->m_lsLocation.a) && (x <= this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a) && (y >= this->m_lsOwnerLocation.b + this->m_lsLocation.b) && (y <= this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_lsResolution.b)) { //If mouse is in range of the textbox
				if (!this->m_bHover)
					this->m_bHover = true;
			} else {
				if (this->m_bHover)
					this->m_bHover = false;
			}
		}
	}

	void CTextbox::OnKeyButton(int iKey, bool bDown)
	{
		//Handle key button events
		
		if ((iKey == KEY_BACKSPACE) && (bDown)) { //Delete last character
			if (this->m_szText[0]) {
				this->m_szText[strlen(this->m_szText)-1] = 0;
			}
		} else if ((iKey == KEY_ENTER) && (!bDown)) { //Save value
			strcpy_s(this->m_pCVar->szValue, this->m_szText);
		} else {
			if (bDown) {
				if (this->m_uiTextLen) {
					if (strlen(this->m_szText) < this->m_uiTextLen) { //Check if would not exceed maximum length
						char caChar[] = {(char)iKey, 0x00};
						strncat_s(this->m_szText, caChar, sizeof(char)); //Append text
					}
				} else {
					char caChar[] = {(char)iKey, 0x00};
					strncat_s(this->m_szText, caChar, sizeof(char)); //Append text
				}
			}
		}
	}

	void CTextbox::OnFocus(bool bStatus)
	{ 
		//Handle focus event

		//Call base method
		CBaseComponent::OnFocus(bStatus);
		
		if (!bStatus) { //Focus lost
			strcpy_s(this->m_pCVar->szValue, this->m_szText); //Save value

			this->m_bDrawUnderline = false;
		} else { //Focus gained
			//Initialize timers
			this->m_dwTimerCur = this->m_dwTimerInit = GetTickCount();
		}
	}

	void CTextbox::UpdateText(void)
	{
		//Update textbox text according to script var expression

		if (this->m_pCVar)
			strcpy_s(this->m_szText, this->m_pCVar->szValue);
	}

	/* CGroupbox methods */

	void CGroupbox::Draw(void)
	{
		//Draw component

		#define CZYVC_GROUPBOX_SECLINEDIST CZYVC_GROUPBOX_LINETOTEXTLEN + this->m_wndInfo.borderSize * 2 + (this->m_wndInfo.fontSizeW * (int)strlen(this->m_szText))

		//Draw frame
		this->m_diInterface.TpfnDrawBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, CZYVC_GROUPBOX_LINETOTEXTLEN, this->m_wndInfo.borderSize, this->m_wndInfo.borderSize, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
		this->m_diInterface.TpfnDrawBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a + CZYVC_GROUPBOX_SECLINEDIST, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_lsResolution.a - (CZYVC_GROUPBOX_SECLINEDIST), this->m_wndInfo.borderSize, this->m_wndInfo.borderSize, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
		this->m_diInterface.TpfnDrawBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_wndInfo.borderSize, this->m_lsResolution.b, this->m_wndInfo.borderSize, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
		this->m_diInterface.TpfnDrawBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_wndInfo.borderSize, this->m_lsResolution.b + this->m_wndInfo.borderSize, this->m_wndInfo.borderSize, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
		this->m_diInterface.TpfnDrawBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_lsResolution.b, this->m_lsResolution.a, this->m_wndInfo.borderSize, this->m_wndInfo.borderSize, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
		
		//Draw text
		this->m_diInterface.TpfnDrawText(this->m_szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + CZYVC_GROUPBOX_LINETOTEXTLEN + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b - (this->m_wndInfo.fontSizeH / 2), this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
	}

	/* CListbox methods */

	size_t CListbox::FindItemByCoord(int x, int y)
	{
		//Get item id via mouse coordinate

		//Get amount of items that suit into the listbox height
		size_t uiAmountOfItems = (this->m_lsResolution.b - this->m_wndInfo.borderSize * 2) / this->m_wndInfo.fontSizeH; 

		//Compare current shown items positions with given coordinates
		if (this->m_uiDrawingIndex < this->m_vItems.size()) {
			for (size_t i = 0; i < uiAmountOfItems; i++) {
				if (this->m_uiDrawingIndex + i < this->m_vItems.size()) {
					if ((x > this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize) && (x < this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize * 2 + this->m_lsResolution.a - CZYVC_LISTBOX_NAVIGATOR_WIDTH) && (y > this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize + ((int)i * this->m_wndInfo.fontSizeH)) && (y < this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize + ((int)i * this->m_wndInfo.fontSizeH) + this->m_wndInfo.fontSizeH)) {
						return this->m_uiDrawingIndex + i;
					}
				}
			}
		}

		return CZYVC_LISTBOX_INVALID_ITEM_ID;
	}

	void CListbox::Draw(void)
	{
		//Draw component

		//Draw list container
		this->DrawDeepBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_lsResolution.a, this->m_lsResolution.b);
		this->m_diInterface.TpfnDrawFilledBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_lsResolution.a - this->m_wndInfo.borderSize, this->m_lsResolution.b - this->m_wndInfo.borderSize, this->m_wndInfo.colors.bodyfill.r, this->m_wndInfo.colors.bodyfill.g, this->m_wndInfo.colors.bodyfill.b, this->m_wndInfo.colors.bodyfill.a);

		//Draw list navigator
		this->DrawSalientBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 2, this->m_lsOwnerLocation.b + this->m_lsLocation.b, CZYVC_LISTBOX_NAVIGATOR_WIDTH, this->m_lsResolution.b);
		this->m_diInterface.TpfnDrawText("U", this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 3 + ((CZYVC_LISTBOX_NAVIGATOR_WIDTH / 2) - (this->m_wndInfo.fontSizeW / 2)), this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
		this->m_diInterface.TpfnDrawText("D", this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 3 + ((CZYVC_LISTBOX_NAVIGATOR_WIDTH / 2) - (this->m_wndInfo.fontSizeW / 2)), this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize + (this->m_lsResolution.b - this->m_wndInfo.borderSize - this->m_wndInfo.fontSizeH), this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);

		//Get amount of items that suit into the listbox height
		size_t uiAmountOfItems = (this->m_lsResolution.b - this->m_wndInfo.borderSize * 2) / this->m_wndInfo.fontSizeH; 

		//Draw list items
		if (this->m_uiDrawingIndex < this->m_vItems.size()) {
			for (size_t i = 0; i < uiAmountOfItems; i++) {
				if (this->m_uiDrawingIndex + i < this->m_vItems.size()) {
					//Draw selection bar if this is the current selected item
					if (this->m_uiSelectedItem == this->m_uiDrawingIndex + i) {
						this->m_diInterface.TpfnDrawFilledBox(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize + ((int)i * this->m_wndInfo.fontSizeH), this->m_lsResolution.a - this->m_wndInfo.borderSize, this->m_wndInfo.fontSizeH, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
					}

					//Draw item text
					char szItemText[CZYVC_MAX_STRING_LEN];
					memset(szItemText, 0x00, sizeof(szItemText));
					int iMaxChars = (this->m_lsResolution.a - this->m_wndInfo.borderSize * 2) / (this->m_wndInfo.fontSizeW + this->m_wndInfo.fontCharDist);
					if (this->m_vItems[this->m_uiDrawingIndex + i].szExpression.length() < (size_t)iMaxChars)
						strcpy_s(szItemText, this->m_vItems[this->m_uiDrawingIndex + i].szExpression.c_str());
					else
						strncpy_s(szItemText, this->m_vItems[this->m_uiDrawingIndex + i].szExpression.c_str(), iMaxChars);
					this->m_diInterface.TpfnDrawText(szItemText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW,  this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize + ((int)i * this->m_wndInfo.fontSizeH), this->m_vItems[this->m_uiDrawingIndex + i].clOverriddenColor.r, this->m_vItems[this->m_uiDrawingIndex + i].clOverriddenColor.g, this->m_vItems[this->m_uiDrawingIndex + i].clOverriddenColor.b, this->m_vItems[this->m_uiDrawingIndex + i].clOverriddenColor.a);
				}
			}
		}
	}

	void CListbox::MouseEvent(int x, int y, int iMouseKey, bool bDown)
	{
		//Handle mouse event

		if ((iMouseKey != -1) && (!bDown)) { //Check if any mouse key is released
			//Get amount of items that suit into the listbox height
			size_t uiAmountOfItems = (this->m_lsResolution.b - this->m_wndInfo.borderSize * 2) / this->m_wndInfo.fontSizeH; 

			if ((x > this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 3) && (x < this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 3 + (CZYVC_LISTBOX_NAVIGATOR_WIDTH + this->m_wndInfo.borderSize * 2)) && (y > this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize) && (y < this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize * 2 + this->m_wndInfo.fontSizeH)) { //Check if clicked the "up"-button
				if (this->m_uiDrawingIndex > 0)
					this->m_uiDrawingIndex--;
			} else if ((x > this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 3) && (x < this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_lsResolution.a + this->m_wndInfo.borderSize * 3 + (CZYVC_LISTBOX_NAVIGATOR_WIDTH + this->m_wndInfo.borderSize * 2)) && (y > this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_lsResolution.b - this->m_wndInfo.borderSize - this->m_wndInfo.fontSizeH) && (y < this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_lsResolution.b - this->m_wndInfo.borderSize)) { //Check if clicked the "down"-button
				if (this->m_uiDrawingIndex < this->m_vItems.size() - uiAmountOfItems)
					this->m_uiDrawingIndex++;
			}
		}
	}

	void CListbox::OnMouseMove(int x, int y)
	{
		//Handle mouse move event

		this->m_iCurMousePosX = x;
		this->m_iCurMousePosY = y;
	}

	void CListbox::OnMouseButton(int iMouseKey, bool bDown)
	{
		//Handle mouse button events

		if (!bDown) { //Mouse button is released
			size_t uiItemId = this->FindItemByCoord(this->m_iCurMousePosX, this->m_iCurMousePosY); //Attempt to get item by mouse position
			if (uiItemId != CZYVC_LISTBOX_INVALID_ITEM_ID) { //If valid then...
				this->m_uiSelectedItem = uiItemId; //...update indicator
			}
		}
	}

	size_t CListbox::ItemCount(void)
	{
		//Get item count

		return this->m_vItems.size();
	}

	const char* CListbox::GetSelectedItem(void)
	{
		//Get selected item if any

		if ((this->m_uiSelectedItem == CZYVC_LISTBOX_INVALID_ITEM_ID) || (this->m_uiSelectedItem >= this->m_vItems.size()))
			return NULL;

		return this->m_vItems[this->m_uiSelectedItem].szExpression.c_str();
	}

	const char* CListbox::GetItem(const size_t uiItemId)
	{
		//Get item expression string pointer by id

		if (uiItemId < this->m_vItems.size())
			return this->m_vItems[uiItemId].szExpression.c_str();

		return NULL;
	}

	const size_t CListbox::FindItem(const char* szExpression)
	{
		//Get item id by expression if exists

		if (!szExpression)
			return CZYVC_LISTBOX_INVALID_ITEM_ID;

		std::string szToStdStr(szExpression);

		for (size_t i = 0; i < this->m_vItems.size(); i++) {
			if (this->m_vItems[i].szExpression == szToStdStr)
				return i;
		}

		return CZYVC_LISTBOX_INVALID_ITEM_ID;
	}

	bool CListbox::AddItem(const char* szExpression)
	{
		//Add default-colored item

		return this->AddItem(szExpression, &this->m_wndInfo.colors.title);
	}

	bool CListbox::AddItem(const char* szExpression, const color32_s* pColor)
	{
		//Add colored item

		if ((!szExpression) || (!pColor))
			return false;

		lb_item_s sItem;
		sItem.szExpression = std::string(szExpression);
		sItem.clOverriddenColor = *pColor;
		this->m_vItems.push_back(sItem);

		return true;
	}

	bool CListbox::DeleteItem(const size_t uiItemId)
	{
		//Remove item by id

		if (uiItemId < this->m_vItems.size()) {
			this->m_vItems.erase(this->m_vItems.begin() + uiItemId);
			return true;
		}

		return false;
	}

	bool CListbox::AlterItem(const size_t uiItemId, const char* szNewExpression)
	{
		//Change item expression

		if ((uiItemId == CZYVC_LISTBOX_INVALID_ITEM_ID) || (uiItemId >= this->m_vItems.size()))
			return false;

		this->m_vItems[uiItemId].szExpression = std::string(szNewExpression);

		return true;
	}

	bool CListbox::AlterItem(const size_t uiItemId, const color32_s* pColor)
	{
		//Change item color

		if ((uiItemId == CZYVC_LISTBOX_INVALID_ITEM_ID) || (uiItemId >= this->m_vItems.size()))
			return false;

		this->m_vItems[uiItemId].clOverriddenColor = *pColor;

		return true;
	}

	void CListbox::ClearItems(void)
	{
		//Clear item list

		this->m_vItems.clear();
	}

	void CListbox::ScrollToEnd(void)
	{
		//Scroll to end

		//Get amount of items that suit into the listbox height
		size_t uiAmountOfItems = (this->m_lsResolution.b - this->m_wndInfo.borderSize * 2) / this->m_wndInfo.fontSizeH; 

		//Update drawing index
		this->m_uiDrawingIndex = (this->m_vItems.size() >= uiAmountOfItems) ? this->m_vItems.size() - uiAmountOfItems : 0;
	}

	/* Imagebox methods */

	void CImagebox::Draw(void)
	{
		if (this->m_uiImageId != CZYVC_IMAGEBOX_INVALID_IMAGE_ID) {
			this->m_diInterface.TpfnDrawImage(this->m_uiImageId, this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b);
		}
	}

	/* Contextmenu methods */

	bool CContextMenu::Initialize(LPCSTR lpszName, CBaseComponent* pAttachToThisComponent, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize component

		if (!pAttachToThisComponent)
			return false;

		this->m_pTargetComponent = pAttachToThisComponent;

		return CBaseComponent::Initialize(CZYVC_CONTEXTMENU_IDENT, lpszName, pWindowInfo, pDrawingInterface);
	}

	const size_t CContextMenu::GetContextMenuWidth(void)
	{
		//Get width of context menu

		size_t uiCurSize = 0;

		for (size_t i = 0; i < this->m_vCtxItems.size(); i++) {
			size_t uiCurItemSize = strlen(this->m_vCtxItems[i].szText) * (this->m_wndInfo.fontSizeW + this->m_wndInfo.fontCharDist * 2);
			if (uiCurItemSize > uiCurSize)
				uiCurSize = uiCurItemSize;
		}

		return uiCurSize + this->m_wndInfo.borderSize * 2 + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST * 2;
	}

	const size_t CContextMenu::GetContextMenuHeight(void)
	{
		//Get height of context menu

		return this->m_wndInfo.borderSize * 2 + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST * 2 + this->m_vCtxItems.size() * this->m_wndInfo.fontSizeH;
	}

	void CContextMenu::UpdateMenuSize(void)
	{
		//Update menu size

		//Update size values
		this->m_lsResolution.a = this->m_uiCurrentMenuWidth = this->GetContextMenuWidth();
		this->m_lsResolution.b = this->m_uiCurrentMenuHeight = this->GetContextMenuHeight();
	}

	size_t CContextMenu::FindItemByCoord(int x, int y)
	{
		//Get item id via mouse coordinate
		
		//Ceck if is in X range
		if ((x <= this->m_lsOwnerLocation.a + this->m_lsLocation.a) || (x >= this->m_lsOwnerLocation.a + this->m_lsLocation.a + (int)this->m_uiCurrentMenuWidth))
			return CZYVC_CONTEXTMENU_INVALID_ITEM_ID;

		//Compare current shown items positions with given coordinates
		for (size_t i = 0; i < this->m_vCtxItems.size(); i++) {
			if (((size_t)y >= this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST + i * this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist) && ((size_t)y <= this->m_lsOwnerLocation.b + this->m_lsLocation.b + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST + this->m_wndInfo.borderSize + ((i+1) * this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist))) //Compare if y coord is inside the y range of current item
				return i;
		}

		return CZYVC_CONTEXTMENU_INVALID_ITEM_ID;
	}

	size_t CContextMenu::AddItem(const char* pszItemText, const TpfnItemClickEvent pfnItemClickEventFunc)
	{
		//Add new contextmenu item

		if (!pszItemText)
			return CZYVC_CONTEXTMENU_INVALID_ITEM_ID;

		//Setup data
		ctx_menu_item_s sItem;
		strcpy_s(sItem.szText, pszItemText);
		sItem.pfnItemClickEventFunc = pfnItemClickEventFunc;
		sItem.bIsPlaceHolder = strcmp(pszItemText, "-") == 0;

		//Add to list
		this->m_vCtxItems.push_back(sItem);

		//Update menu size values
		this->UpdateMenuSize();

		return this->m_vCtxItems.size() - 1;
	}

	size_t CContextMenu::InsertItem(const char* pszItemText, const size_t uiAfterThisItem, const TpfnItemClickEvent pfnItemClickEventFunc)
	{
		//Insert item into list

		if ((!pszItemText) || (!pfnItemClickEventFunc) || (uiAfterThisItem >= this->m_vCtxItems.size() + 1))
			return CZYVC_CONTEXTMENU_INVALID_ITEM_ID;

		//Setup data
		ctx_menu_item_s sItem;
		strcpy_s(sItem.szText, pszItemText);
		sItem.pfnItemClickEventFunc = pfnItemClickEventFunc;
		sItem.bIsPlaceHolder = strcmp(pszItemText, "-") == 0;

		//Add to list
		this->m_vCtxItems.insert(this->m_vCtxItems.begin() + uiAfterThisItem, sItem);

		//Update menu size values
		this->UpdateMenuSize();

		return uiAfterThisItem + 1;
	}

	bool CContextMenu::RemoveItem(const size_t uiItemId)
	{
		//Remove item from list

		if (uiItemId >= this->m_vCtxItems.size())
			return false;

		//Erase from list
		this->m_vCtxItems.erase(this->m_vCtxItems.begin() + uiItemId);

		//Update menu size values
		this->UpdateMenuSize();

		return true;
	}

	void CContextMenu::Raise(const int x, const int y)
	{
		//Show context menu at given position (relative)

		//Update relative position
		
		this->m_lsLocation.a = x;
		this->m_lsLocation.b = y;

		//Set visible
		this->Show();

		//Get focus
		this->m_bGotFocus = true;
	}

	void CContextMenu::Draw(void)
	{
		//Draw component

		if (!this->m_bShow)
			return;

		//Draw border
		this->DrawSalientBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_uiCurrentMenuWidth, this->m_uiCurrentMenuHeight);

		//Draw form area
		DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_uiCurrentMenuWidth - this->m_wndInfo.borderSize, this->m_uiCurrentMenuHeight - this->m_wndInfo.borderSize, this->m_wndInfo.colors.bodyfill.r, this->m_wndInfo.colors.bodyfill.g, this->m_wndInfo.colors.bodyfill.b, this->m_wndInfo.colors.bodyfill.a);

		//Get selected item if any
		const size_t uiCurrentSelectedItem = this->FindItemByCoord(this->m_iCurMousePosX, this->m_iCurMousePosY);

		//Draw items
		for (size_t i = 0; i < this->m_vCtxItems.size(); i++) {
			//Draw field around this item if it is selected and not a placeholder
			if ((i == uiCurrentSelectedItem) && (!this->m_vCtxItems[i].bIsPlaceHolder)) {
				DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST, this->m_lsOwnerLocation.b + this->m_lsLocation.b + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST + (i * this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist), this->m_uiCurrentMenuWidth - CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST - this->m_wndInfo.borderSize, this->m_wndInfo.fontSizeH, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
			}
			
			if (this->m_vCtxItems[i].bIsPlaceHolder) {
				//Draw line
				DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST, this->m_lsOwnerLocation.b + this->m_lsLocation.b + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST + (i * this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist) + (((this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist) / 2) - (this->m_wndInfo.borderSize / 2)), this->m_uiCurrentMenuWidth - this->m_wndInfo.borderSize * 2 - 1, this->m_wndInfo.borderSize / 2, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
			} else {
				//Draw text
				DRAW_TEXT(this->m_vCtxItems[i].szText, this->m_wndInfo.szFont, this->m_wndInfo.fontSizeW, this->m_wndInfo.fontSizeH, this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST, this->m_lsOwnerLocation.b + this->m_lsLocation.b + CZYVC_CONTEXTMENU_BORDER_TO_CONTENT_DIST + (i * this->m_wndInfo.fontSizeH + this->m_wndInfo.fontLineDist), this->m_wndInfo.colors.title.r, this->m_wndInfo.colors.title.g, this->m_wndInfo.colors.title.b, this->m_wndInfo.colors.title.a);
			}
		}
	}

	void CContextMenu::MouseEvent(int x, int y, int iMouseKey, bool bDown)
	{
		//Task: Check with target component

		if (!this->m_pTargetComponent)
			return;

		//if (this->m_pTargetComponent->HasFocus()) { //Check if our target component has currently focus
			if ((iMouseKey == VK_RBUTTON) && (!bDown)) { //Check for rightclick
				//Get pointer to location data
				const component_size_s* pTargetLocation = this->m_pTargetComponent->GetLocation();
				if ((pTargetLocation)) {
					//Get pointer to size data
					const component_size_s* pTargetSize = this->m_pTargetComponent->GetSize();
					if (pTargetSize) {
						//Check if mouse position ins in range of the target component
						if ((x >= this->m_lsOwnerLocation.a + pTargetLocation->a) && (x <= this->m_lsOwnerLocation.a + pTargetLocation->a + pTargetSize->a) && (y >= this->m_lsOwnerLocation.b + pTargetLocation->b) && (y <= this->m_lsOwnerLocation.b + pTargetLocation->b + pTargetSize->b)) {
							this->Raise(x - this->m_lsOwnerLocation.a, y - this->m_lsOwnerLocation.b); //Raise context menu
						}
					}
				}
			}
		//}
	}

	void CContextMenu::OnMouseMove(int x, int y)
	{
		//Handle mouse move event

		this->m_iCurMousePosX = x;
		this->m_iCurMousePosY = y;
	}

	void CContextMenu::OnMouseButton(int iMouseKey, bool bDown)
	{
		//Handle mouse button clicks

		if (!this->m_bShow)
			return;

		if ((iMouseKey == VK_LBUTTON) && (!bDown)) { //Left mouse  button released
			const size_t uiItemId = this->FindItemByCoord(this->m_iCurMousePosX, this->m_iCurMousePosY); //Find item by coord
			if ((uiItemId != CZYVC_CONTEXTMENU_INVALID_ITEM_ID) && (uiItemId < this->m_vCtxItems.size())) { //Verify ID
				if ((!this->m_vCtxItems[uiItemId].bIsPlaceHolder) && (this->m_vCtxItems[uiItemId].pfnItemClickEventFunc)) { //Verifiy flag and function pointer value
					this->m_vCtxItems[uiItemId].pfnItemClickEventFunc(this, uiItemId); //Call event function
					this->m_bGotFocus = false; //Release focus
					this->Hide(); //Hide menu
				}
			}
		}
	}

	void CContextMenu::OnFocus(bool bStatus)
	{
		//Signaled for focus events

		CBaseComponent::OnFocus(bStatus);

		if (!bStatus)
			this->Hide();
	}

	/* ProgressBar methods */

	bool CProgressBar::Initialize(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Initialize componen

		return CBaseComponent::Initialize(CZYVC_PROGRESSBAR_IDENT, lpszName, pWindowInfo, pDrawingInterface);
	}

	int CProgressBar::GetAmountOfBlocks(void)
	{
		//Get amount of blocks that suit into entire component width area

		return (this->m_lsResolution.a - this->m_wndInfo.borderSize * 2) / (this->m_iBlockWidth + this->m_iBlockDist) + 1;
	}

	int CProgressBar::GetBlockNum(int iValue)
	{
		//Calculate the number of a block by value (range: 0 - amount of blocks, iValue must be between this range)

		return (int)((float)this->GetAmountOfBlocks() / (float)this->m_iMax * (float)iValue);
	}

	void CProgressBar::SetRange(int iMin, int iMax)
	{
		//Set range

		this->m_iMin = iMin;
		this->m_iMax = iMax;
	}
	
	void CProgressBar::SetValue(int iValue)
	{
		//Set value

		//Correct if needed
		
		if (iValue < this->m_iMin)
			iValue = this->m_iMin;

		if (iValue > this->m_iMax)
			iValue = this->m_iMax;

		//Save value
		this->m_iValue = iValue;
	}
	
	void CProgressBar::Draw(void)
	{
		//Draw window

		//Draw box
		this->DrawDeepBorder(this->m_lsOwnerLocation.a + this->m_lsLocation.a, this->m_lsOwnerLocation.b + this->m_lsLocation.b, this->m_lsResolution.a, this->m_lsResolution.b);
		DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize, this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_lsResolution.a - this->m_wndInfo.borderSize, this->m_lsResolution.b - this->m_wndInfo.borderSize, this->m_wndInfo.colors.bodyfill.r, this->m_wndInfo.colors.bodyfill.g, this->m_wndInfo.colors.bodyfill.b, this->m_wndInfo.colors.bodyfill.a);
	
		//Draw blocks according to value
		const int iBlocksToHandle = this->GetBlockNum(this->m_iValue);
		for (int i = 0; i < iBlocksToHandle; i++) {
			DRAW_AREA(this->m_lsOwnerLocation.a + this->m_lsLocation.a + this->m_wndInfo.borderSize + i * (this->m_iBlockWidth + this->m_iBlockDist), this->m_lsOwnerLocation.b + this->m_lsLocation.b + this->m_wndInfo.borderSize, this->m_iBlockWidth, this->m_lsResolution.b - this->m_wndInfo.borderSize, this->m_wndInfo.colors.headfill.r, this->m_wndInfo.colors.headfill.g, this->m_wndInfo.colors.headfill.b, this->m_wndInfo.colors.headfill.a);
		}
	}

	/* Interface functions */

	bool SetConfigInt(const CzyConfigMgr::CConfigInt* pConfigInt)
	{
		//Set CzyConfigMgr::CConfigInt interface pointer

		if (!pConfigInt)
			return false;

		nm_pConfigInt = (CzyConfigMgr::CConfigInt*)pConfigInt;

		return true;
	}

	CForm* CreateForm(LPCSTR lpszName, const windowinfo_s* pWindowInfo, const drawinginterface_s* pDrawingInterface)
	{
		//Create new form class instance

		if ((!pWindowInfo) || (!pDrawingInterface))
			return NULL;

		//Allocate memory
		CForm* pForm = new CForm;
		if (!pForm)
			return NULL;

		//Initialize component
		if (!pForm->Initialize(lpszName, pWindowInfo, pDrawingInterface)) {
			delete pForm;
			return NULL;
		}

		return pForm;
	}
	
	CLabel* AttachLabel(CForm* pForm, LPCSTR lpszName, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, const char* lpszText)
	{
		//Create new label and add to form component

		if ((!pForm) || (!lpszText))
			return NULL;
		
		//Allocate memory for component instance
		CLabel* pLabel = new CLabel;
		if (!pLabel)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pLabel;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.title.r = r;
		mcColors.title.g = g;
		mcColors.title.b = b;
		mcColors.title.a = a;
		pLabel->SetColors(&mcColors);

		//Initialize component
		if (!pLabel->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pLabel;
			return NULL;
		}
		
		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pLabel;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pLabel->OnMove(pLocation->a, pLocation->b);

		//Set label location (relative)
		if (!pLabel->SetLocation(x, y)) {
			delete pLabel;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pLabel, NULL, NULL)) {
			delete pLabel;
			return NULL;
		}
		
		//Set text if desired
		if (lpszText) {
			pLabel->SetText(lpszText);
		}

		return pLabel;
	}

	CButton* AttachButton(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pTextColor, color32_s* pBoxColor, color32_s* pHoverColor, const char* lpszText, const TpfnEventFunc pfnEventFunc)
	{
		//Create new button and add to form component

		if ((!pForm) || (!pTextColor) || (!pBoxColor) || (!pHoverColor) || (!lpszText) || (!pfnEventFunc))
			return NULL;

		//Allocate memory for component instance
		CButton* pButton = new CButton;
		if (!pButton)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pButton;
			return NULL;
		}
		
		//Initialize component
		if (!pButton->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pButton;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.headfill.r = pBoxColor->r;
		mcColors.headfill.g = pBoxColor->g;
		mcColors.headfill.b = pBoxColor->b;
		mcColors.headfill.a = pBoxColor->a;
		mcColors.title.r = pTextColor->r;
		mcColors.title.g = pTextColor->g;
		mcColors.title.b = pTextColor->b;
		mcColors.title.a = pTextColor->a;
		mcColors.bodyfill.r = pHoverColor->r;
		mcColors.bodyfill.g = pHoverColor->g;
		mcColors.bodyfill.b = pHoverColor->b;
		mcColors.bodyfill.a = pHoverColor->a;
		pButton->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pButton;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pButton->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pButton->SetLocation(x, y)) {
			delete pButton;
			return NULL;
		}
		
		//Set size
		if (!pButton->SetSize(w, h)) {
			delete pButton;
			return NULL;
		}
		
		//Set text
		if (!pButton->SetText(lpszText)) {
			delete pButton;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pButton, NULL, pfnEventFunc)) {
			delete pButton;
			return NULL;
		}
		
		return pButton;
	}

	CCheckbox* AttachCheckbox(CForm* pForm, LPCSTR lpszName, int x, int y, color32_s* pTextColor, color32_s* pBoxColor, color32_s* pHoverColor, const char* lpszText, const char* lpszAffectedVar, bool bInitialValue)
	{
		//Create new checkbox and add to form component

		if ((!pForm) || (!pTextColor) || (!pBoxColor) || (!pHoverColor) || (!lpszText) || (!lpszAffectedVar))
			return NULL;

		//Allocate memory for component instance
		CCheckbox* pCheckbox = new CCheckbox;
		if (!pCheckbox)
			return NULL;
		
		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pCheckbox;
			return NULL;
		}
		
		//Initialize component
		if (!pCheckbox->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pCheckbox;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.headfill.r = pBoxColor->r;
		mcColors.headfill.g = pBoxColor->g;
		mcColors.headfill.b = pBoxColor->b;
		mcColors.headfill.a = pBoxColor->a;
		mcColors.title.r = pTextColor->r;
		mcColors.title.g = pTextColor->g;
		mcColors.title.b = pTextColor->b;
		mcColors.title.a = pTextColor->a;
		mcColors.bodyfill.r = pHoverColor->r;
		mcColors.bodyfill.g = pHoverColor->g;
		mcColors.bodyfill.b = pHoverColor->b;
		mcColors.bodyfill.a = pHoverColor->a;
		pCheckbox->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pCheckbox;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pCheckbox->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pCheckbox->SetLocation(x, y)) {
			delete pCheckbox;
			return NULL;
		}
		
		//Set text
		if (!pCheckbox->SetText(lpszText)) {
			delete pCheckbox;
			return NULL;
		}

		//Set size
		if (!pCheckbox->SetSize(pWindowInfo->fontSizeH + pWindowInfo->fontLineDist + (int)strlen(lpszText) * pWindowInfo->fontSizeW, pWindowInfo->fontSizeH)) {
			delete pCheckbox;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pCheckbox, lpszAffectedVar, NULL)) {
			delete pCheckbox;
			return NULL;
		}

		//Set initial value
		pCheckbox->SetValue(&bInitialValue);
		
		return pCheckbox;
	}

	CTextbox* AttachTextBox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pTextColor, color32_s* pBoxColor, color32_s* pHoverColor, const char* lpszText, const char* lpszAffectedVar, unsigned int uiTextLen)
	{
		//Create new textbox and add to form component

		if ((!pForm) || (!pTextColor) || (!pBoxColor) || (!pHoverColor) || (!lpszText) || (!lpszAffectedVar))
			return NULL;

		//Allocate memory for component instance
		CTextbox* pTextbox = new CTextbox;
		if (!pTextbox)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pTextbox;
			return NULL;
		}
		
		//Initialize component
		if (!pTextbox->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pTextbox;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.headfill.r = pBoxColor->r;
		mcColors.headfill.g = pBoxColor->g;
		mcColors.headfill.b = pBoxColor->b;
		mcColors.headfill.a = pBoxColor->a;
		mcColors.title.r = pTextColor->r;
		mcColors.title.g = pTextColor->g;
		mcColors.title.b = pTextColor->b;
		mcColors.title.a = pTextColor->a;
		mcColors.bodyfill.r = pHoverColor->r;
		mcColors.bodyfill.g = pHoverColor->g;
		mcColors.bodyfill.b = pHoverColor->b;
		mcColors.bodyfill.a = pHoverColor->a;
		pTextbox->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pTextbox;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pTextbox->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pTextbox->SetLocation(x, y)) {
			delete pTextbox;
			return NULL;
		}
		
		//Set text length
		pTextbox->SetTextLen((uiTextLen > 0) ? uiTextLen : CZYVC_TEXTBOX_DEFAULT_LENGTH);

		//Set text
		if (!pTextbox->SetText(lpszText)) {
			delete pTextbox;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pTextbox, lpszAffectedVar, NULL)) {
			delete pTextbox;
			return NULL;
		}

		return pTextbox;
	}

	CGroupbox* AttachGroupbox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pTextColor, color32_s* pFrameColor, const char* lpszText)
	{
		//Create new groupbox and add to form component

		if ((!pForm) || (!pTextColor) || (!pFrameColor) || (!lpszText))
			return NULL;

		//Allocate memory for component instance
		CGroupbox* pGroupbox = new CGroupbox;
		if (!pGroupbox)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pGroupbox;
			return NULL;
		}
		
		//Initialize component
		if (!pGroupbox->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pGroupbox;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.headfill.r = pFrameColor->r;
		mcColors.headfill.g = pFrameColor->g;
		mcColors.headfill.b = pFrameColor->b;
		mcColors.headfill.a = pFrameColor->a;
		mcColors.title.r = pTextColor->r;
		mcColors.title.g = pTextColor->g;
		mcColors.title.b = pTextColor->b;
		mcColors.title.a = pTextColor->a;
		pGroupbox->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pGroupbox;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pGroupbox->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pGroupbox->SetLocation(x, y)) {
			delete pGroupbox;
			return NULL;
		}

		//Set size
		if (!pGroupbox->SetSize(w, h)) {
			delete pGroupbox;
			return NULL;
		}

		//Set text
		if (!pGroupbox->SetText(lpszText)) {
			delete pGroupbox;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pGroupbox, NULL, NULL)) {
			delete pGroupbox;
			return NULL;
		}

		return pGroupbox;
	}

	CListbox* AttachListbox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pItemColor, color32_s* pBackgroundColor, color32_s* pSelectedItemColor)
	{
		//Create new listbox and add to form component

		if ((!pForm) || (!pItemColor) || (!pBackgroundColor) || (!pSelectedItemColor))
			return NULL;

		//Allocate memory for component instance
		CListbox* pListbox = new CListbox;
		if (!pListbox)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pListbox;
			return NULL;
		}
		
		//Initialize component
		if (!pListbox->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pListbox;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.bodyfill.r = pBackgroundColor->r;
		mcColors.bodyfill.g = pBackgroundColor->g;
		mcColors.bodyfill.b = pBackgroundColor->b;
		mcColors.bodyfill.a = pBackgroundColor->a;
		mcColors.title.r = pItemColor->r;
		mcColors.title.g = pItemColor->g;
		mcColors.title.b = pItemColor->b;
		mcColors.title.a = pItemColor->a;
		mcColors.headfill.r = pSelectedItemColor->r;
		mcColors.headfill.g = pSelectedItemColor->g;
		mcColors.headfill.b = pSelectedItemColor->b;
		mcColors.headfill.a = pSelectedItemColor->a;
		pListbox->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pListbox;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pListbox->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pListbox->SetLocation(x, y)) {
			delete pListbox;
			return NULL;
		}

		//Set size
		if (!pListbox->SetSize(w, h)) {
			delete pListbox;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pListbox, NULL, NULL)) {
			delete pListbox;
			return NULL;
		}

		return pListbox;
	}

	CImagebox* AttachImagebox(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, const size_t uiImageId)
	{
		//Create new imagebox and add to form component

		if ((!pForm) || (!lpszName))
			return NULL;

		//Allocate memory for component instance
		CImagebox* pImagebox = new CImagebox;
		if (!pImagebox)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pImagebox;
			return NULL;
		}
		
		//Initialize component
		if (!pImagebox->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pImagebox;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		pImagebox->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pImagebox;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pImagebox->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pImagebox->SetLocation(x, y)) {
			delete pImagebox;
			return NULL;
		}

		//Set size
		if (!pImagebox->SetSize(w, h)) {
			delete pImagebox;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pImagebox, NULL, NULL)) {
			delete pImagebox;
			return NULL;
		}

		return pImagebox;
	}

	CContextMenu* CreateContextMenu(CForm* pForm, LPCSTR lpszName, LPCSTR lpszTargetCompName, color32_s* pBackgroundColor, color32_s* pSelColor, color32_s* pTextColor)
	{
		//Create new contextmenu and add component to form

		if ((!pForm) || (!lpszName) || (!pBackgroundColor) || (!pSelColor) || (!pTextColor))
			return NULL;

		//Find target component
		CBaseComponent* pTargetComponent = pForm->GetComponent(lpszTargetCompName);
		if (!pTargetComponent)
			return NULL;

		//Allocate new component instance
		CContextMenu* pContextMenu = new CContextMenu;
		if (!pContextMenu)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pContextMenu;
			return NULL;
		}

		//Initialize component
		if (!pContextMenu->Initialize(lpszName, pTargetComponent, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pContextMenu;
			return NULL;
		}

		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.bodyfill.r = pBackgroundColor->r;
		mcColors.bodyfill.g = pBackgroundColor->g;
		mcColors.bodyfill.b = pBackgroundColor->b;
		mcColors.bodyfill.a = pBackgroundColor->a;
		mcColors.title.r = pTextColor->r;
		mcColors.title.g = pTextColor->g;
		mcColors.title.b = pTextColor->b;
		mcColors.title.a = pTextColor->a;
		mcColors.headfill.r = pSelColor->r;
		mcColors.headfill.g = pSelColor->g;
		mcColors.headfill.b = pSelColor->b;
		mcColors.headfill.a = pSelColor->a;
		pContextMenu->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pContextMenu;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pContextMenu->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pContextMenu->SetLocation(1, 1)) {
			delete pContextMenu;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pContextMenu, NULL, NULL)) {
			delete pContextMenu;
			return NULL;
		}

		return pContextMenu;
	}

	CProgressBar* AttachProgressBar(CForm* pForm, LPCSTR lpszName, int x, int y, int w, int h, color32_s* pBoxColor, color32_s* pBlockColor, int iMin, int iMax, int iStartValue)
	{
		//Create new progressbar and add to form component

		if ((!pForm) || (!lpszName) || (!pBoxColor) || (!pBlockColor))
			return NULL;

		//Allocate memory for component instance
		CProgressBar* pProgressBar = new CProgressBar;
		if (!pProgressBar)
			return NULL;

		//Get window info
		windowinfo_s* pWindowInfo = pForm->GetWindowInfo();
		if (!pWindowInfo) {
			delete pProgressBar;
			return NULL;
		}
		
		//Initialize component
		if (!pProgressBar->Initialize(lpszName, pWindowInfo, pForm->GetDrawingInterface())) {
			delete pProgressBar;
			return NULL;
		}
		
		//Set colors
		menu_color_s mcColors = pWindowInfo->colors;
		mcColors.headfill.r = pBlockColor->r;
		mcColors.headfill.g = pBlockColor->g;
		mcColors.headfill.b = pBlockColor->b;
		mcColors.headfill.a = pBlockColor->a;
		mcColors.bodyfill.r = pBoxColor->r;
		mcColors.bodyfill.g = pBoxColor->g;
		mcColors.bodyfill.b = pBoxColor->b;
		mcColors.bodyfill.a = pBoxColor->a;
		pProgressBar->SetColors(&mcColors);

		//Get owner location
		const component_size_s* pLocation = pForm->GetLocation();
		if (!pLocation) {
			delete pProgressBar;
			return NULL;
		}
		
		//Call OnMove() event method initially
		pProgressBar->OnMove(pLocation->a, pLocation->b);

		//Set location (relative)
		if (!pProgressBar->SetLocation(x, y)) {
			delete pProgressBar;
			return NULL;
		}

		//Set size
		if (!pProgressBar->SetSize(w, h)) {
			delete pProgressBar;
			return NULL;
		}
		
		//Add to form
		if (!pForm->AddVisualComponent(pProgressBar, NULL, NULL)) {
			delete pProgressBar;
			return NULL;
		}

		//Set range and start value
		pProgressBar->SetRange(iMin, iMax);
		pProgressBar->SetValue(iStartValue);

		return pProgressBar;
	}

	void FreeForm(CForm* pForm)
	{
		//Free form

		if (!pForm)
			return;

		//Call Free() method
		pForm->Free();

		//Free memory
		delete pForm;
	}

	//Data
	CzyConfigMgr::CConfigInt* nm_pConfigInt = NULL;
};
