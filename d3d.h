#ifndef _D3DRENDERER_H
#define _D3DRENDERER_H

#include "includes.h"
#include <d3d9.h>
#include <d3dx9core.h>
#include <DxErr.h>
#include "viscomps.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	d3d.h: D3D9 related interface
*/

//======================================================================
#define INVALID_LIST_ID std::string::npos
//======================================================================

//======================================================================
typedef HRESULT (WINAPI* TpfnOrigEndScene)(DWORD dwArg);
//======================================================================

//======================================================================
struct d3dfont_s {
	LPD3DXFONT pFont; //Pointer to font object
	CHAR szFontName[LF_FACESIZE]; //Font name string
	unsigned char ucFontSizeW, ucFontSizeH; //Font size
};

struct d3dimage_s {
	std::string szFile; //Full file name
	LPDIRECT3DSURFACE9 pSurface; //Surface data pointer
	D3DXIMAGE_INFO d3dImageInfo; //Image info
};
//======================================================================

//======================================================================
class CD3DRenderer {
private:
	std::string m_szImagePath;

	IDirect3DDevice9* m_pDevice;
	IDirect3DSurface9* m_pBackBuffer;

	std::vector<d3dfont_s*> m_vFonts;
	std::vector<d3dimage_s*> m_vImages;

	d3dfont_s* GetExistingFont(const std::string& szFontName, unsigned char ucFontSizeW, unsigned char ucFontSizeH);
	d3dfont_s* RegisterFont(const std::string& szFontName, unsigned char ucFontSizeW, unsigned char ucFontSizeH);

	friend void WINAPI Event_EndScene(void);

	inline bool IsValidImageId(const size_t uiImageId) { return (uiImageId != INVALID_LIST_ID) && (uiImageId < this->m_vImages.size()); }
public:
	CD3DRenderer() : m_pDevice(NULL), m_pBackBuffer(NULL) { }
	~CD3DRenderer() { this->Clear(); }

	bool Initialize(IDirect3DDevice9* pDevice, const std::string& szAbsImagePath);
	void Clear(void);

	size_t LoadImage(const std::string& szImage);

	bool DrawString(const std::string& szText, const std::string& szFont, unsigned char ucFontSizeW, unsigned char ucFontSizeH, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool DrawBox(int x, int y, int w, int h, int iThickness, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool DrawFilledBox(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool DrawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	bool DrawImage(const size_t uiImageId, int x, int y);

	bool StoreScreenshotToDisk(const std::string& szTargetFile);
};
//======================================================================

//======================================================================
bool HookEndScene(void);
IDirect3DDevice9* GetDeviceFromShader(void);
//======================================================================

//======================================================================
extern CD3DRenderer g_oD3DRenderer;
extern CzyVisualComponents::CForm* g_pMainForm;
//======================================================================

#endif
