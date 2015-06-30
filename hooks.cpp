#include "hooks.h"
#include "drawing.h"
#include "viscomps.h"
#include "d3d.h"
#include <icliententitylist.h>
#include <entitylist_base.h>

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.2
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	File: hooks.cpp: Hook implementations
*/

//======================================================================
CGameEventListener* pSampleEventListener = NULL;
//======================================================================

//======================================================================
void SOURCEAPI OnInitialize(void)
{
	//Called for initializations

	//Get screen size
	g_pEngineClient->GetScreenSize(g_ScreenSize.x, g_ScreenSize.y);

	//Reset player data
	g_oPlayerMgr.Reset();

	//Set colors
	g_oPlayerMgr.SetTeamColor(PLAYER_TEAM_CT, g_oPlayerMgr.MakeColor24(121, 205, 205));
	g_oPlayerMgr.SetTeamColor(PLAYER_TEAM_T, g_oPlayerMgr.MakeColor24(210, 105, 30));
	g_oPlayerMgr.SetTeamColor(PLAYER_TEAM_VISIBLE, g_oPlayerMgr.MakeColor24(0, 255, 0));

	//Load game event resource file
	if (!g_pGameEventManager->LoadEventsFromFile("resource/gameevents.res"))
		ExitError("IGameEventManager::LoadEventsFromFile() failed");

	//Register sample game event listener
	pSampleEventListener = new CGameEventListener("player_spawn");
	if (!pSampleEventListener)
		ExitError("Failed to instantiate sample game event listener");

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
	delete pSampleEventListener;
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
			g_oPlayerMgr.UpdateSlot(i);
		}
	}
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

	if (keynum == g_pcvMenuKey->iValue) { //Check if menu key has been toggled
		g_bMenuToggle = !g_bMenuToggle; //Toggle indicator

		//Set blocking mode
		g_InputMgr.SetBlockMode(g_bMenuToggle, true);

		if (g_bMenuToggle) {
			g_pMainForm->Show(); //Show if enabled
		} else {
			g_pMainForm->Hide(); //Hide if disabled
		}

		return 1; //Handled
	}

	if (g_bMenuToggle) { //If menu is handled
		g_pMainForm->KeyEvent(keynum, eventcode);

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

					if (g_pcvNameESP->bValue) { //Name ESP
						g_oPlayerMgr.DrawName(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
						iSlotCount++;
					}

					if (g_pcvSteamIDESP->bValue) { //SteamID ESP
						g_oPlayerMgr.DrawSteamID(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
						iSlotCount++;
					}

					if (g_pcvDistanceESP->bValue) { //Distance ESP
						g_oPlayerMgr.DrawDistance(EID_TO_MID(i), iSlotCount); //Draw text overlay on screen for this player entity
						iSlotCount++;
					}
				}
			}
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
bool HookCHLClient(void)
{
	//Hook CHLClient method(s)

	HOOK_METHOD("CHLClient", FNI_PostInit, Hook_PostInit);
	HOOK_METHOD("CHLClient", FNI_Shutdown, Hook_Shutdown);
	HOOK_METHOD("CHLClient", FNI_LevelInitPreEntity, Hook_LevelInitPreEntity);
	HOOK_METHOD("CHLClient", FNI_HudUpdate, Hook_HudUpdate);
	HOOK_METHOD("CHLClient", FNI_IN_KeyEvent, Hook_IN_KeyEvent);

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
