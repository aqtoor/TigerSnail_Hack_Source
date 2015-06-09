#ifndef _DRAWING_H
#define _DRAWING_H

#include "gamespec.h"
#include "vars.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.1
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	drawing.h: Game Engine drawing interface
*/

//======================================================================
enum {
	FNI_DrawSetColor = 6,
	FNI_DrawFilledRect = 8,
	FNI_DrawLine = 11,
	FNI_DrawSetTextColor = 14,
	FNI_DrawSetTextPos = 16,
	FNI_DrawPrintText = 18
};
//======================================================================

//======================================================================
struct vgui_surface_s {
	_ISurface* pSurface;
	DWORD* pdwVFTable;
	HANDLE hFont;
};
//======================================================================

//======================================================================
bool SetupDrawingInterface(client_import_s* pImport, const char* szFontName, const int iFontTall);
void DrawString(int x, int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, char* lpszString, ...);
void DrawFill(int x, int y, int w, int h, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
void DrawBox(int x, int y, int w, int h, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, unsigned char bordersize);
//======================================================================

#endif
