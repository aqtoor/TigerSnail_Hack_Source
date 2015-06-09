#include "d3d.h"
#include "utils.h"
#include "vars.h"
#include "memory.h"
#include "inputmgr.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.1
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	d3d.cpp: D3D9 renderer class implementations, D3D9 hook implementations and D3D9 game-context access implementations
*/

//======================================================================
IDirect3DDevice9* pD3DDeviceFromGame = NULL;
TpfnOrigEndScene pfnOrigEndScene = NULL;
CD3DRenderer g_oD3DRenderer;
CzyVisualComponents::CForm* g_pMainForm = NULL;
//======================================================================

//======================================================================
bool CD3DRenderer::Initialize(IDirect3DDevice9* pDevice, const std::string& szImagePath)
{
	//Initialize DirectX
	
	if ((!pDevice) || (!szImagePath.length()))
		return false;
	
	//Get pointer to backbuffer
	if (FAILED(pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer)))
		return false;

	//Save data
	this->m_pDevice = pDevice;
	this->m_szImagePath = szImagePath;

	return true;
}
//======================================================================

//======================================================================
void CD3DRenderer::Clear(void)
{
	//Clear data and free memory

	//Clear fonts
	
	for (size_t i = 0; i < m_vFonts.size(); i++) {
		if (m_vFonts[i]) {
			m_vFonts[i]->pFont->Release(); //Release object
			delete m_vFonts[i]; //Free memory
		}
	}

	m_vFonts.clear(); //Clear list

	//Clear images

	for (size_t i = 0; i < m_vImages.size(); i++) {
		if (m_vImages[i]) {
			m_vImages[i]->pSurface->Release(); //Release object
			delete m_vImages[i]; //Free memory
		}
	}

	m_vImages.clear(); //Clear list

	//Clear data
	m_pDevice = NULL;
}
//======================================================================

//======================================================================
d3dfont_s* CD3DRenderer::GetExistingFont(const std::string& szFontName, unsigned char ucFontSizeW, unsigned char ucFontSizeH)
{
	//Get font data pointer

	if (!szFontName.length())
		return NULL;

	//Search in list
	for (size_t i = 0; i < m_vFonts.size(); i++) {
		if ((m_vFonts[i]->ucFontSizeW == ucFontSizeW) && (m_vFonts[i]->ucFontSizeH == ucFontSizeH) && (m_vFonts[i]->szFontName == szFontName)) { //Compare data
			return m_vFonts[i];
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
d3dfont_s* CD3DRenderer::RegisterFont(const std::string& szFontName, unsigned char ucFontSizeW, unsigned char ucFontSizeH)
{
	//Create new font

	if (!szFontName.length())
		return NULL;

	d3dfont_s* pFont = NULL;

	//Check if already exists
	pFont = this->GetExistingFont(szFontName, ucFontSizeW, ucFontSizeH);
	if (pFont)
		return pFont;

	//Allocate memory font font structure
	pFont = new d3dfont_s;
	if (!pFont)
		return NULL;

	//Create font object
	if (FAILED(D3DXCreateFontA(m_pDevice, (int)ucFontSizeH, (UINT)(ucFontSizeW), FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, szFontName.c_str(), &pFont->pFont))) {
		delete [] pFont;
		return NULL;
	}

	//Set data
	strcpy_s(pFont->szFontName, szFontName.c_str());
	pFont->ucFontSizeW = ucFontSizeW;
	pFont->ucFontSizeH = ucFontSizeH;

	//Add to list
	m_vFonts.push_back(pFont);

	return pFont;
}
//======================================================================

//======================================================================
size_t CD3DRenderer::LoadImage(const std::string& szImage)
{
	//Load image

	if (!szImage.length())
		return INVALID_LIST_ID;

	//Setup full file path
	std::string szAbsImagePath = this->m_szImagePath + "\\" + szImage;

	//Allocate memory
	d3dimage_s* pImageData = new d3dimage_s;
	if (!pImageData)
		return INVALID_LIST_ID;
	
	//Get image info
	if (FAILED(D3DXGetImageInfoFromFileA(szAbsImagePath.c_str(), &pImageData->d3dImageInfo))) {
		delete pImageData;
		return INVALID_LIST_ID;
	}
	
	//Create surface for image
	if (FAILED(m_pDevice->CreateOffscreenPlainSurface(pImageData->d3dImageInfo.Width, pImageData->d3dImageInfo.Height, pImageData->d3dImageInfo.Format, D3DPOOL_DEFAULT, &pImageData->pSurface, NULL))) {
		delete pImageData;
		return INVALID_LIST_ID;
	}

	//Load image into surface
	if (FAILED(D3DXLoadSurfaceFromFileA(pImageData->pSurface, NULL, NULL, szAbsImagePath.c_str(), NULL, D3DX_DEFAULT, 0, &pImageData->d3dImageInfo))) {
		delete pImageData;
		return INVALID_LIST_ID;
	}

	//Save file name
	pImageData->szFile = szAbsImagePath;

	//Add to list
	this->m_vImages.push_back(pImageData);

	return this->m_vImages.size() - 1; //Return item ID
}
//======================================================================

//======================================================================
bool CD3DRenderer::DrawString(const std::string& szText, const std::string& szFont, unsigned char ucFontSizeW, unsigned char ucFontSizeH, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	//Draw a string on backbuffer

	if ((!m_pDevice) || (!szText.length()) || (!szFont.length()))
		return false;

	//Get font or make one

	d3dfont_s* pFont;

	pFont = this->GetExistingFont(szFont, ucFontSizeW, ucFontSizeH);
	if (!pFont) {
		pFont = this->RegisterFont(szFont, ucFontSizeW, ucFontSizeH);
		if (!pFont)
			return false;
	}

	//Set data
	RECT rect = {x, y, x, y};
	D3DCOLOR d3dcColor = D3DCOLOR_ARGB(a, r, g, b);

	//Calculate font rect
	int iResult = pFont->pFont->DrawTextA(NULL, szText.c_str(), (int)szText.length(), &rect, DT_NOCLIP, d3dcColor);

	return false;
}
//======================================================================

//======================================================================
bool CD3DRenderer::DrawBox(int x, int y, int w, int h, int iThickness, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	//Draw a box skeleton on backbuffer

	if (!m_pDevice)
		return false;

	if (!DrawFilledBox(x, y, w, iThickness, r, g, b, a))
		return false;

	if (!DrawFilledBox(x, y, iThickness, h, r, g, b, a))
		return false;

	if (!DrawFilledBox(x + w, y, iThickness, h + iThickness, r, g, b, a))
		return false;

	return DrawFilledBox(x, y + h, w, iThickness, r, g, b, a);
}
//======================================================================

//======================================================================
bool CD3DRenderer::DrawFilledBox(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	//Draw a filled box on backbuffer

	if (!m_pDevice)
		return false;

	IDirect3DSurface9* pSurface = NULL;

    //Create surface
	if (FAILED(m_pDevice->CreateOffscreenPlainSurface(w, h, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSurface, NULL)))
		return false;

	//Fill surface with color
	if (FAILED(m_pDevice->ColorFill(pSurface, NULL, D3DCOLOR_ARGB(a, r, g, b)))) {
		pSurface->Release();
		return false;
	}

	//Set destination area
	RECT rTargetRect;
	rTargetRect.left = x;
	rTargetRect.top = y;
	rTargetRect.right = x + w;
	rTargetRect.bottom = y + h;

	//Copy surface into backbuffer at desired position
	bool bResult = SUCCEEDED(m_pDevice->StretchRect(pSurface, NULL, m_pBackBuffer, &rTargetRect, D3DTEXF_NONE));
	
	//Release surface
	pSurface->Release();

	return bResult;
}
//======================================================================

//======================================================================
bool CD3DRenderer::DrawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	//Draw a line on backbuffer

	if (!m_pDevice)
		return false;

	//Define custom vertex type
	struct D3DVERTEX {
		float x, y, z, rhw;
		D3DCOLOR d3dColor;
	};

	//Setup vertex buffer
	D3DVERTEX vVertices[2] = {
		{(float)x1, (float)y1, 0.0f, 0.0f, D3DCOLOR_ARGB(a, r, g, b)}, //Line source point
		{(float)x2, (float)y2, 0.0f, 0.0f, D3DCOLOR_ARGB(a, r, g, b)} //Line destination point
	};
	
	//Set custom vertex format (Vertex has x, y, z, rhw and color member)
	if (FAILED(m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE)))
		return false;

	//Draw line primitive
	return m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, sizeof(vVertices) / sizeof(D3DVERTEX), vVertices, sizeof(D3DVERTEX)) == D3D_OK;
}
//======================================================================

//======================================================================
bool CD3DRenderer::DrawImage(const size_t uiImageId, int x, int y)
{
	//Draw image from list

	//Check image ID for validity
	if (!IsValidImageId(uiImageId))
		return false;

	if (!this->m_vImages[uiImageId])
		return false;
	
	//Setup rect data
	RECT rDstRect;
	rDstRect.top = y;
	rDstRect.left = x;
	rDstRect.right = x + this->m_vImages[uiImageId]->d3dImageInfo.Width;
	rDstRect.bottom = y + this->m_vImages[uiImageId]->d3dImageInfo.Height;
	
	//Copy image surface to backbuffer
	return SUCCEEDED(m_pDevice->StretchRect(this->m_vImages[uiImageId]->pSurface, NULL, m_pBackBuffer, &rDstRect, D3DTEXF_NONE));
}
//======================================================================

//======================================================================
bool CD3DRenderer::StoreScreenshotToDisk(const std::string& szTargetFile)
{
	//Store a screenshot file with content of the frontbuffer

	if ((!szTargetFile.length()) || (!m_pDevice))
		return false;

	//Check if file name already exists
	if (FileExists(szTargetFile))
		return false;

	IDirect3DSurface9* pSurface = NULL;

	//Create surface
	if (FAILED(m_pDevice->CreateOffscreenPlainSurface(g_ScreenSize.x, g_ScreenSize.y, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, NULL)))
		return false;

	//Copy frontbuffer content into surface
	if (FAILED(m_pDevice->GetFrontBufferData(0, pSurface))) {
		pSurface->Release();
		return false;
	}

	//Save surface content to file with Bitmap format
	if (FAILED(D3DXSaveSurfaceToFileA(szTargetFile.c_str(), D3DXIFF_BMP, pSurface, NULL, NULL))) {
		pSurface->Release();
		return false;
	}

	//Release surface
	pSurface->Release();

	return true;
}
//======================================================================

//======================================================================
//D3D drawing interface functions

BOOL WINAPI D3DI_PrintText(LPCSTR lpszMessage)
{
	return g_pLog->Msg(lpszMessage) == true;
}

BOOL WINAPI D3DI_DrawText(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_oD3DRenderer.DrawString(lpszText, lpszFont, ucFontSizeW, ucFontSizeH, x, y, r, g, b, a) == true;
}

BOOL WINAPI D3DI_DrawBox(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_oD3DRenderer.DrawBox(x, y, w, h, iThickness, r, g, b, a) == true;
}

BOOL WINAPI D3DI_DrawFilledBox(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_oD3DRenderer.DrawFilledBox(x, y, w, h, r, g, b, a) == true;
}

BOOL WINAPI D3DI_DrawLine(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a)
{
	return g_oD3DRenderer.DrawLine(x1, y1, x2, y2, r, g, b, a) == true;
}

size_t WINAPI D3DI_LoadImage(LPCSTR lpszFileName)
{
	return g_oD3DRenderer.LoadImage(lpszFileName);
}

BOOL WINAPI D3DI_DrawImage(const size_t uiImageId, INT x, INT y)
{
	return g_oD3DRenderer.DrawImage(uiImageId, x, y) == true;
}
//======================================================================

//======================================================================
bool RegisterMenu(void)
{
	//Register menu

	static CzyVisualComponents::windowinfo_s sWindowInfo = { //Window info
		200, 200, //x y
		240, 87, //w h
		{
			//Colors
			{164, 190, 217, 255}, //Borders
			{170, 170, 170, 250}, //Headfill
			{200, 200, 200, 200}, //Bodyfill
			{50, 50, 50, 200},	//Cube
			{0, 0, 0, 250},	// Title
		},
		2, //Border size
		7, 15, //Font size (w h)
		0, //Font char dist
		5, //Font line dist
		5, //Box dist
		10, 10, //Cube size (w h)
		"Verdana" //Default font face
	};

	static CzyVisualComponents::drawinginterface_s sDrawingInterface = { //Drawing interface table
		D3DI_PrintText,
		D3DI_DrawText,
		D3DI_DrawBox,
		D3DI_DrawFilledBox,
		D3DI_DrawLine,
		D3DI_LoadImage,
		D3DI_DrawImage
	};

	static CzyVisualComponents::color32_s sTextColor = {
		0, 0, 0, 150
	};

	static CzyVisualComponents::color32_s sBoxColor = {
		255, 255, 225, 255
	};

	static CzyVisualComponents::color32_s sHoverColor = {
		220, 220, 220, 255
	};

	//Create the form
	g_pMainForm = CzyVisualComponents::CreateForm("frmMain", &sWindowInfo, &sDrawingInterface);
	if (!g_pMainForm)
		return false;

	//Set location data
	g_pMainForm->SetLocation(sWindowInfo.x, sWindowInfo.y);

	//Set menu title text
	g_pMainForm->SetText(PROGRAM_NAME);

	//Attach Checkbox
	if (!CzyVisualComponents::AttachCheckbox(g_pMainForm, "cbNameESP", 5, 40, &sTextColor, &sBoxColor, &sHoverColor, "Name ESP", "esp_name", g_pcvNameESP->bValue))
		return false;

	//Attach Checkbox
	if (!CzyVisualComponents::AttachCheckbox(g_pMainForm, "cbSteamIDESP", 5, 65, &sTextColor, &sBoxColor, &sHoverColor, "SteamID ESP", "esp_steamid", g_pcvSteamIDESP->bValue))
		return false;

	return true;
}
//======================================================================

//======================================================================
bool RegisterCVars(void)
{
	//Register script CVars

	#define REG_CVAR(r, n, t, v) r = g_oConfigInt.CCVar::Add(n, t, v); if (!r) return false;

	REG_CVAR(g_pcvNameESP, "esp_name", CzyConfigMgr::CCVar::CVAR_TYPE_BOOL, "0");
	REG_CVAR(g_pcvSteamIDESP, "esp_steamid", CzyConfigMgr::CCVar::CVAR_TYPE_BOOL, "0");
	REG_CVAR(g_pcvMenuKey, "key_menu", CzyConfigMgr::CCVar::CVAR_TYPE_INT, "73");

	return true;
}
//======================================================================

//======================================================================
void WINAPI Event_EndScene(void)
{
	//EndScene event function
	
	if (!pD3DDeviceFromGame) {
		//Get device pointer
		pD3DDeviceFromGame = GetDeviceFromShader();
		if (!pD3DDeviceFromGame)
			ExitError("pD3DDeviceFromGame is NULL");

		g_pLog->MsgFmt("IDirect3DDevice9 pointer: %p", pD3DDeviceFromGame);
	}

	//Initializations on first call
	static bool bOnce = false;
	if (!bOnce) {
		bOnce = true;

		//Initialize d3d renderer
		if (!g_oD3DRenderer.Initialize(pD3DDeviceFromGame, "images"))
			ExitError("CD3DRenderer::Initialize() failed");

		//Set script base directory
		g_oConfigInt.SetScriptDirectory(g_szModuleDir);

		//Register all CVars
		if (!RegisterCVars()) {
			ExitError("RegisterCVars() failed");
		}

		//Initialize input manager
		if (!g_InputMgr.Initialize(5.0f, GAME_TARGET_WINDOW)) {
			ExitError("CInputMgr::Initialize() failed: %d", GetLastError());
		}

		//Initialize VisComps
		if (!CzyVisualComponents::SetConfigInt(&g_oConfigInt))
			ExitError("CzyVisualComponents::SetConfigInt() failed");

		//Execute default config file
		if (!g_oConfigInt.Execute(DEFAULT_SCRIPT_FILE)) {
			g_pLog->Warning("Failed to execute script file \'" DEFAULT_SCRIPT_FILE "\'");
		}

		//Register menu
		if (!RegisterMenu())
			ExitError("RegisterMenu() menu failed");

		g_pLog->Msg("Graphical user interface has been created");
	}

	//Handle menu stuff if currently shown
	if ((g_bMenuToggle) && (g_pMainForm)) {
		//Process and draw stuff
		g_pMainForm->Process();
		g_pMainForm->Draw();

		//Draw cursor
		#define MENU_CURSOR_LENGTH 10
		#define MENU_CURSOR_SIZE 2
		int iCurCursorX = g_InputMgr.GetMouseX();
		int iCurCursorY = g_InputMgr.GetMouseY();
		g_oD3DRenderer.DrawFilledBox(iCurCursorX - MENU_CURSOR_LENGTH / 2, iCurCursorY, MENU_CURSOR_LENGTH + MENU_CURSOR_SIZE, MENU_CURSOR_SIZE, 255, 0, 0, 255);
		g_oD3DRenderer.DrawFilledBox(iCurCursorX, iCurCursorY - MENU_CURSOR_LENGTH / 2, MENU_CURSOR_SIZE, MENU_CURSOR_LENGTH + MENU_CURSOR_SIZE, 255, 0, 0, 255);
	}
}
//======================================================================

//======================================================================
__declspec(naked) HRESULT WINAPI Hook_EndScene(void)
{
	//IDirect3DDevice9::EndScene() hook implementation

	__asm {
		//Execute original instructions (last two operations do setup the stack frame pointer)
		mov EDI, EDI;
		push EBP;
		mov EBP, ESP;

		//Backup general registers on stack
		pushad;

		//Call event function
		call Event_EndScene;

		//Restore general registers from stack
		popad;

		jmp dword ptr[pfnOrigEndScene]; //Proceed with original method execution
	}
}
//======================================================================

//======================================================================
bool HookEndScene(void)
{
	//Hook IDirect3DDevice9::EndScene() method

	//How to get method address:
	//	- Breakpoint at d3d9.dll!Direct3DCreate9()
	//	- Step until ret instruction
	//	- Grab IDirect3D9 pointer from EAX register
	//	- Breakpoint at IDirect3D9::CreateDevice() (grab address from VFT)
	//	- Stept until ret instruction
	//	- Grab IDirect3DDevice9 pointer from stack segment

	#define EC_OFFSET 0x2279F

	//Get handle to d3d9.dll module
	HMODULE hD3DDll = GetModuleHandle(L"d3d9.dll");
	if (!hD3DDll)
		return false;

	//Calc absolute method address
	DWORD dwFuncAddr = (DWORD)hD3DDll + EC_OFFSET;

	//Hook method
	pfnOrigEndScene = (TpfnOrigEndScene)Detour((LPVOID)dwFuncAddr, &Hook_EndScene, 0x05);

	return pfnOrigEndScene != NULL;
}
//======================================================================

//======================================================================
IDirect3DDevice9* GetDeviceFromShader(void)
{
	//Grab interface instance address from shaderapidx9.dll

	//How to get IDirect3DDevice9 pointer dynamically (shaderapidx9.dll context)
	//	- Breakpoint at IDirect3DDevice9::EndScene()
	//	- Follow return address
	//	- Check out the following disassembly
	//	-	100CA735   A1 486E1A10      MOV EAX,DWORD PTR DS:[101A6E48]	; Copy dword at address to EAX: It's the start address of the IDirect3DDevice9 interface instance
	//	-	100CA73A   50               PUSH EAX	; Push EAX onto stack
	//	-	100CA73B   8B08             MOV ECX,DWORD PTR DS:[EAX]	; Copy VFT address from EAX to ECX
	//	-	100CA73D   FF91 A8000000    CALL DWORD PTR DS:[ECX+A8] //Call IDirect3DDevice9::EndScene() from VFT (base(ECX) + offset(0xA8))

	#define ID3DDEV9_OFFSET 0xDA736

	HMODULE hShader = GetModuleHandle(L"shaderapidx9.dll");
	if (!hShader)
		return NULL;

	//Get instance address
	return **(IDirect3DDevice9***)((DWORD)hShader + ID3DDEV9_OFFSET);
}
//======================================================================
