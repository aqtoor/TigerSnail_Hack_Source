#include "snake.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.4
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: snake.cpp: Snake game implementations
*/

namespace Snake {
	//======================================================================
	bool CSnake::Initialize(const drawinginterface_s& rDrawingInterface, const windowinfo_s& rWindowInfo, const int iSnakeCubes, const int iSnakeCubeSize, const std::string& szTitleFont)
	{
		//Initialize component data

		if ((!iSnakeCubes) || (!iSnakeCubeSize) || (!szTitleFont.length()))
			return false;

		this->m_sDrawingInterface = rDrawingInterface;
		this->m_wndInfo = rWindowInfo;
		this->m_iSnakeCubes = iSnakeCubes;
		this->m_iSnakeCubeSize = iSnakeCubeSize;
		this->m_szTitleFont = szTitleFont;

		this->m_bReady = true;

		return true;
	}
	//======================================================================

	//======================================================================
	void CSnake::CenterToScreen(int w, int h)
	{
		//Center window to screen

		if ((!w) || (!h))
			return;

		this->m_wndInfo.x = w / 2 - this->GetWindowWidth() / 2;
		this->m_wndInfo.y = h / 2 - this->GetWindowHeight() / 2;
	}
	//======================================================================

	//======================================================================
	unsigned int CSnake::GetTitleBoxWidth(void)
	{
		//Calculate box width

		return GetWindowWidth();
	}
	//======================================================================

	//======================================================================
	void CSnake::Draw(void)
	{
		//Draw this Snake

		if (!IsReady())
			return;

		#define DRAW_TEXT m_sDrawingInterface.TpfnDrawText
		#define DRAW_BOX m_sDrawingInterface.TpfnDrawBox
		#define DRAW_AREA m_sDrawingInterface.TpfnDrawFilledBox
		#define FD_HEIGHT (m_wndInfo.fontLineDist + m_ucTitleFontSizeH + m_wndInfo.fontLineDist)
		#define BORDER_SIZE m_wndInfo.borderSize
		#define TEXT_TO_BORDER_DIST m_wndInfo.textToBorderDist

		//Draw borders and the areas
		DRAW_BOX(m_wndInfo.x + this->m_iSnakeCubeSize, m_wndInfo.y, GetTitleBoxWidth() - BORDER_SIZE - this->m_iSnakeCubeSize, m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, m_wndInfo.borderSize, m_wndInfo.borders.r, m_wndInfo.borders.g, m_wndInfo.borders.b, m_wndInfo.borders.a); //Title box
		DRAW_AREA(m_wndInfo.x + this->m_iSnakeCubeSize + m_wndInfo.borderSize, m_wndInfo.y + m_wndInfo.borderSize, GetTitleBoxWidth() - m_wndInfo.borderSize * 2 - this->m_iSnakeCubeSize, m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2 - m_wndInfo.borderSize, m_wndInfo.headfill.r, m_wndInfo.headfill.g, m_wndInfo.headfill.b, m_wndInfo.headfill.a); //Head fill

		#define CUBE_WIDTH m_wndInfo.cubeWidth
		#define CUBE_HEIGHT m_wndInfo.cubeHeight

		//Draw title cube
		DRAW_AREA(m_wndInfo.x + this->m_iSnakeCubeSize + CUBE_WIDTH / 2, m_wndInfo.y + m_wndInfo.borderSize + ((m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2) / 2) - (CUBE_HEIGHT / 2), CUBE_WIDTH, CUBE_HEIGHT, m_wndInfo.cube.r, m_wndInfo.cube.g, m_wndInfo.cube.b, m_wndInfo.cube.a);

		//Draw title

		if (this->m_bGameStarted) {
			m_uiLastScore = this->m_vSnake.size() - 6;
		}

		this->m_szTitle = "Snake (Score: " + std::to_string((_ULonglong)m_uiLastScore) + ")";
	
		DRAW_TEXT(m_szTitle.c_str(), m_szTitleFont.c_str(), m_ucTitleFontSizeW, m_ucTitleFontSizeH, m_wndInfo.x + this->m_iSnakeCubeSize + CUBE_WIDTH / 2 + m_wndInfo.fontSizeDist + CUBE_WIDTH + CUBE_WIDTH / 2, m_wndInfo.y + m_wndInfo.borderSize + TEXT_TO_BORDER_DIST, m_wndInfo.title.r, m_wndInfo.title.g, m_wndInfo.title.b, m_wndInfo.title.a);
	

		//Draw field cubes
		for (int i = 0; i < this->m_iSnakeCubes-1; i++) {
			DRAW_AREA(m_wndInfo.x + ((i+1) * this->m_iSnakeCubeSize), m_wndInfo.y + 5 + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, this->m_iSnakeCubeSize, this->m_iSnakeCubeSize, m_wndInfo.bodyfill.r, m_wndInfo.bodyfill.g, m_wndInfo.bodyfill.b, m_wndInfo.bodyfill.a);
		
			for (int j = 0; j < this->m_iSnakeCubes; j++) {
				DRAW_AREA(m_wndInfo.x + ((i+1) * this->m_iSnakeCubeSize), m_wndInfo.y + 5 + (j * this->m_iSnakeCubeSize) + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, this->m_iSnakeCubeSize, this->m_iSnakeCubeSize, m_wndInfo.bodyfill.r, m_wndInfo.bodyfill.g, m_wndInfo.bodyfill.b, m_wndInfo.bodyfill.a);
			}
		}

		//Draw info if game is not running
		if (!this->m_bGameStarted) {
			char szGSInfo[MAX_PATH];
			strcpy_s(szGSInfo, (m_gsrLastStopReason == GSR_GAMEOVER) ? "Game Over" : "Game Stopped");
			DRAW_TEXT(szGSInfo, m_szTitleFont.c_str(), m_ucTitleFontSizeW, m_ucTitleFontSizeH, m_wndInfo.x + ((GetWindowWidth() / 2) - ((strlen(szGSInfo) * m_wndInfo.fontSizeW) / 2)), m_wndInfo.y + 5 + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2 + (GetWindowHeight() / 2 - m_wndInfo.fontSizeH), m_wndInfo.title.r, m_wndInfo.title.g, m_wndInfo.title.b, m_wndInfo.title.a);
		}

		if (this->m_bGameStarted) {
			//Draw snake
			for (size_t i = 0; i < m_vSnake.size(); i++) {
				int iXPos, iYPos;
				if (GetFieldCubeById(m_vSnake[i].iCubeID, iXPos, iYPos)) {
					DRAW_AREA(m_wndInfo.x + iXPos, m_wndInfo.y + 5 + iYPos + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, this->m_iSnakeCubeSize, this->m_iSnakeCubeSize, (i == 0) ? m_wndInfo.snakehead.r : m_wndInfo.snakebody.r, (i == 0) ? m_wndInfo.snakehead.g : m_wndInfo.snakebody.g, (i == 0) ? m_wndInfo.snakehead.b : m_wndInfo.snakebody.b, (i == 0) ? m_wndInfo.snakehead.a : m_wndInfo.snakebody.a);
				}
			}

			//Draw food
			int iXPos, iYPos;
			if (GetFieldCubeById(m_sbFood.iCubeID, iXPos, iYPos)) {
				DRAW_AREA(m_wndInfo.x + iXPos, m_wndInfo.y + 5 + iYPos + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, this->m_iSnakeCubeSize, this->m_iSnakeCubeSize, m_wndInfo.snakefood.r, m_wndInfo.snakefood.g, m_wndInfo.snakefood.b, m_wndInfo.snakefood.a);
			}
		}

		//Draw button

		m_iButtonX = m_wndInfo.x + this->m_iSnakeCubeSize;
		m_iButtonY = m_wndInfo.y + m_wndInfo.borderSize + 10 + GetWindowHeight() + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2;
		m_iButtonW = GetTitleBoxWidth() - m_wndInfo.borderSize * 2;
		m_iButtonH = m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2 - m_wndInfo.borderSize;

		DRAW_BOX(m_wndInfo.x + this->m_iSnakeCubeSize, m_wndInfo.y + 10 + GetWindowHeight() + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, GetTitleBoxWidth() - BORDER_SIZE - this->m_iSnakeCubeSize, m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, m_wndInfo.borderSize, m_wndInfo.borders.r, m_wndInfo.borders.g, m_wndInfo.borders.b, m_wndInfo.borders.a);
		DRAW_AREA(m_wndInfo.x + this->m_iSnakeCubeSize + m_wndInfo.borderSize, m_wndInfo.y + m_wndInfo.borderSize + 10 + GetWindowHeight() + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, GetTitleBoxWidth() - m_wndInfo.borderSize * 2 - this->m_iSnakeCubeSize, m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2 - m_wndInfo.borderSize, m_wndInfo.headfill.r, m_wndInfo.headfill.g, m_wndInfo.headfill.b, m_wndInfo.headfill.a);

		if (this->m_bGameStarted) {
			if (this->m_bGamePaused) {
				m_szButtonText = "Continue Game";
			} else {
				m_szButtonText = "Pause Game";
			}
		} else {
			m_szButtonText = "Start Game";
		}

		DRAW_TEXT(m_szButtonText.c_str(), m_szTitleFont.c_str(), m_ucTitleFontSizeW, m_ucTitleFontSizeH, m_wndInfo.x + (GetTitleBoxWidth() / 2) - ((m_szButtonText.length() * m_wndInfo.fontSizeW) / 2), m_wndInfo.y + m_wndInfo.borderSize + 12 + GetWindowHeight() + m_wndInfo.fontSizeH + BORDER_SIZE * 2 + TEXT_TO_BORDER_DIST * 2, m_wndInfo.title.r, m_wndInfo.title.g, m_wndInfo.title.b, m_wndInfo.title.a);
	}
	//======================================================================

	//======================================================================
	unsigned int CSnake::GetWindowWidth()
	{
		//Snake window size is amount of snake cubes multiplied with the snake cube size

		return this->m_iSnakeCubes * this->m_iSnakeCubeSize;
	}
	//======================================================================

	//======================================================================
	unsigned int CSnake::GetWindowHeight()
	{
		//Snake window size is amount of snake cubes multiplied with the snake cube size

		return this->m_iSnakeCubes * this->m_iSnakeCubeSize;
	}
	//======================================================================

	//======================================================================
	int CSnake::GetFieldMaxCubeId()
	{
		//The last cube ID is the amount of cubes. 'iSnakeCubes' stands for the horizontal and vertical cube amount

		return this->m_iSnakeCubes * this->m_iSnakeCubes;
	}
	//======================================================================

	//======================================================================
	bool CSnake::GetFieldCubeById(int iId, int& rX, int& rY)
	{
		//Get X and Y position of the cube by ID

		if ((iId <= 0) || (iId > GetFieldMaxCubeId()))
			return false;

		int iCubePosX = 0; //X-position of the cube
		int iCubePosY = 0; //Y-position of the cube

		iCubePosX = iId % this->m_iSnakeCubes; //Get division rest of ID with amount of snake cubes
		if (iCubePosX == 0) //If rest zero
			iCubePosX = this->m_iSnakeCubes; //Then the last cube of horizontal line is meant

		iCubePosY = iId / this->m_iSnakeCubes; //Get quotient and ignore rest

		//Calculate relative positions
		rX = iCubePosX * this->m_iSnakeCubeSize;
		rY = iCubePosY * this->m_iSnakeCubeSize;

		return true;
	}
	//======================================================================

	//======================================================================
	void CSnake::StartGame(void)
	{
		//Initialize game start

		//Clear list
		m_vSnake.clear();

		//Set start cube
		snakebox_s sbStartBox;
		sbStartBox.iCubeID = 5;
		sbStartBox.sdDirection = SD_RIGHT;

		//Add initial start cube (snake head)
		m_vSnake.push_back(sbStartBox);

		//Add initial snake body
		for (int i = 0; i < 5; i++) {
			snakebox_s sbStartBox;
			sbStartBox.iCubeID = 4 - i;
			sbStartBox.sdDirection = SD_NONE;
			m_vSnake.push_back(sbStartBox);
		}

		//Set initial food
		m_sbFood.iCubeID = (rand() % (GetFieldMaxCubeId() - 1)) + 1; //Set pseudo-random ID
		if (m_sbFood.iCubeID <= 5)
			m_sbFood.iCubeID = 6;
		m_sbFood.sdDirection = SD_NONE; //Doesn't move

		//Initialize timer
		this->m_dwTimerInit = this->m_dwTimerCur = GetTickCount();

		this->m_bGameStarted = true; //Enable game calculations
	}
	//======================================================================

	//======================================================================
	void CSnake::SetHeadDirection(SnakeDirection_e sdDirection)
	{
		//Set head direction

		if ((!this->m_bGameStarted) || (!m_vSnake.size())) //Check if game calculations are enabled and if at least the snake head exists
			return;

		//Check if new direction is allowed in the context of the current direction. E.g. if moving upwards then setting to up is pointless and if setting to down it's not allowed because it would collide with itself
		if ((sdDirection == SD_UP) && ((m_vSnake[0].sdDirection == SD_UP) || (m_vSnake[0].sdDirection == SD_DOWN))) {
			return;
		} else if ((sdDirection == SD_DOWN) && ((m_vSnake[0].sdDirection == SD_DOWN) || (m_vSnake[0].sdDirection == SD_UP))) {
			return;
		} else if ((sdDirection == SD_LEFT) && ((m_vSnake[0].sdDirection == SD_LEFT) || (m_vSnake[0].sdDirection == SD_RIGHT))) {
			return;
		} else if ((sdDirection == SD_RIGHT) && ((m_vSnake[0].sdDirection == SD_RIGHT) || (m_vSnake[0].sdDirection == SD_LEFT))) {
			return;
		}

		m_sdLastDirection = m_vSnake[0].sdDirection; //Save current direction
		m_vSnake[0].sdDirection = sdDirection; //Set new direction
	}
	//======================================================================

	//======================================================================
	bool CSnake::FoodWouldCollideOnSpawn(int iCubeID)
	{
		//Check if food would collide with snake when spawning

		for (size_t i = 0; i < m_vSnake.size(); i++) {
			if (m_vSnake[i].iCubeID == iCubeID)
				return true;
		}

		return false;
	}
	//======================================================================

	//======================================================================
	void CSnake::ProcessGame(void)
	{
		//Process snake game

		if ((!this->m_bGameStarted) || (!m_vSnake.size())) //Check if game calculations are enabled and if at least the snake head exists
			return;

		//Check if game is paused
		if (this->m_bGamePaused)
			return;

		this->m_dwTimerCur = GetTickCount(); //Update current timer
		if (this->m_dwTimerCur > this->m_dwTimerInit + this->m_dwVelocity) { //Check if time has elapsed
			this->m_dwTimerInit = GetTickCount(); //Reset initial timer

			//Update snake body cube IDs
			if (m_vSnake.size() > 1) {
				for (size_t i = m_vSnake.size()-1; i > 0; i--) {
					m_vSnake[i].iCubeID = m_vSnake[i-1].iCubeID;
				}
			}

			//Calculate next cube of snake head related to direction
			if (m_vSnake[0].sdDirection == SD_UP) { //Move up
				m_vSnake[0].iCubeID -= this->m_iSnakeCubes; //Subtract to go a line up
				if (m_vSnake[0].iCubeID <= 0) { //If snake has left the field then game is over
					StopGame(GSR_GAMEOVER);
					return;
				}
			} else if (m_vSnake[0].sdDirection == SD_DOWN) { //Move down
				m_vSnake[0].iCubeID += this->m_iSnakeCubes; //Add to go a line down
				if (m_vSnake[0].iCubeID > GetFieldMaxCubeId()) { //If snake has left the field then game is over
					StopGame(GSR_GAMEOVER);
					return;
				}
			} else if (m_vSnake[0].sdDirection == SD_LEFT) { //Move left
				m_vSnake[0].iCubeID--; //Decrement to go to previous cube
				if ((m_vSnake[0].iCubeID % this->m_iSnakeCubes) == 0) { //If snake has left the field then game is over
					StopGame(GSR_GAMEOVER);
					return;
				}
			} else if (m_vSnake[0].sdDirection == SD_RIGHT) { //Move right
				m_vSnake[0].iCubeID++; //Increment to go to next cube
				if ((m_vSnake[0].iCubeID % this->m_iSnakeCubes) == 0) {//If snake has left the field then game is over
					StopGame(GSR_GAMEOVER);
					return;
				}
			}

			//Check if snake has collided with its body
			for (size_t i = 1; i < m_vSnake.size(); i++) {
				if (m_vSnake[0].iCubeID == m_vSnake[i].iCubeID) {
					StopGame(GSR_GAMEOVER);
					return;
				}
			}
		
			//Check if collided with food
			if (m_vSnake[0].iCubeID == this->m_sbFood.iCubeID) {
				//Extend snake body
				snakebox_s sbTail;
				sbTail.iCubeID = m_vSnake[m_vSnake.size()-1].iCubeID;
				sbTail.sdDirection = SD_NONE;
				m_vSnake.push_back(sbTail);

				//Calculate new pseudo-random food position
				int iNewID = (rand() % GetFieldMaxCubeId()) + 1;
			
				//As long as food would collide with snake calculate new pseudo-random position
				while (FoodWouldCollideOnSpawn(iNewID)) {
					iNewID = (rand() % GetFieldMaxCubeId()) + 1;
				}

				this->m_sbFood.iCubeID = iNewID; //Save new cube ID
			}
		}
	}
	//======================================================================

	//======================================================================
	void CSnake::StopGame(GameStopReason_e gsrReason)
	{
		//Stop game

		if (!this->m_bGameStarted)
			return;

		this->m_bGameStarted = false; //Disable game calculations
		this->m_gsrLastStopReason = gsrReason; //Save reason indicator
	
		//Clear list
		m_vSnake.clear();

		m_sbFood.iCubeID = 0; //Clear cube ID
	}
	//======================================================================

	//======================================================================
	void CSnake::KeyEvent(INT iKey, BOOL bDown)
	{
		//Handle key events

		if ((!this->m_bGameStarted) || (this->m_bGamePaused) || (!bDown))
			return;

		if (iKey == KEY_LEFT)
			SetHeadDirection(SD_LEFT);
		else if (iKey == KEY_RIGHT)
			SetHeadDirection(SD_RIGHT);
		else if (iKey == KEY_UP)
			SetHeadDirection(SD_UP);
		else if (iKey == KEY_DOWN)
			SetHeadDirection(SD_DOWN);
	}
	//======================================================================

	//======================================================================
	void CSnake::MouseEvent(INT x, INT y, INT iMouseKey, BOOL bDown)
	{
		//Handle mouse events

		if ((iMouseKey == 1) && (!bDown)) {
			if (((x >= m_iButtonX) && (x <= m_iButtonX + m_iButtonW)) && ((y >= m_iButtonY) && (y <= m_iButtonY + m_iButtonH))) { //Check if in button range
				if (!this->m_bGameStarted) {
					//Start game if game is stopped
					StartGame();
				} else {
					//Toggle pause flag
					this->m_bGamePaused = !this->m_bGamePaused;
				}
			}
		}
	}
	//======================================================================
}
