#ifndef _GAMESPEC_H
#define _GAMESPEC_H

#define CLIENT_DLL

#include <cbase.h>
#include <vgui\ISurface.h>
#include <vgui\IPanel.h>
#include <igameevents.h>
#include <inputsystem\iinputsystem.h>
#include <in_buttons.h>
#include <input.h>
#include <util_shared.h>
#include "memory.h"

/*
	TigerSnail Hack: Source

	Developed by sk0r / Czybik
	Credits: sk0r, OllyDbg, Source SDK

	Version: 0.5
	Visit: http://sk0r.sytes.net
	Mail Czybik_Stylez<0x40>gmx<0x2E>de

	gamespec.h: Game specific interface
*/

/*
	//Entry Point of:
	//void* __cdecl CreateInterface(const char*, int*);
	5487D700 >|$ 55             |PUSH EBP								 ; Push EBP onto stack to save old frame pointer
	5487D701  |. 8BEC           |MOV EBP,ESP							 ; Copy ESP to EBP to have frame pointer for this function
	5487D703  |. 5D             |POP EBP							     ; Restore old frame pointer 
	5487D704  \.^E9 87FFFFFF    \JMP client.5487D690					 ; Jump to code part below
	->
	5487D690  /> 55             /PUSH EBP
	5487D691  |. 8BEC           |MOV EBP,ESP
	5487D693  |. 56             |PUSH ESI
	5487D694  |. 8B35 E8ABD554  |MOV ESI,DWORD PTR DS:[54D5ABE8]         ;  Copy address of 'InterfaceReg::s_pInterfaceRegs' to ESI

	//Get ICvar pointer via class objects VFT:
	//Code part from CHLClient Init function
	16BF3384   8B0D F4E0491B    MOV ECX,DWORD PTR DS:[1B49E0F4]          ; Copy value from address to ECX (ICvar)
	16BF338A   68 F0222417      PUSH client.172422F0                     ; Push string address onto stack as function argument: ASCII "host_thread_mode"
	16BF338F   8B01             MOV EAX,DWORD PTR DS:[ECX]				 ; Copy address of VFT to EAX
	16BF3391   FF50 40          CALL DWORD PTR DS:[EAX+40]				 ; Call function at VFT+0x40 (ICVar::FindVar)

	//Code part which is responsible to call client.dll Init() function
	5456096B   FF35 C47B9754    PUSH DWORD PTR DS:[54977BC4]             ; Push address of appSystemFactory onto stack as function argument
	54560971   FF50 08          CALL DWORD PTR DS:[EAX+8]				 ; Call CHLClient::Init() method
	54560974   85C0             TEST EAX,EAX							 ; Perform test-operation of EAX with EAX to handle return value
	54560976   75 0D            JNZ SHORT engine.54560985				 ; If at least one bit could be set (result != false) then jump to success part, else continue with error part below
	54560978   68 742E9354      PUSH engine.54932E74                     ; ASCII "Client.dll Init() in library client failed."


	//Some notes about the CreateInterface function of client.dll:
	The client.dll module provides an interface function called CreateInterface.
	It is exported via the export address table of the module. Its purpose is to
	look for own exported interfaces to the engine. The identity of the interface 
	is passed via a string (for instance VClient015). The CreateInterface function
	now searches for the string in a linked list. If found it returns the address
	of a function. This function now simply returns the start address of a class
	object with API methods. Those methods are called by the engine. You can
	also pass a pointer to an integer to the CreateInterface function which will
	be set to a status value (either success or failure) but it is not really 
	important, so you just can pass NULL. The client recieves also a pointer to
	the function appSystemFactory which is used to retrieve interfaces from the
	engine (e.g. IVEngineClient). Those interfaces are used to force the engine
	to perform specific tasks. The client.dll can now define the game mod via the 
	exports and the imports.
	
	//Disassembly of CHLClient::IN_IsKeyDown() method implementation
	//Purpose: Find base address of IInput or examine kbutton structure layout
	19BA9950   55               PUSH EBP	; Backup old EBP on stack
	19BA9951   8BEC             MOV EBP,ESP	; Copy ESP to EBP, stackframe setup done
	19BA9953   FF75 08          PUSH DWORD PTR SS:[EBP+8]	; Push first argument onto stack as method argument (const char* name)
	19BA9956   B9 10CF3C1E      MOV ECX,client.1E3CCF10	; Copy base address of IInput instance to ECX as this pointer
	19BA995B   E8 E0840800      CALL client.19C31E40	; Call IInput::FindKey() method
	19BA9960   85C0             TEST EAX,EAX	; Perform TEST-operation with result value in EAX
	19BA9962   75 06            JNZ SHORT client.19BA996A	; If not NULL jump proceed with success-related part
	19BA9964   32C0             XOR AL,AL	; Clear bits in AL (result = false)
	19BA9966   5D               POP EBP	; Restore old EBP
	19BA9967   C2 0800          RETN 8	; Restore return address and free stack space of method arguments
	19BA996A   8A48 08          MOV CL,BYTE PTR DS:[EAX+8]	; Copy key->state into CL
	19BA996D   8B45 0C          MOV EAX,DWORD PTR SS:[EBP+C]	; Copy second method argument to EAX (bool& isdown)
	19BA9970   80E1 01          AND CL,1	; Perform bitwise AND operation with CL and 0x01 (check if key is down)
	19BA9973   8808             MOV BYTE PTR DS:[EAX],CL	; Copy result from CL into isdown variable
	19BA9975   B0 01            MOV AL,1	; Copy 1 to AL (result = true)
	19BA9977   5D               POP EBP	; Restore old EBP
	19BA9978   C2 0800          RETN 8	; Restore return address and free stack space of method arguments

	//Disassembly of CInput::CreateMove() method implementation parts
	//Purpose: Find address of CUserCmd and CVerifiedUserCmd
	18464200   55               PUSH EBP	; Backup old EBP onto stack
	18464201   8BEC             MOV EBP,ESP	; Copy ESP to EBP (stack frame pointer setup done)
	18464203   83EC 18          SUB ESP,18	; Subtract 0x18 bytes from ESP to allocate memory for stack variables
	18464206   53               PUSH EBX	; Backup EBX on stack
	18464207   8B5D 08          MOV EBX,DWORD PTR SS:[EBP+8]	; Copy dword at [EBP+0x08] (int sequence_number) to EBX
	...
	18464223   8BF9             MOV EDI,ECX	; Copy ECX (CInput class instance base address) to EDI
	18464225   8BCB             MOV ECX,EBX	; Copy EBX to ECX
	...
	18464229   8BB7 EC000000    MOV ESI,DWORD PTR DS:[EDI+EC]	; Copy dword at [EDI+EC] (CInput::m_pCommands) to ESI
	1846422F   6BC1 64          IMUL EAX,ECX,64	; Calculate multiplication (signed) EAX = ECX * 0x64 (Result is offset to array entry (EAX/sizeof(DWORD])))
	18464232   03F0             ADD ESI,EAX	; Add EAX to ESI to get absolute address of array entry (m_pCommands[EAX/sizeof(DWORD)])
	18464234   8945 FC          MOV DWORD PTR SS:[EBP-4],EAX	; Copy address to [EBP-4]
	18464237   6BC1 68          IMUL EAX,ECX,68	; Calculate multiplication (signed) EAX = ECX * 0x68 (Result is offset to array entry (EAX/sizeof(DWORD])))
	1846423A   8BCE             MOV ECX,ESI	; Copy ESI to ECX (CUserCmd this-pointer)
	1846423C   0387 F0000000    ADD EAX,DWORD PTR DS:[EDI+F0]	; Add value at dword [EDI+0xF0] (CInput::m_pVerifiedCommands) to EAX to have full address of array entry
	18464242   8945 F8          MOV DWORD PTR SS:[EBP-8],EAX	; Copy address to [EBP-0x08]
	18464245   E8 66E3F2FF      CALL client.183925B0	; Call cmd->Reset()
	1846424A   807D 10 00       CMP BYTE PTR SS:[EBP+10],0	; Compare dword at [EBP+0x10] (bool active) with 0 (false)
	1846424E   A1 80BEBC18      MOV EAX,DWORD PTR DS:[18BCBE80]	; Copy class instance address of CGlobalVarsBase to EAX
	18464253   895E 04          MOV DWORD PTR DS:[ESI+4],EBX	; Copy EBX (int sequence_number) to [ESI+0x04] (cmd->command_number)
	18464256   8B40 1C          MOV EAX,DWORD PTR DS:[EAX+1C]	; Copy dword at [EAX+0x1C] (CGlobalVarsBase::tickcount) to EAX
	18464259   8946 08          MOV DWORD PTR DS:[ESI+8],EAX	; Copy value from EAX to [ESI+0x08] (cmd->tick_count)
	1846425C   75 4F            JNZ SHORT client.184642AD	; Jump to part (comparision was from above)
*/

//======================================================================
#define DIFFERENCE_BETWEE_PUSH_AND_MOV 0x0B
#define CHLCLIENT "VClient017"
#define ICLIENTENTITYLIST "VClientEntityList003"
#define IENGINECVAR "VEngineCvar007"
#define CHLCLIENT_VFT_SIZE 84 //According to last method index
#define IVENGINECLIENT_VFT_SIZE 21*sizeof(VFTPOINTER) //According to needed method index
#define ICVAR_VFT_SIZE 20 //Random value, not checked yet
#define IPANEL_VFT_SIZE 43*sizeof(VFTPOINTER) //According to last method index
#define ISTUDIORENDER_VFT_SIZE 12*sizeof(VFTPOINTER) //According to an in-range index
#define SOURCEAPI __stdcall
#ifndef VPANEL
#define VPANEL unsigned int
#endif
#define ADD_PADDING(num, size) unsigned char __Padding__##num[size];
//======================================================================

//======================================================================
typedef void* (__cdecl *TGetExportObject)(void);
//======================================================================

//======================================================================
struct client_interface_export {
	LPVOID lpvFunction; //Pointer to interface function
	char* pszExportName; //Pointer to interface name
	client_interface_export* pNextExport; //Pointer to next entry
};

//Indices of VClient017
enum {
	FNI_PostInit = 3,
	FNI_Shutdown = 4,
	FNI_LevelInitPreEntity = 5,
	FNI_HudUpdate = 10,
	FNI_IN_IsKeyDown = 19,
	FNI_IN_KeyEvent = 20,
	FNI_CreateMove = 21
};

//Indices of IPanel
enum {
	FNI_PaintTraverse = 41
};

//Indices of IStudioRender
enum {
	FNI_BeginFrame = 9
};

//Indices of IVEngineClient
enum {
	FNI_SetViewAngles = 19
};
//======================================================================

//======================================================================
//Up-to-date Source Engine definitions for Counter-Strike: Global Offensive on 30th June 2015

// Engine player info, no game related infos here
// If you change this, change the two byteswap defintions: 
// cdll_client_int.cpp and cdll_engine_int.cpp
typedef struct _player_info_s
{
	ADD_PADDING(1, 16);

	//Name
	char			name[MAX_PLAYER_NAME_LENGTH];
	
	ADD_PADDING(2, 100);

	//SteamID
	char			steamid[MAX_PLAYER_NAME_LENGTH];

	ADD_PADDING(3, 500);
	
} _player_info_t;

struct _CUserCmd {
	DWORD dwVFTable;
	
	int command_number;
	int tick_count;

	ADD_PADDING(1, 36);

	int buttons;
};

class CVerifiedUserCmd
{
public:
	CUserCmd	m_cmd;
	CRC32_t		m_crc;
};

abstract_class _IVEngineClient
{
public:
	// Find the model's surfaces that intersect the given sphere.
	// Returns the number of surfaces filled in.
	virtual int					GetIntersectingSurfaces(
									const model_t *model,
									const Vector &vCenter, 
									const float radius,
									const bool bOnlyVisibleSurfaces,	// Only return surfaces visible to vCenter.
									SurfInfo *pInfos, 
									const int nMaxInfos) = 0;
	
	// Get the lighting intensivty for a specified point
	// If bClamp is specified, the resulting Vector is restricted to the 0.0 to 1.0 for each element
	//virtual Vector				GetLightForPoint(const Vector &pos, bool bClamp) = 0;
	virtual void __Unknown_1(unsigned int uiUnknown1, unsigned int uiUnknown2, unsigned int uiUnknown3) = 0;

	// Traces the line and reports the material impacted as well as the lighting information for the impact point
	virtual IMaterial			*TraceLineMaterialAndLighting( const Vector &start, const Vector &end, 
									Vector &diffuseLightColor, Vector& baseColor ) = 0;

	// Given an input text buffer data pointer, parses a single token into the variable token and returns the new
	//  reading position
	virtual const char			*ParseFile( const char *data, char *token, int maxlen ) = 0;
	virtual bool				CopyFile( const char *source, const char *destination ) = 0;

	// Gets the dimensions of the game window
	virtual void				GetScreenSize( int& width, int& height ) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
	//       Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void				ClientCmd( const char *szCmdString ) = 0;

	// Fill in the player info structure for the specified player index (name, model, etc.)
	virtual bool				GetPlayerInfo( int ent_num, player_info_t *pinfo ) = 0;

	// Retrieve the player entity number for a specified userID
	virtual int					GetPlayerForUserID( int userID ) = 0;

	// Retrieves text message system information for the specified message by name
	virtual client_textmessage_t *TextMessageGet( const char *pName ) = 0;

	// Returns true if the console is visible
	virtual bool				Con_IsVisible( void ) = 0;

	// Get the entity index of the local player
	virtual int					GetLocalPlayer( void ) = 0;

	// Client DLL is hooking a model, loads the model into memory and returns  pointer to the model_t
	virtual const model_t		*LoadModel( const char *pName, bool bProp = false ) = 0;

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time( void ) = 0; 

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	//virtual float				GetLastTimeStamp( void ) = 0; 
	virtual void __Unknown_2(unsigned int uiUnknown1) = 0; //16

	// Given a CAudioSource (opaque pointer), retrieve the underlying CSentence object ( stores the words, phonemes, and close
	//  captioning data )
	virtual CSentence			*GetSentence( CAudioSource *pAudioSource ) = 0;
	// Given a CAudioSource, determines the length of the underlying audio file (.wav, .mp3, etc.)
	//virtual float				GetSentenceLength( CAudioSource *pAudioSource ) = 0;
	// Returns true if the sound is streaming off of the hard disk (instead of being memory resident)
	virtual bool				IsStreaming( CAudioSource *pAudioSource ) const = 0;

	// Copy current view orientation into va
	virtual void				GetViewAngles( QAngle& va ) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles( QAngle& va ) = 0;
	
	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients( void ) = 0; //21

	// Given the string pBinding which may be bound to a key, 
	//  returns the string name of the key to which this string is bound. Returns NULL if no such binding exists
	virtual	const char			*Key_LookupBinding( const char *pBinding ) = 0;

	// Given the name of the key "mouse1", "e", "tab", etc., return the string it is bound to "+jump", "impulse 50", etc.
	virtual const char			*Key_BindingForKey( ButtonCode_t &code ) = 0;

	// key trapping (for binding keys)
	//virtual void				StartKeyTrapMode( void ) = 0;
	virtual void __Unknown_3(unsigned int uiUnknown1, unsigned int uiUnknown2) = 0;

	//virtual bool				CheckDoneKeyTrapping( ButtonCode_t &code ) = 0;
	virtual void __Unknown_4(void) = 0;
	virtual void __Unknown_5(unsigned int uiUnknown1) = 0;

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame( void ) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected( void ) = 0;

	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage( void ) = 0;

	virtual void __Unknown_6(void) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	// Similar to Con_NPrintf, but allows specifying custom text color and duration information
	virtual void				Con_NXPrintf( const struct con_nprint_s *info, const char *fmt, ... ) = 0;

	// Is the specified world-space bounding box inside the view frustum?
	virtual int					IsBoxVisible( const Vector& mins, const Vector& maxs ) = 0;

	// Is the specified world-space boudning box in the same PVS cluster as the view origin?
	virtual int					IsBoxInViewCluster( const Vector& mins, const Vector& maxs ) = 0;
	
	// Returns true if the specified box is outside of the view frustum and should be culled
	virtual bool				CullBox( const Vector& mins, const Vector& maxs ) = 0; //35

	// Allow the sound system to paint additional data (during lengthy rendering operations) to prevent stuttering sound.
	virtual void				Sound_ExtraUpdate( void ) = 0;

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory( void ) = 0; //37

	// Get access to the world to screen transformation matrix
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;
	
	// Get the matrix to move a point from world space into view space
	// (translate and rotate so the camera is at the origin looking down X).
	virtual const VMatrix& 		WorldToViewMatrix() = 0;

	// The .bsp file can have mod-specified data lumps. These APIs are for working with such game lumps.

	// Get mod-specified lump version id for the specified game data lump
	virtual int					GameLumpVersion( int lumpId ) const = 0;
	// Get the raw size of the specified game data lump.
	virtual int					GameLumpSize( int lumpId ) const = 0;
	// Loads a game lump off disk, writing the data into the buffer pointed to bye pBuffer
	// Returns false if the data can't be read or the destination buffer is too small
	virtual bool				LoadGameLump( int lumpId, void* pBuffer, int size ) = 0; //42

	// Returns the number of leaves in the level
	virtual int					LevelLeafCount() const = 0;
	
	// Gets a way to perform spatial queries on the BSP tree
	virtual ISpatialQuery*		GetBSPTreeQuery() = 0;
	
	// Convert texlight to gamma...
	virtual void		LinearToGamma( float* linear, float* gamma ) = 0;

	// Get the lightstyle value
	virtual float		LightStyleValue( int style ) = 0;

	// Computes light due to dynamic lighting at a point
	// If the normal isn't specified, then it'll return the maximum lighting
	virtual void		ComputeDynamicLighting( const Vector& pt, const Vector* pNormal, Vector& color ) = 0;

	// Returns the color of the ambient light
	virtual void		GetAmbientLightColor( Vector& color ) = 0;

	// Returns the dx support level
	virtual int			GetDXSupportLevel() = 0;

	// GR - returns the HDR support status
	virtual bool        SupportsHDR() = 0; //50

	// Replace the engine's material system pointer.
	virtual void		Mat_Stub( IMaterialSystem *pMatSys ) = 0;

	// Get the name of the current map
	virtual void GetChapterName( char *pchBuff, int iMaxLength ) = 0;
	virtual char const	*GetLevelName( void ) = 0; //53
//#if !defined( NO_VOICE )
	// Obtain access to the voice tweaking API
	virtual struct IVoiceTweak_s *GetVoiceTweakAPI( void ) = 0;
//#endif

	virtual void __Unknown_7(void) = 0;
	virtual void __Unknown_8(void) = 0; //56

	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	// This tells the engine to fire any events (temp entity messages) that it has queued up this frame. 
	// It should only be called once per frame.
	virtual void		FireEvents() = 0;

	virtual void __Unknown_9(void) = 0;

	// Returns an area index if all the leaves are in the same area. If they span multple areas, then it returns -1.
	virtual int			GetLeavesArea( int *pLeaves, int nLeaves ) = 0;

	// Returns true if the box touches the specified area's frustum.
	virtual bool		DoesBoxTouchAreaFrustum( const Vector &mins, const Vector &maxs, int iArea ) = 0;

	// Sets the hearing origin (i.e., the origin and orientation of the listener so that the sound system can spatialize 
	//  sound appropriately ).

	virtual void __Unknown_10(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0; //63

	//virtual void		SetAudioState( const AudioState_t& state ) = 0;

	virtual bool __Unknown_11(unsigned int dwUnknown1) = 0; //64

	virtual void __Unknown_12(void) = 0;

	// Sentences / sentence groups
	virtual int			SentenceGroupPick( int groupIndex, char *name, int nameBufLen ) = 0;
	virtual int			SentenceGroupPickSequential( int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset ) = 0; //67
	virtual int			SentenceIndexFromName( const char *pSentenceName ) = 0;
	virtual const char *SentenceNameFromIndex( int sentenceIndex ) = 0;
	virtual int			SentenceGroupIndexFromName( const char *pGroupName ) = 0; //70
	virtual const char *SentenceGroupNameFromIndex( int groupIndex ) = 0;
	virtual float		SentenceLength( int sentenceIndex ) = 0; //72

	// Computes light due to dynamic lighting at a point
	// If the normal isn't specified, then it'll return the maximum lighting
	// If pBoxColors is specified (it's an array of 6), then it'll copy the light contribution at each box side.
	virtual void		ComputeLighting( const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors=NULL ) = 0; //73

	// Activates/deactivates an occluder...
	virtual void		ActivateOccluder( int nOccluderIndex, bool bActive ) = 0;
	//virtual bool		IsOccluded( const Vector &vecAbsMins, const Vector &vecAbsMaxs ) = 0;

	virtual bool __Unknown_12(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3) = 0;

	virtual bool __Unknown_13(void) = 0; //76

	// The save restore system allocates memory from a shared memory pool, use this allocator to allocate/free saverestore 
	//  memory.
	virtual void		*SaveAllocMemory( size_t num, size_t size ) = 0; //77
	virtual void		SaveFreeMemory( void *pSaveMem ) = 0;

	// returns info interface for client netchannel
	virtual INetChannelInfo	*GetNetChannelInfo( void ) = 0;

	// Debugging functionality:
	// Very slow routine to draw a physics model
	virtual void		DebugDrawPhysCollide( const CPhysCollide *pCollide, IMaterial *pMaterial, matrix3x4_t& transform, const color32 &color ) = 0; //80
	// This can be used to notify test scripts that we're at a particular spot in the code.
	virtual void		CheckPoint( const char *pName ) = 0;
	// Draw portals if r_DrawPortals is set (Debugging only)
	virtual void		DrawPortals() = 0; //82
	// Determine whether the client is playing back or recording a demo
	virtual bool		IsPlayingDemo( void ) = 0;
	virtual bool		IsRecordingDemo( void ) = 0;
	virtual bool		IsPlayingTimeDemo( void ) = 0;
	// Is the game paused?
	virtual bool		IsPaused( void ) = 0;
	// Is the game currently taking a screenshot?
	virtual bool		IsTakingScreenshot( void ) = 0;
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV( void ) = 0;
	// is this level loaded as just the background to the main menu? (active, but unplayable)
	virtual bool		IsLevelMainMenuBackground( void ) = 0;
	// returns the name of the background level
	virtual void		GetMainMenuBackgroundName( char *dest, int destlen ) = 0;

	// Occlusion system control
	//virtual void		SetOcclusionParameters( const OcclusionParams_t &params ) = 0;

	virtual bool __Unknown_14(void) = 0; //91

	// What language is the user expecting to hear .wavs in, "english" or another...
	//virtual void		GetUILanguage( char *dest, int destlen ) = 0;

	virtual void __Unknown_15(void) = 0;

	// Can skybox be seen from a particular point?
	//virtual SkyboxVisibility_t IsSkyboxVisibleFromPoint( const Vector &vecPoint ) = 0;

	// Get the pristine map entity lump string.  (e.g., used by CS to reload the map entities when restarting a round.)
	//virtual const char*	GetMapEntitiesString() = 0;

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode( void ) = 0;

	virtual bool __Unknown_16(void) = 0; //94

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	//virtual float		GetScreenAspectRatio() = 0;

	// allow the game UI to login a user
	//virtual bool		REMOVED_SteamRefreshLogin( const char *password, bool isSecure ) = 0;

	virtual bool __Unknown_17(void) = 0;

	//virtual bool		REMOVED_SteamProcessCall( bool & finished ) = 0;

	virtual void __Unknown_18(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	//virtual unsigned int	GetEngineBuildNumber() = 0; // engines build

	virtual void __Unknown_19(unsigned int dwUnknown1) = 0;

	//virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

	virtual void __Unknown_20(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0; //98

	// Communicates to the color correction editor that it's time to grab the pre-color corrected frame
	// Passes in the actual size of the viewport
	//virtual void			GrabPreColorCorrectedFrame( int x, int y, int width, int height ) = 0;

	virtual unsigned int __Unknown_20(unsigned int dwUnknown1) = 0;

	virtual unsigned int __Unknown_21(void) = 0;

	virtual bool			IsHammerRunning( ) const = 0; //101

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	//
	// Note: this is NOT checked against the FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	//virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;

	virtual void __Unknown_22(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;

	// returns if the loaded map was processed with HDR info. This will be set regardless
	// of what HDR mode the player is in.
	//virtual bool MapHasHDRLighting(void) = 0;

	virtual bool __Unknown_23(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;

	virtual bool __Unknown_24(unsigned int dwUnknown1) = 0; //104

	//virtual int	GetAppID() = 0;

	virtual unsigned int __Unknown_25(void) = 0; //105

	virtual const char* EngineVersion(void) = 0;

	// Just get the leaf ambient light - no caching, no samples
	//virtual Vector			GetLightForPointFast(const Vector &pos, bool bClamp) = 0;

	virtual void __Unknown_26(void) = 0;

	virtual bool __Unknown_27(void) = 0;

	// This version does NOT check against FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void			ClientCmd_Unrestricted( const char *szCmdString ) = 0; //109

	virtual bool __Unknown_28(void) = 0;

	// ** 110 methods confirmed **
	
	// This used to be accessible through the cl_restrict_server_commands cvar.
	// By default, Valve games restrict the server to only being able to execute commands marked with FCVAR_SERVER_CAN_EXECUTE.
	// By default, mods are allowed to execute any server commands, and they can restrict the server's ability to execute client
	// commands with this function.
	virtual void			SetRestrictServerCommands( bool bRestrict ) = 0;
	
	// If set to true (defaults to true for Valve games and false for others), then IVEngineClient::ClientCmd
	// can only execute things marked with FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void			SetRestrictClientCommands( bool bRestrict ) = 0;

	// Sets the client renderable for an overlay's material proxy to bind to
	virtual void			SetOverlayBindProxy( int iOverlayID, void *pBindProxy ) = 0;

	virtual bool			CopyFrameBufferToMaterial( const char *pMaterialName ) = 0;

	// Matchmaking
	virtual void			ChangeTeam( const char *pTeamName ) = 0;

	// Causes the engine to read in the user's configuration on disk
	virtual void			ReadConfiguration( const bool readDefault = false ) = 0; 

	virtual void SetAchievementMgr( IAchievementMgr *pAchievementMgr ) = 0;
	virtual IAchievementMgr *GetAchievementMgr() = 0;

	virtual bool			MapLoadFailed( void ) = 0;
	virtual void			SetMapLoadFailed( bool bState ) = 0;
	
	virtual bool			IsLowViolence() = 0;
	virtual const char		*GetMostRecentSaveGame( void ) = 0;
	virtual void			SetMostRecentSaveGame( const char *lpszFilename ) = 0;

	virtual void			StartXboxExitingProcess() = 0;
	virtual bool			IsSaveInProgress() = 0;
	virtual uint			OnStorageDeviceAttached( void ) = 0;
	virtual void			OnStorageDeviceDetached( void ) = 0;

	virtual void			ResetDemoInterpolation( void ) = 0;

	// Methods to set/get a gamestats data container so client & server running in same process can send combined data
	virtual void SetGamestatsData( CGamestatsData *pGamestatsData ) = 0;
	virtual CGamestatsData *GetGamestatsData() = 0;

	// Returns true if the engine is playing back a "locally recorded" demo, which includes
	// both SourceTV and replay demos, since they're recorded locally (on servers), as opposed
	// to a client recording a demo while connected to a remote server.
	virtual bool IsPlayingDemoALocallyRecordedDemo() = 0;
};

abstract_class _CClientEntityList {
public:
	/*
		536ADB00   55               PUSH EBP	; Backup EBP on stack
		536ADB01   8BEC             MOV EBP,ESP	; Copy ESP to EBP, stackpointer setup done
		536ADB03   8B45 08          MOV EAX,DWORD PTR SS:[EBP+8]	; Copy method argument (int entnum) to EAX
		536ADB06   8B44C1 28        MOV EAX,DWORD PTR DS:[ECX+EAX*8+28]	; Copy element value of this::m_EntityCacheInfo[entnum].m_pNetworkable to EAX as result
		536ADB0A   5D               POP EBP	; Restore old EBP from stack
		536ADB0B   C2 0400          RETN 4	; Restore return address and free argument space from stack


	*/
	virtual IClientNetworkable*	GetClientNetworkable( int entnum ) = 0;

	/*
		536ADB70   55               PUSH EBP	; Backup EBP on stack
		536ADB71   8BEC             MOV EBP,ESP	; Copy ESP to EBP, stackpointer setup done
		536ADB73   8B45 08          MOV EAX,DWORD PTR SS:[EBP+8]	; Copy method argument (int entnum) to EAX from stack
		536ADB76   51               PUSH ECX	; Push ECX onto stack just to create space as method argument for the following method call
		536ADB77   8BD4             MOV EDX,ESP	; Copy ESP to EDX (points to the method arg)
		536ADB79   8902             MOV DWORD PTR DS:[EDX],EAX	; Copy EAX (int entnum) as value to the method arg space on stack
		536ADB7B   8B01             MOV EAX,DWORD PTR DS:[ECX]	; Copy VFT address to EAX
		536ADB7D   FF50 08          CALL DWORD PTR DS:[EAX+8]	; Call method (this::GetListedEntity)
		536ADB80   8BC8             MOV ECX,EAX	; Copy method result from EAX to ECX
		536ADB82   85C9             TEST ECX,ECX	; Perform TEST operation of ECX with itself
		536ADB84   74 09            JE SHORT client.536ADB8F	; If zero then jump to address
		536ADB86   8B01             MOV EAX,DWORD PTR DS:[ECX]	; Copy VFT of IClientUnknown instance to EAX
		536ADB88   FF50 10          CALL DWORD PTR DS:[EAX+10]	; Call method (IClientUnknown::GetIClientEntity), result will be in EAX
		536ADB8B   5D               POP EBP	; Restore old EBP from stack
		536ADB8C   C2 0400          RETN 4	; Restore return address and free argument space from stack
		536ADB8F   33C0             XOR EAX,EAX	; Clear bits in EAX
		536ADB91   5D               POP EBP	; Restore old EBP from stack
		536ADB92   C2 0400          RETN 4	; Restore return address and free argument space from stack
	*/
	virtual IClientEntity*		GetClientEntity( int entnum ) = 0;
};

abstract_class _C_BaseEntity {
public:
	virtual void __Unknown_1(unsigned int dwUnknown) = 0;
	virtual void SetRefEHandle( const CBaseHandle &handle ) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual C_BaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual void* __Unknown_2() = 0;
	virtual void __Unknown_3() = 0;
	virtual void __Unknown_4() = 0;
	virtual void* __Unknown_5() = 0;
	virtual bool __Unknown_6(unsigned int dwUnknown) = 0;
	virtual bool __Unknown_7() = 0; //ECX+0x9CC
	virtual void* __Unknown_8() = 0;
	virtual bool __Unknown_9() = 0; //return false
	virtual void* __Unknown_10() = 0;
	virtual void* __Unknown_11() = 0;
	virtual void __Unknown_12(unsigned int dwUnknown) = 0;
	virtual bool __Unknown_13(void) = 0; //return false;
	virtual bool __Unknown_14(void) = 0; //return true;
	virtual bool __Unknown_15(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3, unsigned int dwUnknown4) = 0;
	virtual unsigned int __Unknown_16() = 0;
	virtual void __Unknown_17() = 0;
	virtual void __Unknown_18(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3) = 0;
	virtual void __Unknown_19(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual void __Unknown_20(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3) = 0;
	virtual unsigned int __Unknown_21() = 0;
	virtual unsigned int __Unknown_22() = 0;
	virtual void __Unknown_23(unsigned int dwUnknown) = 0;
	virtual void __Unknown_24(void) = 0;
	virtual void __Unknown_25(void) = 0;
	virtual void __Unknown_26(void) = 0;
	virtual void __Unknown_27(void) = 0;
	virtual void __Unknown_28(void) = 0;
	virtual void __Unknown_29(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual void __Unknown_30(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual void __Unknown_31(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual bool __Unknown_32(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual bool __Unknown_33(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3) = 0;
	virtual void __Unknown_34(void) = 0;
	virtual bool __Unknown_35(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual void* __Unknown_35(void) = 0;
	virtual void* __Unknown_36(void) = 0;
	virtual void __Unknown_37(unsigned int dwUnknown) = 0;
	virtual unsigned int __Unknown_38(void) = 0;
	virtual void __Unknown_39(void) = 0;
	virtual void __Unknown_40(void) = 0;
	virtual bool __Unknown_41(void) = 0;
public:
	unsigned char __ucPadding_1[0x60];
	int m_iIndex;
	unsigned long m_EntClientFlags;
	struct model_t* m_pModel;
	color32_s m_clrRender;
	unsigned char __ucPadding_2[11 * sizeof(unsigned int)];
	Vector m_vecAbsOrigin;
	Vector m_vecOrigin;
	unsigned char __ucPadding_3[12];
	QAngle m_vecViewangles;
	unsigned char __ucPadding_4[20];
	unsigned long m_iEFlags;
	char m_nWaterType;
	bool m_bDormant;
	unsigned char __ucPadding_5[2];
	int m_fEffects;
	int m_iTeamNum;
	unsigned char __ucPadding_6[8];
	int m_iHealth;
	char __ucPadding_7[68];
	EHANDLE m_hNetworkMoveParent, m_hOwnerEntity, m_hGroundEntity;
	char m_iName[MAX_PATH];
	short m_nModelIndex;
	unsigned char m_nRenderFX, m_nRenderMode, m_MoveType, m_MoveCollide, m_nWaterLevel;
	unsigned char m_lifeState;
};

abstract_class _IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect( CreateInterfaceFn factory ) = 0; //0
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	virtual unsigned int __Unknown_1(void) = 0;
	virtual unsigned int __Unknown_2(void) = 0;
	virtual void __Unknown_3(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
	virtual bool __Unknown_4(void) = 0; //8
};

abstract_class _IStudioRender : public _IAppSystem
{
public:
	virtual void BeginFrame( void ) = 0;
	virtual void EndFrame( void ) = 0;

	// Used for the mat_stub console command.
	virtual void Mat_Stub( IMaterialSystem *pMatSys ) = 0;

	// Updates the rendering configuration 
	virtual void UpdateConfig( const StudioRenderConfig_t& config ) = 0;
	virtual void GetCurrentConfig( StudioRenderConfig_t& config ) = 0;

	// Load, unload model data
	virtual bool LoadModel( studiohdr_t *pStudioHdr, void *pVtxData, studiohwdata_t	*pHardwareData ) = 0;
	virtual void UnloadModel( studiohwdata_t *pHardwareData ) = 0;

	// Refresh the studiohdr since it was lost...
	virtual void RefreshStudioHdr( studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData ) = 0;

	// This is needed to do eyeglint and calculate the correct texcoords for the eyes.
	virtual void SetEyeViewTarget( const studiohdr_t *pStudioHdr, int nBodyIndex, const Vector& worldPosition ) = 0;
		
	// Methods related to lighting state
	// NOTE: SetAmbientLightColors assumes that the arraysize is the same as 
	// returned from GetNumAmbientLightSamples
	virtual int GetNumAmbientLightSamples() = 0;
	virtual const Vector *GetAmbientLightDirections() = 0;
	virtual void SetAmbientLightColors( const Vector4D *pAmbientOnlyColors ) = 0;
	virtual void SetAmbientLightColors( const Vector *pAmbientOnlyColors ) = 0;
	virtual void SetLocalLights( int numLights, const LightDesc_t *pLights ) = 0;

	// Sets information about the camera location + orientation
	virtual void SetViewState( const Vector& viewOrigin, const Vector& viewRight, 
		const Vector& viewUp, const Vector& viewPlaneNormal ) = 0;
	
	// Allocates flex weights for use in rendering
	// NOTE: Pass in a non-null second parameter to lock delayed flex weights
	virtual void LockFlexWeights( int nWeightCount, float **ppFlexWeights, float **ppFlexDelayedWeights = NULL ) = 0;
	virtual void UnlockFlexWeights() = 0;
	
	// Used to allocate bone matrices to be used to pass into DrawModel
	virtual matrix3x4_t* LockBoneMatrices( int nBoneCount ) = 0;
	virtual void UnlockBoneMatrices() = 0;
	
	// LOD stuff
	virtual int GetNumLODs( const studiohwdata_t &hardwareData ) const = 0;
	virtual float GetLODSwitchValue( const studiohwdata_t &hardwareData, int lod ) const = 0;
	virtual void SetLODSwitchValue( studiohwdata_t &hardwareData, int lod, float switchValue ) = 0;

	// Sets the color/alpha modulation
	virtual void SetColorModulation( float const* pColor ) = 0;
	virtual void SetAlphaModulation( float flAlpha ) = 0;
	
	// Draws the model
	virtual void DrawModel( DrawModelResults_t *pResults, const DrawModelInfo_t& info, 
		matrix3x4_t *pBoneToWorld, float *pFlexWeights, float *pFlexDelayedWeights, const Vector &modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;

	// Methods related to static prop rendering
	virtual void DrawModelStaticProp( const DrawModelInfo_t& drawInfo, const matrix3x4_t &modelToWorld, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
	virtual void DrawStaticPropDecals( const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld ) = 0;
	virtual void DrawStaticPropShadows( const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld, int flags ) = 0;

	// Causes a material to be used instead of the materials the model was compiled with
	virtual void ForcedMaterialOverride( IMaterial *newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL ) = 0;

	// Create, destroy list of decals for a particular model
	virtual StudioDecalHandle_t CreateDecalList( studiohwdata_t *pHardwareData ) = 0;
	virtual void DestroyDecalList( StudioDecalHandle_t handle ) = 0;

	// Add decals to a decal list by doing a planar projection along the ray
	// The BoneToWorld matrices must be set before this is called
	virtual void AddDecal( StudioDecalHandle_t handle, studiohdr_t *pStudioHdr, matrix3x4_t *pBoneToWorld, 
		const Ray_t & ray, const Vector& decalUp, IMaterial* pDecalMaterial, float radius, int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS ) = 0;

	// Compute the lighting at a point and normal
	virtual void ComputeLighting( const Vector* pAmbient, int lightCount,
		LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting ) = 0;

	// Compute the lighting at a point, constant directional component is passed
	// as flDirectionalAmount
	virtual void ComputeLightingConstDirectional( const Vector* pAmbient, int lightCount,
		LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting, float flDirectionalAmount ) = 0;

	// Shadow state (affects the models as they are rendered)
	virtual void AddShadow( IMaterial* pMaterial, void* pProxyData, FlashlightState_t *m_pFlashlightState = NULL, VMatrix *pWorldToTexture = NULL, ITexture *pFlashlightDepthTexture = NULL ) = 0;
	virtual void ClearAllShadows() = 0;

	// Gets the model LOD; pass in the screen size in pixels of a sphere 
	// of radius 1 that has the same origin as the model to get the LOD out...
	virtual int ComputeModelLod( studiohwdata_t* pHardwareData, float unitSphereSize, float *pMetric = NULL ) = 0;

	// Return a number that is usable for budgets, etc.
	// Things that we care about:
	// 1) effective triangle count (factors in batch sizes, state changes, etc)
	// 2) texture memory usage
	// Get Triangles returns the LOD used
	virtual void GetPerfStats( DrawModelResults_t *pResults, const DrawModelInfo_t &info, CUtlBuffer *pSpewBuf = NULL ) const = 0;
	virtual void GetTriangles( const DrawModelInfo_t& info, matrix3x4_t *pBoneToWorld, GetTriangles_Output_t &out ) = 0;

	// Returns materials used by a particular model
	virtual int GetMaterialList( studiohdr_t *pStudioHdr, int count, IMaterial** ppMaterials ) = 0;
	virtual int GetMaterialListFromBodyAndSkin( MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials, IMaterial** ppOutputMaterials ) = 0;
	// draw an array of models with the same state
	virtual void DrawModelArray( const DrawModelInfo_t &drawInfo, int arrayCount, model_array_instance_t *pInstanceData, int instanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL ) = 0;
};

typedef void* OverlayText_t;
abstract_class _IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0; //???
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void __Unknown_1(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3, unsigned int dwUnknown4, unsigned int dwUnknown5, unsigned int dwUnknown6, unsigned int dwUnknown7, unsigned int dwUnknown8, unsigned int dwUnknown9) = 0;
	virtual void __Unknown_2(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3, unsigned int dwUnknown4, unsigned int dwUnknown5, unsigned int dwUnknown6, unsigned int dwUnknown7, unsigned int dwUnknown8) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b,bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float duration, const char *format, ...) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float duration, const char *format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos,float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vector& origin) = 0;
	
	virtual unsigned int __Unknown_3(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;
	virtual int ScreenPosition(const Vector& point, Vector& screen) = 0;

	virtual OverlayText_t *GetFirst( void ) = 0;
	virtual OverlayText_t *GetNext( OverlayText_t *current ) = 0;
	virtual void ClearDeadOverlays( void ) = 0;
	virtual void ClearAllOverlays() = 0; //18

	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;

	virtual void AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2( const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, const Color& faceColor, const Color& edgeColor, float duration ) = 0;

private:
	inline void AddTextOverlay(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) {} /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};

class _ISurface : public _IAppSystem
{
public:
	virtual void ___Unknown_1(void) = 0;
	
	// hierarchy root
	virtual VPANEL GetEmbeddedPanel() = 0;
	virtual void SetEmbeddedPanel( VPANEL pPanel ) = 0;

	// drawing context
	virtual void PushMakeCurrent(VPANEL panel, bool useInsets) = 0;
	virtual void PopMakeCurrent(VPANEL panel) = 0;

	// rendering functions
	virtual void DrawSetColor(Color col) = 0;
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawFilledRectArray( void *pRects, int numRects ) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;

	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawPolyLine(int *px, int *py, int numPoints) = 0;

	virtual void DrawSetTextFont(HANDLE font) = 0;
	virtual void ___Unknown_2(void) = 0;
	virtual void ___Unknown_3(unsigned int dwUnknown) = 0; //24
	virtual void DrawSetTextColor(Color col) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int& x,int& y) = 0; //28
	virtual void DrawPrintText(const wchar_t *text, int textLen, int drawType ) = 0;
	virtual void DrawUnicodeChar(wchar_t wch, int drawType ) = 0; //30

	virtual void DrawFlushText() = 0;		// flushes any buffered text (for rendering optimizations)
	virtual HANDLE *CreateHTMLWindow(vgui::IHTMLEvents *events,VPANEL context)=0; //32
	virtual void PaintHTMLWindow(vgui::IHTML *htmlwin) =0;
	virtual void DeleteHTMLWindow(HANDLE *htmlwin)=0;

	virtual int	 DrawGetTextureId( char const *filename ) = 0;
	virtual bool DrawGetTextureFile(int id, char *filename, int maxlen ) = 0; //36
	virtual void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
	virtual void ___Unknown_4(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3, unsigned int dwUnknown4) = 0;
	//virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)=0;
	virtual void DrawSetTexture(int id) = 0;
	virtual bool IsTextureIDValid(int id) = 0; //40
	virtual void DrawGetTextureSize(int id, int &wide, int &tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	
	virtual bool ___Unknown_5(unsigned int dwUnknown1) = 0;
	virtual int CreateNewTextureID( bool procedural = false ) = 0; //44
#ifdef _X360
	virtual void DestroyTextureID( int id ) = 0;
	virtual void UncacheUnusedMaterials() = 0;
#endif

	virtual void GetScreenSize(int &wide, int &tall) = 0;
	virtual void SetAsTopMost(VPANEL panel, bool state) = 0;
	virtual void BringToFront(VPANEL panel) = 0;
	virtual void SetForegroundWindow (VPANEL panel) = 0; //48
	virtual void SetPanelVisible(VPANEL panel, bool state) = 0;
	virtual void SetMinimized(VPANEL panel, bool state) = 0;
	virtual bool IsMinimized(VPANEL panel) = 0;
	virtual void FlashWindow(VPANEL panel, bool state) = 0; //52
	virtual void SetTitle(VPANEL panel, const wchar_t *title) = 0;
	virtual void SetAsToolBar(VPANEL panel, bool state) = 0;		// removes the window's task bar entry (for context menu's, etc.)

	// windows stuff
	virtual void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true , bool kbInput = true) = 0;
	virtual void SwapBuffers(VPANEL panel) = 0; //56
	virtual void Invalidate(VPANEL panel) = 0;
	virtual void SetCursor(HANDLE cursor) = 0;
	virtual bool IsCursorVisible() = 0;
	virtual void ApplyChanges() = 0; //60
	virtual bool IsWithin(int x, int y) = 0;
	virtual bool HasFocus() = 0;
	
	// returns true if the surface supports minimize & maximize capabilities
	enum SurfaceFeature_e
	{
		ANTIALIASED_FONTS	= 1,
		DROPSHADOW_FONTS	= 2,
		ESCAPE_KEY			= 3,
		OPENING_NEW_HTML_WINDOWS = 4,
		FRAME_MINIMIZE_MAXIMIZE	 = 5,
		OUTLINE_FONTS	= 6,
		DIRECT_HWND_RENDER		= 7,
	};
	virtual bool SupportsFeature(SurfaceFeature_e feature) = 0;

	virtual bool ___Unknown_6(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0; //64

	// restricts what gets drawn to one panel and it's children
	// currently only works in the game
	virtual void RestrictPaintToSinglePanel(VPANEL panel) = 0;

	// these two functions obselete, use IInput::SetAppModalSurface() instead
	virtual VPANEL GetModalPanel() = 0;

	virtual void UnlockCursor() = 0;
	virtual void LockCursor() = 0; //68

	virtual void SetTranslateExtendedKeys(bool state) = 0;
	virtual VPANEL GetTopmostPopup() = 0;

	// engine-only focus handling (replacing WM_FOCUS windows handling)
	virtual void SetTopLevelFocus(VPANEL panel) = 0;

	// fonts
	// creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
	virtual HANDLE CreateFont() = 0; //72

	// adds to the font
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC			= 0x001,
		FONTFLAG_UNDERLINE		= 0x002,
		FONTFLAG_STRIKEOUT		= 0x004,
		FONTFLAG_SYMBOL			= 0x008,
		FONTFLAG_ANTIALIAS		= 0x010,
		FONTFLAG_GAUSSIANBLUR	= 0x020,
		FONTFLAG_ROTARY			= 0x040,
		FONTFLAG_DROPSHADOW		= 0x080,
		FONTFLAG_ADDITIVE		= 0x100,
		FONTFLAG_OUTLINE		= 0x200,
		FONTFLAG_CUSTOM			= 0x400,		// custom generated font - never fall back to asian compatibility mode
		FONTFLAG_BITMAP			= 0x800,		// compiled bitmap font - no fallbacks
	};

	virtual bool SetFontGlyphSet(HANDLE font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) = 0;

	// adds a custom font file (only supports true type font files (.ttf) for now)
	virtual bool AddCustomFontFile(const char *fontFileName) = 0; //74

	// returns the details about the font
	virtual int GetFontTall(HANDLE font) = 0;
	virtual int GetFontAscent(HANDLE font, wchar_t wch) = 0;
	virtual bool IsFontAdditive(HANDLE font) = 0;
	virtual void GetCharABCwide(HANDLE font, int ch, int &a, int &b, int &c) = 0;
	virtual int GetCharacterWidth(HANDLE font, int ch) = 0;
	virtual void GetTextSize(HANDLE font, const wchar_t *text, int &wide, int &tall) = 0;

	// notify icons?!?
	virtual VPANEL GetNotifyPanel() = 0;
	virtual void SetNotifyIcon(VPANEL context, HANDLE icon, VPANEL panelToReceiveMessages, const char *text) = 0;

	// plays a sound
	virtual void PlaySound(const char *fileName) = 0;

	//!! these functions should not be accessed directly, but only through other vgui items
	//!! need to move these to seperate interface
	virtual int GetPopupCount() = 0;
	virtual VPANEL GetPopup(int index) = 0;
	virtual bool ShouldPaintChildPanel(VPANEL childPanel) = 0;
	virtual bool RecreateContext(VPANEL panel) = 0;
	virtual void AddPanel(VPANEL panel) = 0;
	virtual void ReleasePanel(VPANEL panel) = 0;
	virtual void MovePopupToFront(VPANEL panel) = 0;
	virtual void MovePopupToBack(VPANEL panel) = 0;

	virtual void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) = 0;
	virtual void PaintTraverse(VPANEL panel) = 0;

	virtual void EnableMouseCapture(VPANEL panel, bool state) = 0;

	// returns the size of the workspace
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;

	// gets the absolute coordinates of the screen (in windows space)
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;

	// gets the base resolution used in proportional mode
	virtual void GetProportionalBase( int &width, int &height ) = 0;

	virtual void CalculateMouseVisible() = 0;
	virtual bool NeedKBInput() = 0;

	virtual bool HasCursorPosFunctions() = 0;
	virtual void SurfaceGetCursorPos(int &x, int &y) = 0;
	virtual void SurfaceSetCursorPos(int x, int y) = 0;


	// SRC only functions!!!
	virtual void DrawTexturedLine( const HANDLE &a, const HANDLE &b ) = 0;
	virtual void DrawOutlinedCircle(int x, int y, int radius, int segments) = 0;
	virtual void DrawTexturedPolyLine( const HANDLE *p,int n ) = 0; // (Note: this connects the first and last points).
	virtual void DrawTexturedSubRect( int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1 ) = 0;
	virtual void DrawTexturedPolygon(int n, HANDLE *pVertices) = 0;
	virtual const wchar_t *GetTitle(VPANEL panel) = 0;
	virtual bool IsCursorLocked( void ) const = 0;
	virtual void SetWorkspaceInsets( int left, int top, int right, int bottom ) = 0;

	// Lower level char drawing code, call DrawGet then pass in info to DrawRender
	virtual bool DrawGetUnicodeCharRenderInfo( wchar_t ch, HANDLE& info ) = 0;
	virtual void DrawRenderCharFromInfo( const HANDLE& info ) = 0;

	// global alpha setting functions
	// affect all subsequent draw calls - shouldn't normally be used directly, only in Panel::PaintTraverse()
	virtual void DrawSetAlphaMultiplier( float alpha /* [0..1] */ ) = 0;
	virtual float DrawGetAlphaMultiplier() = 0;

	// web browser
	virtual void SetAllowHTMLJavaScript( bool state ) = 0;

	// video mode changing
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;

	virtual vgui::HCursor CreateCursorFromFile( char const *curOrAniFile, char const *pPathID = 0 ) = 0;

	// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
	virtual IVguiMatInfo *DrawGetTextureMatInfoFactory( int id ) = 0;

	virtual void PaintTraverseEx(VPANEL panel, bool paintPopups = false ) = 0;

	virtual float GetZPos() const = 0;

	// From the Xbox
	virtual void SetPanelForInput( VPANEL vpanel ) = 0;
	virtual void DrawFilledRectFade( int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0;
	virtual void DrawSetTextureRGBAEx(int id, const unsigned char *rgba, int wide, int tall, ImageFormat imageFormat ) = 0;
	virtual void DrawSetTextScale(float sx, float sy) = 0;
	virtual bool SetBitmapFontGlyphSet(HANDLE font, const char *windowsFontName, float scalex, float scaley, int flags) = 0;
	// adds a bitmap font file
	virtual bool AddBitmapFontFile(const char *fontFileName) = 0;
	// sets a symbol for the bitmap font
	virtual void SetBitmapFontName( const char *pName, const char *pFontFilename ) = 0;
	// gets the bitmap font filename
	virtual const char *GetBitmapFontName( const char *pName ) = 0;
	virtual void ClearTemporaryFontCache( void ) = 0;

	virtual HANDLE *GetIconImageForFullPath( char const *pFullPath ) = 0;
	virtual void DrawUnicodeString( const wchar_t *pwString, int drawType ) = 0;
	virtual void PrecacheFontCharacters(HANDLE font, wchar_t *pCharacters) = 0;
	// Console-only.  Get the string to use for the current video mode for layout files.
	virtual const char *GetResolutionKey( void ) const = 0;
};

abstract_class _IGameEventListener2
{
public:
	virtual	~_IGameEventListener2( void ) {};

	/*
		//Event class handler from engine:
		55F275A0   FF50 08          CALL DWORD PTR DS:[EAX+8]	; Call IndicateEventHandling() method
		55F275A3   83F8 2A          CMP EAX,2A	; Compare result value in EAX with 0x2A
		55F275A6   74 1C            JE SHORT engine.55F275C4	; If equal jump to part where to proceed handling
		...
		55F275C4   8B03             MOV EAX,DWORD PTR DS:[EBX]	; Copy DWORD at [EBX] to EAX (address to VFT of this class instance)
		55F275C6   8BCB             MOV ECX,EBX	; Copy EBX to ECX	; Copy base address of this class instance to ECX as this-pointer
		55F275C8   57               PUSH EDI	; Push EDI onto stack as function argument (holds IGameEvent instance address)
		55F275C9   FF50 04          CALL DWORD PTR DS:[EAX+4]	; Call HandleGameEvent() method

		//Example HandleGameEvent() method
		55EC6390   55               PUSH EBP	; Backup old EBP value onto stack
		55EC6391   8BEC             MOV EBP,ESP	; Copy value from ESP to EBP, stack frame pointer setup done
		55EC6393   83EC 10          SUB ESP,10	; Allocate 0x10 bytes on stack
		55EC6396   57               PUSH EDI	; Backup EDI onto stack
		55EC6397   8BF9             MOV EDI,ECX	; Copy ECX to EDI (class instance this-pointer)
		55EC6399   807F 08 00       CMP BYTE PTR DS:[EDI+8],0	; Compare byte at [EDI+0x08] with 0
		55EC639D   0F84 6F030000    JE engine.55EC6712	; If equal then jump to address
		...
		55EC6712   5F               POP EDI	; Restore EDI from stack
		55EC6713   8BE5             MOV ESP,EBP	; Copy value of EBP to ESP to free the allocated 0x10 stack space
		55EC6715   5D               POP EBP	; Restore old frame pointer
		55EC6716   C2 0400          RETN 4	; Restore return address and free 4 bytes of argument stack space

		//Example IndicateEventHandling() method
		55D964E0   8B41 04          MOV EAX,DWORD PTR DS:[ECX+4] ; Should copy 0x2A to EAX if HandleGameEvent() shall get called
		55D964E3   C3               RETN	; Restore return address
	*/

	//Called when indicated that the event shall be handled
	virtual void HandleGameEvent(abstract_class _IGameEvent *pEvent) = 0;

	//Called to indicate whether 'HandleGameEvent' shall be called or not
	virtual int IndicateEventHandling(void) = 0;
};

abstract_class _IGameEventManager2 {
public:
	virtual int __Unknown_1(unsigned int dwUnknown) = 0;

	// load game event descriptions from a file eg "resource\gameevents.res"
	virtual int LoadEventsFromFile(const char *filename) = 0;

	// removes all and anything
	virtual void  Reset() = 0;

	// adds a listener for a particular event
	virtual bool AddListener(_IGameEventListener2 *listener, const char *name, bool bServerSide) = 0;

	// returns true if this listener is listens to given event
	virtual bool FindListener(IGameEventListener2 *listener, const char *name) = 0;

	// removes a listener 
	virtual int RemoveListener(IGameEventListener2 *listener) = 0;

	// create an event by name, but doesn't fire it. returns NULL is event is not
	// known or no listener is registered for it. bForce forces the creation even if no listener is active
	virtual IGameEvent *CreateEvent(const char *name, bool bForce, unsigned int dwUnknown) = 0;

	// fires a server event created earlier, if bDontBroadcast is set, event is not send to clients
	virtual bool FireEvent(IGameEvent *event, bool bDontBroadcast = false) = 0;

	// fires an event for the local client only, should be used only by client code
	virtual bool FireEventClientSide(IGameEvent *event) = 0;

	// create a new copy of this event, must be free later
	virtual IGameEvent *DuplicateEvent(IGameEvent *event) = 0;

	// if an event was created but not fired for some reason, it has to bee freed, same UnserializeEvent
	virtual void FreeEvent(IGameEvent *event) = 0;

	// write/read event to/from bitbuffer
	virtual bool SerializeEvent(IGameEvent *event, bf_write *buf) = 0;

	// create new KeyValues, must be deleted
	virtual IGameEvent *UnserializeEvent( bf_read *buf ) = 0;
};

abstract_class _IGameEvent
{
public:
	virtual ~_IGameEvent() {};
	virtual const char *GetName() const = 0;	// get event name

	virtual bool  IsReliable() const = 0; // if event handled reliable
	virtual bool  IsLocal() const = 0; // if event is never networked
	virtual bool  IsEmpty(const char *keyName = NULL) = 0; // check if data field exists
	
	// Data access
	virtual bool  GetBool(const char *keyName = NULL, bool defaultValue = false) = 0;
	virtual int   GetInt(const char *keyName = NULL, int defaultValue = 0) = 0;
	virtual __int64 GetInt64(const char *keyName = NULL, __int64 defaultValue = 0) = 0;
	virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f) = 0;
	virtual const char *GetString(const char *keyName = NULL, const char *defaultValue = "") = 0;
	virtual void* __Unknown_1(const char *keyName = NULL, const char *defaultValue = "") = 0;

	virtual void SetBool(const char *keyName, bool value) = 0;
	virtual void SetInt(const char *keyName, int value) = 0;
	virtual void SetFloat(const char *keyName, float value) = 0;
	virtual void SetString(const char *keyName, const char *value) = 0;
};

class _IInputSystem : public _IAppSystem {
public:
	// Attach, detach input system from a particular window
	// This window should be the root window for the application
	// Only 1 window should be attached at any given time.
	virtual void AttachToWindow( void* hWnd ) = 0; //9
	virtual void DetachFromWindow( ) = 0;

	// Enables/disables input. PollInputState will not update current 
	// button/analog states when it is called if the system is disabled.
	virtual void EnableInput( bool bEnable ) = 0;

	// Enables/disables the windows message pump. PollInputState will not
	// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump( bool bEnable ) = 0;

	// Polls the current input state
	virtual void PollInputState(unsigned int dwUnknown) = 0;

	// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0; //14

	// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown( ButtonCode_t code ) const = 0; //15

	// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick( ButtonCode_t code ) const = 0;
	virtual int GetButtonReleasedTick( ButtonCode_t code ) const = 0;

	// Gets the value of an analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogValue( AnalogCode_t code ) const = 0;

	// Gets the change in a particular analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogDelta( AnalogCode_t code ) const = 0; //19

	// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData( ) const = 0;

	// Posts a user-defined event into the event queue; this is expected
	// to be called in overridden wndprocs connected to the root panel.
	virtual bool ___Unknown_1(void) = 0;
	//virtual void PostUserEvent( const InputEvent_t &event ) = 0;

	// Returns the number of joysticks
	//virtual int GetJoystickCount() const = 0;
	virtual void ___Unknown_2(void) = 0;
	virtual void ___Unknown_3(void) = 0;

	virtual unsigned int ___Unknown_4(void) = 0;
	virtual unsigned __int64 ___Unknown_5(void) = 0; //27
	virtual void ___Unknown_6(unsigned int) = 0;
	virtual void ___Unknown_7(void) = 0;
	virtual void ___Unknown_8(void) = 0;
	virtual void ___Unknown_9(void) = 0; //31
	virtual void ___Unknown_10(unsigned int dwUnknown) = 0;
	virtual unsigned int ___Unknown_11(void) = 0;
	virtual void ___Unknown_12(unsigned int dwUnknown, bool bUnknown ) = 0;
	// Enable/disable joystick, it has perf costs
	//virtual void EnableJoystickInput( int nJoystick, bool bEnable ) = 0;

	// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV( int nJoystick, bool bEnable ) = 0; //35

	// Sample the joystick and append events to the input queue
	virtual void SampleDevices( void ) = 0;

	// FIXME: Currently force-feedback is only supported on the Xbox 360
	virtual void SetRumble( float fLeftMotor, float fRightMotor, int userId = INVALID_USER_ID ) = 0;
	//virtual void StopRumble( void ) = 0;
	virtual void ___Unknown_13(unsigned int dwUnknown) = 0;

	// Resets the input state
	virtual void ResetInputState() = 0; //39

	// Sets a player as the primary user - all other controllers will be ignored.
	virtual void SetPrimaryUserId( int userId ) = 0;

	// Convert back + forth between ButtonCode/AnalogCode + strings
	virtual const char *ButtonCodeToString( ButtonCode_t code ) const = 0;
	virtual const char *AnalogCodeToString( AnalogCode_t code ) const = 0;

	// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput( int nMaxSleepTimeMS = -1 ) = 0; //44

	virtual ButtonCode_t StringToButtonCode( const char *pString ) const = 0; //45
	//virtual AnalogCode_t StringToAnalogCode( const char *pString ) const = 0;

	// Convert back + forth between virtual codes + button codes
	// FIXME: This is a temporary piece of code
	virtual ButtonCode_t VirtualKeyToButtonCode( int nVirtualKey ) const = 0; //46
	virtual int ButtonCodeToVirtualKey( ButtonCode_t code ) const = 0; //47
	//virtual ButtonCode_t ScanCodeToButtonCode( int lParam ) const = 0;

	// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0; //48

	// Sets the cursor position
	virtual void SetCursorPosition( int x, int y ) = 0; //49
};

abstract_class _IInput
{
public:
	// Initialization/shutdown of the subsystem
	virtual	void		Init_All( void ) = 0;
	virtual void		Shutdown_All( void ) = 0;
	// Latching button states
	virtual int			GetButtonBits( int ) = 0;
	// Create movement command
	virtual void		CreateMove ( int sequence_number, float input_sample_frametime, bool active ) = 0;
	virtual void		ExtraMouseSample( float frametime, bool active ) = 0;
	virtual bool		WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand, unsigned int dwUnknown ) = 0;
	virtual void		EncodeUserCmdToBuffer( bf_write& buf, int slot, unsigned int dwUnknown ) = 0;
	virtual void		DecodeUserCmdFromBuffer( bf_read& buf, int slot, unsigned int dwUnknown ) = 0;

	virtual CUserCmd	*GetUserCmd( int slot, int sequence_number ) = 0;

	virtual void		MakeWeaponSelection( C_BaseCombatWeapon *weapon ) = 0;

	// Retrieve key state
	virtual float		KeyState ( struct kbutton_t *key ) = 0;
	// Issue key event
	virtual int			KeyEvent( int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding ) = 0;
	// Look for key
	virtual struct kbutton_t	*FindKey( const char *name ) = 0;

	// Issue commands from controllers
	virtual void		ControllerCommands( void ) = 0;

	/* 14 methods confirmed */

	// Extra initialization for some joysticks
	virtual void		Joystick_Advanced( void ) = 0;
	virtual void		Joystick_SetSampleTime( float frametime ) = 0;
	virtual void		IN_SetSampleTime( float frametime ) = 0;

	// Accumulate mouse delta
	virtual void		AccumulateMouse( void ) = 0;
	// Activate/deactivate mouse
	virtual void		ActivateMouse( void ) = 0;
	virtual void		DeactivateMouse( void ) = 0;

	// Clear mouse state data
	virtual void		ClearStates( void ) = 0;
	// Retrieve lookspring setting
	virtual float		GetLookSpring( void ) = 0;

	// Retrieve mouse position
	virtual void		GetFullscreenMousePos( int *mx, int *my, int *unclampedx = 0, int *unclampedy = 0 ) = 0;
	virtual void		SetFullscreenMousePos( int mx, int my ) = 0;
	virtual void		ResetMouse( void ) = 0;
	virtual	float		GetLastForwardMove( void ) = 0;

	// Third Person camera ( TODO/FIXME:  Move this to a separate interface? )
	virtual void		CAM_Think( void ) = 0;
	virtual int			CAM_IsThirdPerson( void ) = 0;
	virtual void		CAM_GetCameraOffset( Vector& ofs ) = 0;
	virtual void		CAM_ToThirdPerson(void) = 0;
	virtual void		CAM_ToFirstPerson(void) = 0;
	virtual void		CAM_StartMouseMove(void) = 0;
	virtual void		CAM_EndMouseMove(void) = 0;
	virtual void		CAM_StartDistance(void) = 0;
	virtual void		CAM_EndDistance(void) = 0;
	virtual int			CAM_InterceptingMouse( void ) = 0;

	// orthographic camera info	( TODO/FIXME:  Move this to a separate interface? )
	virtual void		CAM_ToOrthographic() = 0;
	virtual	bool		CAM_IsOrthographic() const = 0;
	virtual	void		CAM_OrthographicSize( float& w, float& h ) const = 0;

#if defined( HL2_CLIENT_DLL )
	// IK back channel info
	virtual void		AddIKGroundContactInfo( int entindex, float minheight, float maxheight ) = 0;
#endif

	virtual void		LevelInit( void ) = 0;

	// Causes an input to have to be re-pressed to become active
	virtual void		ClearInputButton( int bits ) = 0;

	virtual	void		CAM_SetCameraThirdData( struct CameraThirdData_t *pCameraData, const QAngle &vecCameraOffset ) = 0;
	virtual void		CAM_CameraThirdThink( void ) = 0;
};

abstract_class _IEngineTrace
{
public:
	virtual unsigned int __Unknown_1(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3) = 0;

	// Returns the contents mask + entity at a particular world-space position
	virtual int		GetPointContents( const Vector &vecAbsPosition, IHandleEntity** ppEntity = NULL ) = 0;
	
	// Get the point contents, but only test the specific entity. This works
	// on static props and brush models.
	//
	// If the entity isn't a static prop or a brush model, it returns CONTENTS_EMPTY and sets
	// bFailed to true if bFailed is non-null.
	virtual int		GetPointContents_Collideable( ICollideable *pCollide, const Vector &vecAbsPosition ) = 0;

	// Traces a ray against a particular entity
	virtual void	ClipRayToEntity( const Ray_t &ray, unsigned int fMask, IHandleEntity *pEnt, trace_t *pTrace ) = 0;

	// Traces a ray against a particular entity
	virtual void	ClipRayToCollideable( const Ray_t &ray, unsigned int fMask, ICollideable *pCollide, trace_t *pTrace ) = 0;

	// A version that simply accepts a ray (can work as a traceline or tracehull)
	virtual void	TraceRay( const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace ) = 0; //5

	// A version that sets up the leaf and entity lists and allows you to pass those in for collision.
	virtual void	SetupLeafAndEntityListRay( const Ray_t &ray, CTraceListData &traceData ) = 0;
	virtual void    SetupLeafAndEntityListBox( const Vector &vecBoxMin, const Vector &vecBoxMax, CTraceListData &traceData ) = 0;
	virtual void	TraceRayAgainstLeafAndEntityList( const Ray_t &ray, CTraceListData &traceData, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace ) = 0;

	// A version that sweeps a collideable through the world
	// abs start + abs end represents the collision origins you want to sweep the collideable through
	// vecAngles represents the collision angles of the collideable during the sweep
	virtual void	SweepCollideable( ICollideable *pCollide, const Vector &vecAbsStart, const Vector &vecAbsEnd, 
		const QAngle &vecAngles, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace ) = 0;

	// Enumerates over all entities along a ray
	// If triggers == true, it enumerates all triggers along a ray
	virtual void	EnumerateEntities( const Ray_t &ray, bool triggers, IEntityEnumerator *pEnumerator ) = 0;

	// Same thing, but enumerate entitys within a box
	virtual void	EnumerateEntities( const Vector &vecAbsMins, const Vector &vecAbsMaxs, IEntityEnumerator *pEnumerator ) = 0;

	// Convert a handle entity to a collideable.  Useful inside enumer
	virtual ICollideable *GetCollideable( IHandleEntity *pEntity ) = 0; //12

	// HACKHACK: Temp for performance measurments
	virtual int GetStatByIndex( int index, bool bClear ) = 0;


	//finds brushes in an AABB, prone to some false positives
	virtual unsigned int __Unknown_2(unsigned int dwUnknown1, unsigned int dwUnknown2, unsigned int dwUnknown3, unsigned int dwUnknown4, unsigned int dwUnknown5) = 0;
	//virtual void GetBrushesInAABB( const Vector &vMins, const Vector &vMaxs, CUtlVector<int> *pOutput, int iContentsMask = 0xFFFFFFFF ) = 0;

	//Creates a CPhysCollide out of all displacements wholly or partially contained in the specified AABB
	virtual CPhysCollide* GetCollidableFromDisplacementsInAABB( const Vector& vMins, const Vector& vMaxs ) = 0;

	virtual unsigned int __Unknown_3(void) = 0;
	virtual void __Unknown_4(unsigned int dwUnknown1, unsigned int dwUnknown2) = 0;
};
//======================================================================

//======================================================================
LPVOID FindClientExport(HMODULE hClientDll, LPCSTR lpszExportName);
CreateInterfaceFn FindAppSystemFactory(header_info_s* pEngineDllHeader);
ICvar* FindICvarClassInstance(header_info_s* pClientDllHeader);
class IInput* FindIInputClassInstance(header_info_s* pClientDllHeader);
//======================================================================

#endif
