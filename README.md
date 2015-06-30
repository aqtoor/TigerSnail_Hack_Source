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
