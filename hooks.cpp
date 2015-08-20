#include "hooks.h"
#include "drawing.h"
#include "viscomps.h"
#include "d3d.h"
#include "utils.h"
#include <icliententitylist.h>
#include <entitylist_base.h>

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.5
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: hooks.cpp: Hook implementations
*/

//======================================================================
CGameEventListener* pDecoyStarted = NULL;
CGameEventListener* pDecoyFiring = NULL;
CGameEventListener* pDecoyDetonated = NULL;
CGameEventListener* pPlayerFragged = NULL;
CGameEventListener* pBombPlanted = NULL;
CGameEventListener* pBombExploded = NULL;
CGameEventListener* pBombDefused = NULL;
CGameEventListener* pBombBeginDefusal = NULL;
CGameEventListener* pBombAbortDefusal = NULL;
CGameEventListener* pBombBeep = NULL;
CGameEventListener* pBombDropped = NULL;
CGameEventListener* pBombPickup = NULL;
color24 sDecoyDefaultColor = {180, 238, 180};
color24 sDecoyFiringColor = {255, 91, 107};
CDecoyESP oDecoyESP(sDecoyDefaultColor, sDecoyFiringColor);
//======================================================================

//======================================================================
void GameEvent_DecoyStarted(_IGameEvent* pEvent)
{
	//Game event function for 'decoy_started'

	//Get data
	int userid = pEvent->GetInt("userid");
	int entityid = pEvent->GetInt("entityid");
	float x = pEvent->GetFloat("x");
	float y = pEvent->GetFloat("y");
	float z = pEvent->GetFloat("z");
	
	//Add entry for this decoy
	oDecoyESP.Add(Vector(x, y, z), userid, entityid);
}
//======================================================================

//======================================================================
void GameEvent_DecoyFiring(_IGameEvent* pEvent)
{
	//Game event function for 'decoy_firing'

	//Get data
	int userid = pEvent->GetInt("userid");
	int entityid = pEvent->GetInt("entityid");
	float x = pEvent->GetFloat("x");
	float y = pEvent->GetFloat("y");
	float z = pEvent->GetFloat("z");

	//Update status
	oDecoyESP.UpdateStatus(entityid, CDecoyESP::DES_FIRING, Vector(x, y, z));
}
//======================================================================

//======================================================================
void GameEvent_DecoyDetonated(_IGameEvent* pEvent)
{
	//Game event function for 'decoy_detonate'

	//Get data
	int userid = pEvent->GetInt("userid");
	int entityid = pEvent->GetInt("entityid");
	float x = pEvent->GetFloat("x");
	float y = pEvent->GetFloat("y");
	float z = pEvent->GetFloat("z");

	//Update status
	oDecoyESP.UpdateStatus(entityid, CDecoyESP::DES_EXPLODED, Vector(x, y, z));
}
//======================================================================

//======================================================================
void GameEvent_PlayerFragged(_IGameEvent* pEvent)
{
	//Player fragging event
	
	player_data_s* pLocalData = g_oPlayerMgr.Local();
	if (!pLocalData)
		return;

	//Get data
	int userid = pEvent->GetInt("userid");
	int attacker = pEvent->GetInt("attacker");
	bool headshot = pEvent->GetBool("headshot");

	//Check for selffrag and if attacker is not the local player
	if ((userid == attacker) || (pLocalData->pEntity->m_iIndex != attacker - 1))
		return;

	//Update data

	g_sAccuracy.iFrags++;

	if (headshot) {
		g_sAccuracy.iHeadshots++;
	}

	g_sAccuracy.iPercent = (float)g_sAccuracy.iHeadshots / g_sAccuracy.iFrags * 100;
}
//======================================================================

//======================================================================
void GameEvent_BombPlanted(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_planted'

	g_sBombData.ucStatus = BOMB_PLANTED;
	g_sBombData.wBombSpot = pEvent->GetInt("site");
}
//======================================================================

//======================================================================
void GameEvent_BombBeep(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_beep'

	g_sBombData.iBombEntityIndex = pEvent->GetInt("entindex");
	g_sBombData.pBombEntity = GetClientBaseEntity(g_sBombData.iBombEntityIndex);
}
//======================================================================

//======================================================================
void GameEvent_BombExploded(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_exploded'

	memset(&g_sBombData, 0x00, sizeof(g_sBombData));
}
//======================================================================

//======================================================================
void GameEvent_BombDefused(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_defused'

	memset(&g_sBombData, 0x00, sizeof(g_sBombData));
}
//======================================================================

//======================================================================
void GameEvent_BeginDefusal(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_begindefuse'

	g_sBombData.bDefusalStarted = true;
	g_sBombData.bWithDefKit = pEvent->GetBool("haskit");
}
//======================================================================

//======================================================================
void GameEvent_AbortDefusal(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_abortdefuse'

	g_sBombData.bDefusalStarted = false;
}
//======================================================================

//======================================================================
void GameEvent_BombDropped(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_dropped'

	g_sBombData.ucStatus = BOMB_DROPPED;
	g_sBombData.iBombEntityIndex = pEvent->GetInt("entindex");
	g_sBombData.pBombEntity = GetClientBaseEntity(g_sBombData.iBombEntityIndex);
}
//======================================================================

//======================================================================
void GameEvent_BombPickup(_IGameEvent* pEvent)
{
	//Game event function for 'bomb_pickup'

	memset(&g_sBombData, 0x00, sizeof(g_sBombData));
}
//======================================================================

//======================================================================
void SOURCEAPI OnInitialize(void)
{
	//Called for initializations

	//Get screen size
	g_pEngineClient->GetScreenSize(g_ScreenSize.x, g_ScreenSize.y);

	//Register infobox
	if (!RegisterInfobox())
		ExitError("RegisterInfobox() failed");

	//Initialize Snake component
	if (!InitSnakeGame())
		ExitError("InitSnakeGame() failed");

	//Center snake window to screen
	g_oSnake.CenterToScreen(g_ScreenSize.x, g_ScreenSize.y);

	//Set snake velocity
	g_oSnake.SetVelocity(g_pcvSnakeVelocity->iValue);

	//Reset player data
	g_oPlayerMgr.Reset();

	//Set colors
	g_oPlayerMgr.SetTeamColor(PLAYER_TEAM_CT, g_oPlayerMgr.MakeColor24(121, 205, 205));
	g_oPlayerMgr.SetTeamColor(PLAYER_TEAM_T, g_oPlayerMgr.MakeColor24(210, 105, 30));
	g_oPlayerMgr.SetTeamColor(PLAYER_TEAM_VISIBLE, g_oPlayerMgr.MakeColor24(0, 255, 0));
	g_oPlayerMgr.SetTeamPlayerModelColor(PLAYER_TEAM_CT, g_oPlayerMgr.MakeColor24(127, 255, 212));
	g_oPlayerMgr.SetTeamPlayerModelColor(PLAYER_TEAM_T, g_oPlayerMgr.MakeColor24(255, 48, 48));

	//Load game event resource file
	if (!g_pGameEventManager->LoadEventsFromFile("resource/gameevents.res"))
		ExitError("IGameEventManager::LoadEventsFromFile() failed");

	//Register game event listeners
	#define REG_EVENT_LISTENER(p, e, n) p = new CGameEventListener(n, e); if (!p) ExitError("Failed to instantiate game event listener \'" #n "\'");
	REG_EVENT_LISTENER(pDecoyStarted, &GameEvent_DecoyStarted, "decoy_started");
	REG_EVENT_LISTENER(pDecoyFiring, &GameEvent_DecoyFiring, "decoy_firing");
	REG_EVENT_LISTENER(pDecoyDetonated, &GameEvent_DecoyDetonated, "decoy_detonate");
	REG_EVENT_LISTENER(pPlayerFragged, &GameEvent_PlayerFragged, "player_death");
	REG_EVENT_LISTENER(pBombPlanted, &GameEvent_BombPlanted, "bomb_planted");
	REG_EVENT_LISTENER(pBombExploded, &GameEvent_BombExploded, "bomb_exploded");
	REG_EVENT_LISTENER(pBombDefused, &GameEvent_BombDefused, "bomb_defused");
	REG_EVENT_LISTENER(pBombBeginDefusal, &GameEvent_BeginDefusal, "bomb_begindefuse");
	REG_EVENT_LISTENER(pBombAbortDefusal, &GameEvent_AbortDefusal, "bomb_abortdefuse");
	REG_EVENT_LISTENER(pBombBeep, &GameEvent_BombBeep, "bomb_beep");
	REG_EVENT_LISTENER(pBombDropped, &GameEvent_BombDropped, "bomb_dropped");
	REG_EVENT_LISTENER(pBombPickup, &GameEvent_BombPickup, "bomb_pickup");

	//Inform via console
	g_oDelayedClientCmd.AddClientCmd("clear", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"	====================================================\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"	[" PROGRAM_NAME "] Developed by " PROGRAM_AUTHOR "\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"	====================================================\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"\"", 1000);
	g_oDelayedClientCmd.AddClientCmd("echo \"\"", 1000);

	//Show console (delayed) if not already visible
	if (!g_pEngineClient->Con_IsVisible()) {
		g_oDelayedClientCmd.AddClientCmd("toggleconsole", 2000);
	}
	
	//Log status
	g_pLog->Msg("OnInitialize(): job done");

	//Reset playtime
	g_oPlayTime.Reset();
	
	g_bOnInitialize = true; //Indicate that initialization has been done
}
//======================================================================

//======================================================================
void SOURCEAPI OnFirstFrame(void)
{
	//Called for first HudUpdate call
}
//======================================================================

//======================================================================
void SOURCEAPI OnMapChange(void)
{
	//Called for each map change
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_PostInit(void)
{
	//Implementation of PostInit() hook
	//Second client.dll Init function
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_PostInit);
	
	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		call dwOrigFunction; //Call original PostInit method
		pop ECX; //Restore ECX
	}

	//Call initialization function
	OnInitialize();
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_Shutdown(void)
{
	//Implementation of Shutdown() hook
	//Called whenever the client.dll gets unloaded
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_Shutdown);

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		call dwOrigFunction; //Call original Shutdown method
		pop ECX; //Restore ECX
	}

	g_bMapInit = false; //Uninitialized

	//Free memory
	_delete(pDecoyStarted);
	_delete(pDecoyFiring);
	_delete(pDecoyDetonated);
	_delete(pPlayerFragged);
	_delete(pBombPlanted);
	_delete(pBombExploded);
	_delete(pBombDefused);
	_delete(pBombBeginDefusal);
	_delete(pBombAbortDefusal);
	_delete(pBombBeep);
	_delete(pBombDropped);
	_delete(pBombPickup);
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_LevelInitPreEntity(const char* pMapName)
{
	//Implementation of LevelInitPreEntity() hook
	//Called at every map change start
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_LevelInitPreEntity);
	
	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		push pMapName; //Push function argument on stack
		call dwOrigFunction; //Call original LevelInitPreEntity method
		pop ECX; //Restore ECX
	}

	g_bMapInit = true; //Map initialization is ok

	//Save map name
	g_szCurrentMapName = std::string(pMapName);

	//Call event function
	OnMapChange();
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_HudUpdate(bool bActive)
{
	//Implementation of HudUpdate() hook
	//Called every frame
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_HudUpdate);

	DWORD dwActive = bActive;

	//If no initialization has been done then do it now
	if (!g_bOnInitialize) {
		OnInitialize();
	}

	//Handle first frame execution
	static bool bOnlyOnce = false;
	if (!bOnlyOnce) {
		bOnlyOnce = true;
		
		OnFirstFrame();
	}

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		push dwActive; //Push function argument on stack
		call dwOrigFunction; //Call original HudUpdate method
		pop ECX; //Restore ECX
	}

	//Update indicator
	g_bIsInGame = g_pEngineClient->IsInGame();

	//Process delayed client commands if any
	g_oDelayedClientCmd.Process();

	//Update player data
	if (g_bIsInGame) {
		for (EPLAYERID i = 1; i < MAX_CLIENTS; i++) {
			if (g_oPlayerMgr.UpdateSlot(i)) {
				if (g_oPlayerMgr.IsPlaying(i)) {
					//Handle playermodel color
					g_oPlayerMgr.SetPlayerModelColor(EID_TO_MID(i), g_pcvPlayerModelColor->bValue);
				}
			}
		}
	}

	//Update playtime
	g_oPlayTime.Think();

	//Update infobox data
	UpdateInfoboxData();
}
//======================================================================

//======================================================================
bool SOURCEAPI Hook_IN_IsKeyDown(const char *name, bool& isdown)
{
	//Implementation of IN_IsKeyDown() hook
	//Called for kbutton issues. Return 1 to indicate that the specified kbutton is held down, otherwise false
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_IN_IsKeyDown);

	DWORD dwReturnValue;

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		push isdown; //Push function second argument on stack
		push name; //Push function first argument on stack
		call dwOrigFunction; //Call original IN_IsKeyDown method
		mov dwReturnValue, EAX; //Copy function result from EAX to DWORD var
		pop ECX; //Restore ECX
	}

	return dwReturnValue;
}
//======================================================================

//======================================================================
int SOURCEAPI Hook_IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding)
{
	//Implementation of IN_KeyEvent() hook
	//Called for evey key issue. Return 1 to let the engine handle the key, else return 0
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_IN_KeyEvent);

	if (keynum == g_pcvMenuKey->iValue) { //Check if menu key has been used
		g_bMenuToggle = !g_bMenuToggle; //Toggle indicator

		//Set blocking mode
		g_InputMgr.SetBlockMode(g_bMenuToggle, true);

		if (g_bMenuToggle) {
			g_pMainForm->Show(); //Show if enabled
		} else {
			g_pMainForm->Hide(); //Hide if disabled
		}

		return 1; //Handled
	} else if ((keynum == g_pcvSnakeKey->iValue) && (!g_bMenuToggle) && (eventcode)) { //Check if snake key has been used and menu is not shown
		g_bSnakeToggle = !g_bSnakeToggle; //Toggle indicator
		
		//Set blocking mode
		g_InputMgr.SetBlockMode(g_bSnakeToggle, true);

		return 1; //Handled
	} else if ((keynum == g_pcvInfoboxKey->iValue) && (eventcode)) { //Check if infobox key has been used
		g_bInfoboxToggle = !g_bInfoboxToggle; //Toggle indicator
		
		if (g_bInfoboxToggle) {
			g_pInfoBox->Show(); //Show if enabled
		} else {
			g_pInfoBox->Hide(); //Hide if disabled
		}

		return 1; //Handled
	}

	if (g_bMenuToggle) { //If menu is handled
		g_pMainForm->KeyEvent(keynum, eventcode);

		return 1; //Handled
	} else if (g_bSnakeToggle) {
		g_oSnake.KeyEvent(keynum, eventcode);

		return 1; //Handled
	}

	DWORD dwReturnValue;

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		push pszCurrentBinding; //Push third function argument on stack
		push keynum; //Push function second argument on stack
		push eventcode; //Push function first argument on stack
		call dwOrigFunction; //Call original IN_KeyEvent method
		mov dwReturnValue, EAX; //Copy function result from EAX to DWORD var
		pop ECX; //Restore ECX
	}

	return dwReturnValue;
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_PaintTraverse(VPANEL vguiPanel, DWORD forceRepaint, DWORD allowForce)
{
	//Implementation of PaintTraverse() hook
	//Hooked for drawing issues
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_IPANEL(FNI_PaintTraverse);

	DWORD dwVguiPanel = vguiPanel;
	DWORD dwForceRepaint = forceRepaint;
	DWORD dwAllowForce = allowForce;
	
	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the IPanel instance to ECX, the same the this-pointer holds. This is needed for every method call
		push dwAllowForce; //Push third function argument on stack
		push dwForceRepaint; //Push function second argument on stack
		push dwVguiPanel; //Push function first argument on stack
		call dwOrigFunction; //Call original PaintTraverse method
		pop ECX; //Restore ECX
	}
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_BeginFrame(void)
{
	//Implementation of BeginFrame(() hook
	//Hooked for overlay issues
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_ISTUDIORENDER(FNI_BeginFrame);

	//Clear all overlays before a frame execution takes place, because other possible overlays
	//that may be created during the call of the original method shall not get removed
	g_pDebugOverlay->ClearAllOverlays();

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the IStudioRender instance to ECX, the same the this-pointer holds. This is needed for every method call
		call dwOrigFunction; //Call original BeginFrame method
		pop ECX; //Restore ECX
	}

	if (g_bIsInGame) { //If ingame context is given
		if ((g_pcvNameESP->bValue) || (g_pcvSteamIDESP->bValue) || (g_pcvDistanceESP->bValue)) { //Check if one of the concerning enhancements is enabled
			for (EPLAYERID i = 1 ; i < MAX_CLIENTS; i++) { //Loop through player entity slots
				player_data_s* pPlayerData = g_oPlayerMgr.Slot(i); //Get player data of current player slot
				if ((pPlayerData) && (g_oPlayerMgr.IsPlaying(i))) { //Check if player is valid to be used for drawing
					//Ignore local player
					if (pPlayerData->bIsLocal)
						continue;

					//If desired ignore teammates for drawing
					if (g_pcvIgnoreTeammatesESP->bValue) {
						player_data_s* pLocalPlayer = g_oPlayerMgr.Local();
						if (pLocalPlayer) {
							if (pLocalPlayer->iTeam == pPlayerData->iTeam)
								continue;
						}
					}

					register int iSlotCount = 0; //Used so there won't be empty slot lines

					//Name ESP
					if (g_pcvNameESP->bValue) {
						g_oPlayerMgr.DrawName(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
						iSlotCount++;
					}

					//SteamID ESP
					if (g_pcvSteamIDESP->bValue) {
						g_oPlayerMgr.DrawSteamID(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
						iSlotCount++;
					}

					//Distance ESP
					if (g_pcvDistanceESP->bValue) {
						g_oPlayerMgr.DrawDistance(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
						iSlotCount++;
					}

					//Health ESP
					if (g_pcvHealthESP->bValue) {
						g_oPlayerMgr.DrawHealth(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
					}
				}
			}
		}

		//Handle decoy thinking if feature is enabled
		if (g_pcvDecoyESP->bValue)
			oDecoyESP.Think();

		//Handle bomb stuff
		if ((g_pcvBombESP->bValue) && (g_sBombData.ucStatus > BOMB_UNKNOWN) && (g_sBombData.pBombEntity)) {
			g_pDebugOverlay->AddTextOverlayRGB(g_sBombData.pBombEntity->m_vecAbsOrigin, 0, 1.0f, 238, 48, 167, 200, "Bomb");
		}
	}
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_SetViewAngles(QAngle& va)
{
	//Implementation of SetViewAngles(() hook
	//Hooked for overlay issues
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_IVENGINECLIENT(FNI_SetViewAngles);
	
	if (!g_InputMgr.Event_SetViewAngles(va)) //Call event method and block setting viewangles if indicated
		return;

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the IVEngineClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		push DWORD ptr[va]; //Push only function argument on stack
		call dwOrigFunction; //Call original SetViewAngles method
		pop ECX; //Restore ECX
	}
}
//======================================================================

//======================================================================
void SOURCEAPI Hook_CreateMove(int sequence_number, float input_sample_frametime, unsigned int active)
{
	//Implementation of CreateMove(() hook
	//Hooked for interfering with input/movement calculation
	//Note: C++ class methods are cleaning up the function arguments themselves (via 'ret n') that's why it must be declared as __stdcall
	//      Also they pass the this pointer via ECX to each method

	//Static objects for class base and original method address pointer
	SETUP_STATIC_OBJECTS_CHLCLIENT(FNI_CreateMove);

	__asm {
		push ECX; //Backup ECX
		mov ECX, dwClassBase; //Copy start address of the CHLClient instance to ECX, the same the this-pointer holds. This is needed for every method call
		push active; //Push third function argument on stack
		push input_sample_frametime; //Push second function argument on stack
		push sequence_number; //Push first function argument on stack
		call dwOrigFunction; //Call original CreateMove method
		pop ECX; //Restore ECX
	}

	if (g_bIsInGame) { //Do following stuff only when being ingame
		//Access CUserCmd and CVerifiedUserCmd base objects
		CUserCmd* pCommands = *(CUserCmd**)((DWORD)g_pInput + 0xEC);
		CVerifiedUserCmd* pVerifiedCommands = *(CVerifiedUserCmd**)((DWORD)g_pInput + 0xF0);
	}
}
//======================================================================

//======================================================================
bool HookCHLClient(void)
{
	//Hook CHLClient method(s)

	HOOK_METHOD("CHLClient", FNI_PostInit, Hook_PostInit);
	HOOK_METHOD("CHLClient", FNI_Shutdown, Hook_Shutdown);
	HOOK_METHOD("CHLClient", FNI_LevelInitPreEntity, Hook_LevelInitPreEntity);
	HOOK_METHOD("CHLClient", FNI_HudUpdate, Hook_HudUpdate);
	HOOK_METHOD("CHLClient", FNI_IN_IsKeyDown, Hook_IN_IsKeyDown);
	HOOK_METHOD("CHLClient", FNI_IN_KeyEvent, Hook_IN_KeyEvent);
	HOOK_METHOD("CHLClient", FNI_CreateMove, Hook_CreateMove);

	return true;
}
//======================================================================

//======================================================================
bool HookIPanel(void)
{
	//Hook IPanel method(s)
	
	HOOK_METHOD("IPanel", FNI_PaintTraverse, Hook_PaintTraverse);

	return true;
}
//======================================================================

//======================================================================
bool HookIStudioRender(void)
{
	//Hook IStudioRender method(s)

	HOOK_METHOD("IStudioRender", FNI_BeginFrame, Hook_BeginFrame);

	return true;
}
//======================================================================

//======================================================================
bool HookIVEngineClient(void)
{
	//Hook IVEngineClient method(s)

	HOOK_METHOD("IVEngineClient", FNI_SetViewAngles, Hook_SetViewAngles);

	return true;
}
//======================================================================
