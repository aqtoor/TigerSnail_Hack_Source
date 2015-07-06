#include "drawing.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	drawing.cpp: Game Engine drawing implementation
*/

//======================================================================
vgui_surface_s g_Surface;
bool bReady = false;
//======================================================================

//======================================================================
bool SetupDrawingInterface(client_import_s* pImport, const char* szFontName, const int iFontTall)
{
	//Initialize drawing

	bReady = false;

	if ((!pImport) || (!szFontName))
		return false;

	//Copy object pointer and vft pointer
	g_Surface.pSurface = (_ISurface*)pImport->pInterfaceObject;
	g_Surface.pdwVFTable = pImport->pdwVFTPointer;
	
	//Create font
	g_Surface.hFont = g_Surface.pSurface->CreateFont();
	if (!g_Surface.hFont)
		return false;
	
	//Assign font data
	if (!g_Surface.pSurface->SetFontGlyphSet(g_Surface.hFont, szFontName, iFontTall, 150, 0, 0, _ISurface::FONTFLAG_OUTLINE))
		return false;
	
	bReady = true;

	return true;
}
//======================================================================

//======================================================================
void DrawString(int x, int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, char* lpszString, ...)
{
	//Draw a text string on screen

	if ((!bReady) || (!lpszString))
		return;

	char szFmtBuffer[1024];

	//Format output buffer
	va_list list;
	va_start(list, lpszString);
	vsprintf_s(szFmtBuffer, lpszString, list);
	va_end(list);

	//Convert to wide char string
	wchar_t wChar[sizeof(szFmtBuffer) + 1] = {0};
	int iWideStrLen = mbstowcs(wChar, szFmtBuffer, strlen(szFmtBuffer));
	if (!iWideStrLen)
		return;

	//Set current font
	g_Surface.pSurface->DrawSetTextFont(g_Surface.hFont);

	//Set current color
	g_Surface.pSurface->DrawSetTextColor(red, green, blue, alpha);
	
	//Set text position
	g_Surface.pSurface->DrawSetTextPos(x, y);

	//Draw the text
	g_Surface.pSurface->DrawPrintText(wChar, iWideStrLen, 0);
}
//======================================================================

//======================================================================
void DrawFill(int x, int y, int w, int h, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	//Draw a filled area

	if (!bReady)
		return;

	//Set drawing color
	g_Surface.pSurface->DrawSetColor(red, green, blue, alpha);

	//Draw the filled rect
	g_Surface.pSurface->DrawFilledRect(x, y, w, h);
}
//======================================================================

//======================================================================
void DrawBox(int x, int y, int w, int h, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, unsigned char bordersize)
{
	//Draw a non-filled box

	if (!bReady)
		return;

	//Set drawing color
	g_Surface.pSurface->DrawSetColor(red, green, blue, alpha);
	
	//Draw the box
	g_Surface.pSurface->DrawFilledRect(x, y, w, bordersize);
	g_Surface.pSurface->DrawFilledRect(x, y, bordersize, h);
	g_Surface.pSurface->DrawFilledRect(x + w, y, bordersize, h);
	g_Surface.pSurface->DrawFilledRect(x, y + h, w, bordersize);
}
//======================================================================
