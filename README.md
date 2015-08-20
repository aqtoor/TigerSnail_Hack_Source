# TigerSnail_Hack_Source
An internal clientside hackbase for the Source Engine (specifically for Counter-Strike: Global Offensive)

Features:
* Up-to-date classes/structs (complete or partially):
	- player_info_t
	- IVEngineClient
	- CClientEntityList
	- C_BaseEntity
	- IAppSystem
	- IStudioRender
	- IVDebugOverlay
	- ISurface
	- IGameEventManager2
	- IGameEventListener2
	- IGameEvent
	- IInputSystem
	- IInput
	- IEngineTrace
* Access to:
	- appSystemFactory function
	- Client exports
	- IVEngineClient instance
	- CClientEntityList instance
	- IVDebugOverlay instance
	- IStudioRender instance
	- CHLClient instance
	- IPanel instance
	- ISurface instance
	- ICVar instance
	- IGameEventManager2 instance
	- Games' IDirect3DDevice9 context
	- Clients' IInput class instance
	- IEngineTrace instance
* Hooks:
	- CHLClient method(s)
	- IPanel method(s)
	- IStudioRender method(s)
	- IVEngineClient method(s)
	- IDirect3DDevice9::EndScene
* CzyConfigMgr v0.3
* Window menu system
	- Base class
	- Form
	- Label
	- Button
	- Checkbox
	- Textbox
	- Groupbox
	- Listbox
	- Imagebox
	- Contextmenu
	- ProgressBar
* Input manager
* VFT hook manager
* D3D renderer
* Player manager
* Game event handler
* Log system
* Delayed command manager
* Engine drawing wrapper
* Memory functions
* Utilities
* Enhancements
	- Name ESP
	- SteamID ESP
	- Distance ESP
	- Health ESP
	- Decoy ESP
	- Bomb ESP
	- Playermodel color
* Infobox
* Snake game

Changelog:
* Version 0.1
	- < Initial release >
* Version 0.2
	- Extended _C_BaseEntity class definition
	- Added _IGameEventManager2 and _IGameEventListener2 class definition
	- Added access to IGameEventManager2 instance
	- Added game event manager
	- Added player manager
	- Added distance ESP
	- Added further ESP control CVars
	- Fixed a bug of the VFT hook manager
* Version 0.3
	- CHLClient has been updated (now 017)
	- Unified non-unified header comments (on some mail-info was missing)
* Version 0.4
	- Extended _C_BaseEntity class definition
	- Redesigned the graphical look of the GUI form
	- Added _IGameEvent class definition
	- Completed manager for game events
	- Added Decoy ESP
	- Added Infobox
	- Added Snake game
	- Added bomb ESP
	- Added health ESP
	- Changed ESP color handling
* Version 0.5
	- Added Contextmenu GUI component
	- Added ProgressBar GUI component
	- Improved GUI component implementations
	- Added IInputSystem class definition
	- Added playermodel color enhancement
	- Added IInput class definition
	- Added explained IInput::FindKey() disassembly
	- Added access to client IInput instance
	- Added CHLClient::IN_IsKeyDown() hook
	- Added CHLClient::CreateMove() hook
	- Added explained CInput::CreateMove() disassembly
	- Added access to IEngineTrace instance
	- Added IEngineTrace class definition

