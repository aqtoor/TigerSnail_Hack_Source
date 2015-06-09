TigerSnail Hack: Source
=======================

Developed by sk0r / Czybik

Version: 0.1
Contact: Czybik_Stylez<0x40>gmx<0x2E>de

Description:
	This project is a process-internal hack for the Source Engine and specifically for Counter-Strike: Global Offensive.
	It is only tested with CS:GO and all engine/client definitions have been reversed in the context of it. This hack is 
	a hackbase which you can use to build upon it. It covers important features. All disassemblies and the sourcecode are 
	commented and explained.

Features:
	+ Up-to-date classes/structs (complete or partially):
		- player_info_t
		- IVEngineClient
		- CClientEntityList
		- C_BaseEntity
		- IAppSystem
		- IStudioRender
		- IVDebugOverlay
		- ISurface
	+ Access to:
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
		- Games' IDirect3DDevice9 context
	+ Hooks:
		- CHLClient method(s)
		- IPanel method(s)
		- IStudioRender method(s)
		- IVEngineClient method(s)
		- IDirect3DDevice9::EndScene
	+ CzyConfigMgr v0.3
	+ Window menu system
		- Base class
		- Form
		- Label
		- Button
		- Checkbox
		- Textbox
		- Groupbox
		- Listbox
		- Imagebox
	+ Input manager
	+ VFT hook manager
	+ D3D renderer
	+ Log system
	+ Delayed command manager
	+ Engine drawing wrapper
	+ Memory functions
	+ Utilities
	+ Enhancements
		- Name ESP
		- SteamID ESP

Credits:
	sk0r / Czybik, OllyDbg, Source SDK
