#ifndef _SNAKE_H
#define _SNAKE_H

#include "includes.h"
#include <inputsystem\buttoncode.h>

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.5
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: snake.h: Snake game interface
*/

namespace Snake {
	//======================================================================
	enum GameStopReason_e {
		GSR_ABORTEDBYUSER,
		GSR_GAMEOVER
	};

	enum SnakeDirection_e {
		SD_NONE = -1,
		SD_LEFT,
		SD_RIGHT,
		SD_UP,
		SD_DOWN
	};
	//======================================================================

	//======================================================================
	struct color32_s {
		unsigned char r, g, b, a; //24-Bit color values with 8-bit alpha channel
	};

	struct drawinginterface_s {
		BOOL (WINAPI *TpfnPrintText)(LPCSTR lpszMessage);
		BOOL (WINAPI *TpfnDrawText)(LPCSTR lpszText, LPCSTR lpszFont, BYTE ucFontSizeW, BYTE ucFontSizeH, INT x, INT y, BYTE r, BYTE g, BYTE b, BYTE a);
		BOOL (WINAPI *TpfnDrawBox)(INT x, INT y, INT w, INT h, INT iThickness, BYTE r, BYTE g, BYTE b, BYTE a);
		BOOL (WINAPI *TpfnDrawFilledBox)(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b, BYTE a);
		BOOL (WINAPI *TpfnDrawLine)(INT x1, INT y1, INT x2, INT y2, BYTE r, BYTE g, BYTE b, BYTE a);
		size_t (WINAPI *TpfnLoadImage)(LPCSTR lpszFileName);
		BOOL (WINAPI *TpfnDrawImage)(const size_t uiImageId, INT x, INT y);
	};

	struct windowinfo_s {
		int x; //X postion
		int y; //Y postion
		unsigned char fontSizeW; //Width of a font character
		unsigned char fontSizeH; //Height of a font character
		unsigned char fontSizeDist; //Distance between each character in a line
		unsigned char fontLineDist; //Distance between each line
		unsigned char borderSize; //Size of the borders
		unsigned char textToBorderDist; //Distance between content and the window border
		unsigned char cubeWidth; //Title cube width
		unsigned char cubeHeight; //Title cube height
		unsigned char LabelToValueDist; //Distance between labels and values
		unsigned char offsetFromRightBorder; //This value is needed to draw the values left to the right border
		color32_s borders; //Color of borders
		color32_s headfill; //Color of title area
		color32_s bodyfill; //Color of body area
		color32_s title; //Title text color
		color32_s cube; //Title cube
		color32_s snakehead; //Snake head
		color32_s snakebody; //Snake body
		color32_s snakefood; //Snake food
		char szDefaultFont[MAX_PATH]; //Default font
	};

	struct snakebox_s {
		int iCubeID;
		SnakeDirection_e sdDirection;
	};
	//======================================================================

	//======================================================================
	class CSnake {
	private:
		std::string m_szTitle; //Snake title
		std::string m_szTitleFont; //Snake title font

		drawinginterface_s m_sDrawingInterface; //Drawing interface
		windowinfo_s m_wndInfo; //Window information

		bool m_bReady; //Whether component is ready or not

		BYTE m_ucTitleFontSizeW, m_ucTitleFontSizeH; //Title font size

		int m_iButtonX; //Button X-pos
		int m_iButtonY; //Button Y-pos
		int m_iButtonW; //Button Width
		int m_iButtonH; //Button Height

		std::string m_szButtonText; //Button text

		bool m_bGamePaused; //Indicator if game is paused or not

		int m_iSnakeCubes; //Amount of snake cubes
		int m_iSnakeCubeSize; //Size of each snake cube

		bool m_bGameStarted; //Indicator if game has started
		GameStopReason_e m_gsrLastStopReason; //Stop reason of last game

		std::vector<snakebox_s> m_vSnake; //The snake
		snakebox_s m_sbFood; //The food

		DWORD m_dwVelocity; //Snake velocity
		DWORD m_dwTimerCur; //Timer current
		DWORD m_dwTimerInit; //Timer last

		SnakeDirection_e m_sdLastDirection; //last direction

		size_t m_uiLastScore; //Last score

		unsigned int GetTitleBoxWidth(void);

		unsigned int GetWindowWidth();
		unsigned int GetWindowHeight();

		bool GetFieldCubeById(int iId, int& rX, int& rY);
		int GetFieldMaxCubeId();
	public:
		CSnake() : m_bReady(false), m_bGameStarted(false), m_dwVelocity(70), m_sdLastDirection(SD_NONE), m_uiLastScore(0), m_bGamePaused(false), m_gsrLastStopReason(GSR_ABORTEDBYUSER) {}
		~CSnake() { }

		bool Initialize(const drawinginterface_s& rDrawingInterface, const windowinfo_s& rWindowInfo, const int iSnakeCubes, const int iSnakeCubeSize, const std::string& szTitleFont);
		void SetVelocity(DWORD dwVelocity) { this->m_dwVelocity = dwVelocity; if (this->m_dwVelocity < 10) this->m_dwVelocity = 10; else if (this->m_dwVelocity > 200) this->m_dwVelocity = 200; }
		void CenterToScreen(int w, int h);

		void Draw(void);
		void KeyEvent(INT iKey, BOOL bDown);
		void MouseEvent(INT x, INT y, INT iMouseKey, BOOL bDown);

		void StartGame(void);
		void SetHeadDirection(SnakeDirection_e sdDirection);
		bool FoodWouldCollideOnSpawn(int iCubeID);
		void ProcessGame(void);
		void StopGame(GameStopReason_e gsrReason);

		bool IsReady(void) { return this->m_bReady; } //If component is ready to be used
	};
	//======================================================================
}

#endif
