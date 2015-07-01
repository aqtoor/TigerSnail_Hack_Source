#include "players.h"
#include "vars.h"
#include "d3d.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.3
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: players.cpp: Player manager specific implementations
*/

//======================================================================
void CPlayerMgr::Reset(void)
{
	//Reset data

	memset(&this->m_sPlayerData, 0x00, sizeof(this->m_sPlayerData));
}
//======================================================================

//======================================================================
bool CPlayerMgr::UpdateSlot(const EPLAYERID iEntIndex)
{
	//Update player slot (gateway)

	if (!VALID_EID(iEntIndex))
		return false;

	register MPLAYERID uiEntIndex = EID_TO_MID(iEntIndex);

	return this->UpdateSlot(uiEntIndex);
}
//======================================================================

//======================================================================
bool CPlayerMgr::UpdateSlot(const MPLAYERID uiEntIndex)
{
	//Update player slot

	//Validate index
	if (!VALID_MID(uiEntIndex))
		return false;

	//Clear indicators
	this->m_sPlayerData[uiEntIndex].bSlotUsed = false;
	this->m_sPlayerData[uiEntIndex].bHudInfoAvailable = false;
	this->m_sPlayerData[uiEntIndex].pEntity = NULL;
	this->m_sPlayerData[uiEntIndex].bScreenPosValid = false;
	this->m_sPlayerData[uiEntIndex].bVisible = false;

	//Get pointer to client networkable instance
	IClientNetworkable* pClientNetworkable = g_pClientEntityList->GetClientNetworkable(MID_TO_EID(uiEntIndex));
	if (!pClientNetworkable)
		return false;

	//Get pointer to client unknown instance
	IClientUnknown* pClientUnknown = pClientNetworkable->GetIClientUnknown();
	if (!pClientUnknown)
		return false;

	//Update player engine data pointer and slot usage indicator
	this->m_sPlayerData[uiEntIndex].pEntity = (_C_BaseEntity*)pClientUnknown->GetBaseEntity();
	this->m_sPlayerData[uiEntIndex].bSlotUsed = this->m_sPlayerData[uiEntIndex].pEntity != NULL;
	this->m_sPlayerData[uiEntIndex].iEngineId = (this->m_sPlayerData[uiEntIndex].pEntity) ? this->m_sPlayerData[uiEntIndex].pEntity->m_iIndex : -1;

	if (this->m_sPlayerData[uiEntIndex].bSlotUsed) {
		//Flag if this is the local player
		this->m_sPlayerData[uiEntIndex].bIsLocal = this->m_sPlayerData[uiEntIndex].iEngineId == g_pEngineClient->GetLocalPlayer();

		//Save team value
		this->m_sPlayerData[uiEntIndex].iTeam = this->m_sPlayerData[uiEntIndex].pEntity->m_iTeamNum;

		if (this->IsPlaying(uiEntIndex)) { //If player entity is "alive" then handle related stuff
			//Get hud player info
			this->m_sPlayerData[uiEntIndex].bHudInfoAvailable = g_pEngineClient->GetPlayerInfo(MID_TO_EID(uiEntIndex), (player_info_t*)&this->m_sPlayerData[uiEntIndex].sHudInfo);
			
			//Update player weapon
			this->UpdateCurWeapon(uiEntIndex);
			
			//Calculate 2D screen position of origin
			if (!this->m_sPlayerData[uiEntIndex].bIsLocal) {
				//First try world position
				this->m_sPlayerData[uiEntIndex].bScreenPosValid = false;
				//this->m_sPlayerData[uiEntIndex].bScreenPosValid = this->CalcScreen(Vector(this->m_sPlayerData[uiEntIndex].pEntity->m_vecAbsOrigin[0], this->m_sPlayerData[uiEntIndex].pEntity->m_vecAbsOrigin[1], this->m_sPlayerData[uiEntIndex].pEntity->m_vecAbsOrigin[2]), this->m_sPlayerData[uiEntIndex].vScreenPos);
			}

			//Check if visible to local player
			this->m_sPlayerData[uiEntIndex].bVisible = this->IsVisible(uiEntIndex);
		}
	}

	return true;
}
//======================================================================

//======================================================================
inline size_t CPlayerMgr::Count(void)
{
	//Return amount of array entries

	return _countof(this->m_sPlayerData);
}
//======================================================================

//======================================================================
player_data_s* CPlayerMgr::Slot(const EPLAYERID iEntIndex)
{
	//Get pointer to player data from slot

	if (!VALID_EID(iEntIndex))
		return NULL;

	return &this->m_sPlayerData[EID_TO_MID(iEntIndex)];
}
//======================================================================

//======================================================================
player_data_s* CPlayerMgr::Slot(const MPLAYERID uiEntIndex)
{
	//Get pointer to player data from slot

	if (!VALID_MID(uiEntIndex))
		return NULL;

	return &this->m_sPlayerData[uiEntIndex];
}
//======================================================================

//======================================================================
player_data_s* CPlayerMgr::Local(void)
{
	//Find data of local player

	for (size_t i = 0; i < this->Count(); i++) {
		if ((this->m_sPlayerData[i].bSlotUsed) && (this->m_sPlayerData[i].bIsLocal))
			return &this->m_sPlayerData[i];
	}

	return NULL;
}
//======================================================================

//======================================================================
bool CPlayerMgr::IsPlaying(const EPLAYERID iEntIndex)
{
	//Return whether this player entity is "alive" (gateway)

	if (!VALID_EID(iEntIndex))
		return false;

	register MPLAYERID uiEntIndex = EID_TO_MID(iEntIndex);

	return this->IsPlaying(uiEntIndex);
}
//======================================================================

//======================================================================
bool CPlayerMgr::IsPlaying(const MPLAYERID uiEntIndex)
{
	//Return whether this player entity is "alive"

	//Validate index
	if (!VALID_MID(uiEntIndex))
		return false;

	//Check entity/flags
	return (this->m_sPlayerData[uiEntIndex].bSlotUsed) && (this->m_sPlayerData[uiEntIndex].pEntity) && (!this->m_sPlayerData[uiEntIndex].pEntity->m_bDormant) && (this->m_sPlayerData[uiEntIndex].pEntity->m_lifeState == LIFE_ALIVE);
}
//======================================================================

//======================================================================
bool CPlayerMgr::CalcScreen(const Vector& vWorldPos, Vector& vScreenPos)
{
	//Transform player 3D world position to 2D screen position

	return g_pDebugOverlay->ScreenPosition(vWorldPos, vScreenPos) != 0;
}
//======================================================================

//======================================================================
bool CPlayerMgr::UpdateCurWeapon(const MPLAYERID uiEntIndex)
{
	//Get target players weapon from model file name and remove unneccessary file name pre/postfix

	//Validate index and entity

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	
	
	return false;
}
//======================================================================

//======================================================================
bool CPlayerMgr::IsVisible(const MPLAYERID uiEntIndex)
{
	//Check if target player is visible

	//Validate index and entity

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	//Get local player data pointer
	player_data_s* pLocalPlayer = this->Local();
	if (!pLocalPlayer)
		return false;

	//Perform trace calculation
	

	return false; //Check if something has been between the two vectors
}
//======================================================================

//======================================================================
float CPlayerMgr::GetDistance(const MPLAYERID uiPlayerSrc, const MPLAYERID uiPlayerDst)
{
	//Calculate distance from local player to remote player

	#define FSQR(v) (v * v)

	if ((!VALID_MID(uiPlayerSrc)) || (!VALID_MID(uiPlayerDst)))
		return -1.0f;

	Vector vDist = this->m_sPlayerData[uiPlayerDst].pEntity->m_vecAbsOrigin - this->m_sPlayerData[uiPlayerSrc].pEntity->m_vecAbsOrigin;

	return sqrt(FSQR(vDist.x) + FSQR(vDist.y) + FSQR(vDist.z));
}
//======================================================================

/*//======================================================================
bool CPlayerMgr::DrawInfo(const MPLAYERID uiEntIndex, const std::string& szText, const byte ucLineId, const bool bCentered)
{
	//Draw info string to this player

	//Validate index, entity and screen position

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	if (!this->m_sPlayerData[uiEntIndex].bScreenPosValid)
		return false;

	//Get pointer to local player data
	player_data_s* pLocalPlayer = this->Local();
	if (!pLocalPlayer)
		return false;

	//Correct line id
	short siLineId = (short)ucLineId - 1;

	color24 sDrawingColor;

	//Set drawing text color
	if ((pLocalPlayer->eTeam != this->m_sPlayerData[uiEntIndex].eTeam) && (this->m_sPlayerData[uiEntIndex].bVisible))
		//If this player is not in the same team of local player and is visible then use belonging color
		sDrawingColor = this->m_sTeamColors[PLAYER_TEAM_VISIBLE];
	else //Use team related color
		sDrawingColor = this->m_sTeamColors[this->m_sPlayerData[uiEntIndex].eTeam];

	if (!bCentered) {
		//Draw non-centered string
		g_oD3DRenderer.DrawString(szText, "Verdana", 7, 15, (int)this->m_sPlayerData[uiEntIndex].vScreenPos[0], (int)this->m_sPlayerData[uiEntIndex].vScreenPos[1] + LINE_OFFSET(siLineId), sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, 255);
	} else {
		const int iLength = 7, iHeight = 15;

		//Draw centered string
		g_oD3DRenderer.DrawString(szText, "Verdana", 7, 15, (int)this->m_sPlayerData[uiEntIndex].vScreenPos[0] - (iLength / 2), (int)this->m_sPlayerData[uiEntIndex].vScreenPos[1] + LINE_OFFSET(siLineId), sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, 255);
	}

	return true;
}
//======================================================================*/

//======================================================================
bool CPlayerMgr::DrawInfo(const MPLAYERID uiEntIndex, const std::string& szText, const byte ucLineId)
{
	//Draw info string to this player

	//Validate index, entity and screen position

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	//Get pointer to local player data
	player_data_s* pLocalPlayer = this->Local();
	if (!pLocalPlayer)
		return false;

	color24 sDrawingColor;

	//Set drawing text color
	if ((pLocalPlayer->iTeam != this->m_sPlayerData[uiEntIndex].iTeam) && (this->m_sPlayerData[uiEntIndex].bVisible)) {
		//If this player is not in the same team of local player and is visible then use belonging color
		sDrawingColor = this->m_sTeamColors[PLAYER_TEAM_VISIBLE];
	} else { //Use team related color
		if ((g_pcvColorModeESP) && (!g_pcvColorModeESP->iValue)) { //Check if team color shall be used
			sDrawingColor = this->m_sTeamColors[this->m_sPlayerData[uiEntIndex].pEntity->m_iTeamNum - 1];
		} else { //Team context related color shall be used
			if (pLocalPlayer->iTeam != this->m_sPlayerData[uiEntIndex].iTeam)
				sDrawingColor = this->MakeColor24(255, 0, 0); //Opponent team color
			else
				sDrawingColor = this->MakeColor24(200, 200, 200); //Own team color
		}
	}

	//Add overlay text to player entity
	g_pDebugOverlay->AddEntityTextOverlay(this->m_sPlayerData[uiEntIndex].iEngineId, ucLineId, 1.0f, sDrawingColor.r, sDrawingColor.g, sDrawingColor.b, 255, szText.c_str());

	return true;
}
//======================================================================

//======================================================================
bool CPlayerMgr::DrawName(const MPLAYERID uiEntIndex, const byte ucLineId/*, const bool bCentered*/)
{
	//Draw name of this player

	//Validate index and entity

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	//Draw name
	return this->DrawInfo(uiEntIndex, this->m_sPlayerData[uiEntIndex].sHudInfo.name, ucLineId/*, bCentered*/);
}
//======================================================================

//======================================================================
bool CPlayerMgr::DrawDistance(const MPLAYERID uiEntIndex, const byte ucLineId/*, const bool bCentered*/)
{
	//Draw distance from local player to this player

	//Validate index and entity

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	//Get pointer to local player related data
	player_data_s* pLocal = this->Local();
	if (!pLocal)
		return false;

	//Calculate distance between both players
	float fDistance = this->GetDistance(EID_TO_MID(pLocal->iEngineId), uiEntIndex);
	if (fDistance <= 0.0f)
		return false;

	//Convert to string
	std::string szText = std::to_string((_ULonglong)(int)fDistance);

	//Draw distance
	return this->DrawInfo(uiEntIndex, szText, ucLineId/*, bCentered*/);
}
//======================================================================

//======================================================================
bool CPlayerMgr::DrawWeapon(const MPLAYERID uiEntIndex, const byte ucLineId/*, const bool bCentered*/)
{
	//Draw weapon of this player

	//Validate index and entity

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	//Draw weapon
	return this->DrawInfo(uiEntIndex, this->m_sPlayerData[uiEntIndex].szCurWeapon, ucLineId/*, bCentered*/);
}
//======================================================================

//======================================================================
bool CPlayerMgr::DrawSteamID(const MPLAYERID uiEntIndex, const byte ucLineId/*, const bool bCentered*/)
{
	//Draw SteamID of this player

	//Validate index and entity

	if (!VALID_MID(uiEntIndex))
		return false;

	if (!this->IsPlaying(uiEntIndex))
		return false;

	//Draw SteamID
	return this->DrawInfo(uiEntIndex, this->m_sPlayerData[uiEntIndex].sHudInfo.steamid, ucLineId/*, bCentered*/);
}
//======================================================================
