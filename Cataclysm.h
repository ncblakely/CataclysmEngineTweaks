/// Main header for defines and constants based on the Cataclysm executable.

#pragma once

#include "trails.h"
#include "rinit.h"
#include "UIcontrols.h"
#include "spaceobj.h"
#include "universe.h"
#include "singleplayer.h"
#include "glcaps.h"

#define DEFINE_ADDRESS(name, address) \
	inline void* name = (void*)address;

// Addresses of specific instructions in game functions.
namespace Instructions
{
	// Push phystimeelapsed argument
	DEFINE_ADDRESS(univUpdate1, 0x00524841);
	// Push phystimeelapsed argument
	DEFINE_ADDRESS(univUpdate2, 0x005256B7);
	// Push phystimeelapsed argument
	DEFINE_ADDRESS(univUpdate3, 0x0053E8A5);
	// Push phystimeelapsed argument
	DEFINE_ADDRESS(univUpdate4, 0x0053E8DB);
	// Push phystimeelapsed argument
	DEFINE_ADDRESS(univUpdate5, 0x0053E9B5);

	// Push UNIVERSE_UPDATE_PERIOD argument
	DEFINE_ADDRESS(utyGameSystemsInit_UNIVERSE_UPDATE_PERIOD, 0x005AC174);

	// Push UNIVERSE_UPDATE_PERIOD argument
	DEFINE_ADDRESS(nisStartup_UNIVERSE_UPDATE_PERIOD, 0x004CC8F2);

	// timeElapsed = UNIVERSE_UPDATE_PERIOD;
	DEFINE_ADDRESS(nisUpdateTask_UNIVERSE_UPDATE_PERIOD_1, 0x004CC8BE);
	// Push UNIVERSE_UPDATE_PERIOD argument to nisUpdate
	DEFINE_ADDRESS(nisUpdateTask_UNIVERSE_UPDATE_PERIOD_2, 0x004CC801);

	// Push UNIVERSE_UPDATE_PERIOD argument
	DEFINE_ADDRESS(BabyTasksStartup_UNIVERSE_UPDATE_PERIOD, 0x0052AF12);

	// Push UNIVERSE_UPDATE_PERIOD argument
	DEFINE_ADDRESS(univUpdateReset_UNIVERSE_UPDATE_PERIOD, 0x0054ACDB);

	// Check for harvest keyframe
	DEFINE_ADDRESS(CollectResources_HarvestRateCheck, 0x004F17C9);

	DEFINE_ADDRESS(utyGameSystemsInit_TimerResolutionMax, 0x005ABF68);

	// Resource regrowth check in univUpdate
	DEFINE_ADDRESS(startUnivCheckRegrowResources, 0x0054CD73);
	DEFINE_ADDRESS(endUnivCheckRegrowResources, 0x0054CD8F);

	// Research update
	DEFINE_ADDRESS(startRmUpdateResearch, 0x0054C83D);
	DEFINE_ADDRESS(endRmUpdateResearch, 0x0054C859);

	DEFINE_ADDRESS(collectResources, 0x004F17D5); // Add resources to worker
	DEFINE_ADDRESS(skipCollectResources, 0x004F1A46); // Skip adding resources to worker

	DEFINE_ADDRESS(gunShipFirePower_UniverseUpdatePeriod1, 0x0048CBD0);
	DEFINE_ADDRESS(gunShipFirePower_UniverseUpdatePeriod2, 0x0048CC57);

	DEFINE_ADDRESS(etgDefineAt_UpdateRate1, 0x0046E042);
	DEFINE_ADDRESS(etgDefineAt_UpdateRate2, 0x0046E04F);
	DEFINE_ADDRESS(etgDefineAt_UpdateRate3, 0x0046E05B);

	DEFINE_ADDRESS(nisUpdateTask_UniverseUpdatePeriod1, 0x004CC634);
	DEFINE_ADDRESS(nisUpdateTask_UniverseUpdatePeriod2, 0x004CC701);
	DEFINE_ADDRESS(nisUpdateTask_UniverseUpdatePeriod3, 0x004CC72C);
	DEFINE_ADDRESS(nisUpdateTask_UniverseUpdatePeriod4, 0x004CC7B8);

	DEFINE_ADDRESS(InitWindow_DeviceCrcCheck, 0x00563563);

	DEFINE_ADDRESS(ActivateMe_ReInitRenderer, 0x00562084);

	DEFINE_ADDRESS(CheckPlayerWin_GameTypeCheck, 0x0054860C);
	DEFINE_ADDRESS(CheckPlayerWin_IsMultiplayerGame, 0x00548619);
}

// Addresses of game functions.
namespace Functions
{
	typedef void (*fn_univUpdateReset)();
	inline fn_univUpdateReset univUpdateReset = (fn_univUpdateReset)0x0054ACD0;

	typedef void (*fn_setframesCB)(char* directory, char* field, void* dataToFillIn);
	inline fn_setframesCB scriptSetFramesCB = (fn_setframesCB)0x0053AC70;

	typedef void (*fn_singlePlayerGameUpdate)();
	inline fn_singlePlayerGameUpdate singlePlayerGameUpdate = (fn_singlePlayerGameUpdate)0x005197D0;

	typedef void (*fn_UpdateMidLevelHyperspacingShips)();
	inline fn_UpdateMidLevelHyperspacingShips UpdateMidLevelHyperspacingShips = (fn_UpdateMidLevelHyperspacingShips)0x00519700;

	typedef void (*fn_mistrailUpdate)(missiletrail* trail, vector* position);
	inline fn_mistrailUpdate mistrailUpdate = (fn_mistrailUpdate)0x005A4D90;

	typedef void (*fn_trailSegmentsRead)(char* directory, char* field, void* dataToFillIn);
	inline fn_trailSegmentsRead trailSegmentsRead = (fn_trailSegmentsRead)0x005A7A70;

	typedef void (*fn_rinSortModes)(rdevice* dev);
	inline fn_rinSortModes rinSortModes = (fn_rinSortModes)0x0057DE00;

	typedef HRESULT (__stdcall *fn_rinEnumDisplayModes_cb)(LPDDSURFACEDESC2 ddsd, LPVOID lpContext);
	inline fn_rinEnumDisplayModes_cb rinEnumDisplayModes_cb = (fn_rinEnumDisplayModes_cb)0x0057E5C0;
	inline fn_rinEnumDisplayModes_cb rinEnumPrimaryDisplayModes_cb = (fn_rinEnumDisplayModes_cb)0x0057E710;

	typedef void* (*fn_memAlloc)(size_t size);
	inline fn_memAlloc memAlloc = (fn_memAlloc)0x006725D0;

	typedef void (*fn_memFree)(void* ptr);
	inline fn_memFree memFree = (fn_memFree)0x00673080;

	typedef void (*fn_mgGameTypesOtherButtonPressed)();
	inline fn_mgGameTypesOtherButtonPressed mgGameTypesOtherButtonPressed = (fn_mgGameTypesOtherButtonPressed)0x004C2590;

    typedef void (*fn_uicTextEntrySet)(textentryhandle entry, char* text, sdword cursorPos);
    inline fn_uicTextEntrySet uicTextEntrySet = (fn_uicTextEntrySet)0x00538980;

    typedef void (*fn_uicTextBufferResize)(textentryhandle entry, sdword size);
    inline fn_uicTextBufferResize uicTextBufferResize = (fn_uicTextBufferResize)0x005388F0;

    typedef void (*fn_uicTextEntryInit)(textentryhandle entry, udword flags);
    inline fn_uicTextEntryInit uicTextEntryInit = (fn_uicTextEntryInit)0x00538A30;

    typedef void (*fn_feToggleButtonSet)(char* name, sdword bPressed);
    inline fn_feToggleButtonSet feToggleButtonSet = (fn_feToggleButtonSet)0x00475990;

    typedef void (*fn_mgResourceInjectionInterval)(char* name, featom* atom);
    inline fn_mgResourceInjectionInterval mgResourceInjectionInterval = (fn_mgResourceInjectionInterval)0x004BDE80;

	typedef void (*fn_mgResourceLumpSumInterval)(char* name, featom* atom);
	inline fn_mgResourceLumpSumInterval mgResourceLumpSumInterval = (fn_mgResourceLumpSumInterval)0x004BE090;

	typedef void (*fn_univCheckRegrowResources)();
	inline fn_univCheckRegrowResources univCheckRegrowResources = (fn_univCheckRegrowResources)0x0054C270;

	typedef void (*fn_rmUpdateResearch)();
	inline fn_rmUpdateResearch rmUpdateResearch = (fn_rmUpdateResearch)0x004F2A20;

	typedef void (*fn_BeastMothershipSelfDamage)(ShipStaticInfo* shipstatic);
	inline fn_BeastMothershipSelfDamage beastMothershipSelfDamage = (fn_BeastMothershipSelfDamage)0x005BC900;

    typedef void (*fn_univBulletCollidedWithTarget)(int unknown, SpaceObjRotImpTarg* target, StaticHeader* targetstaticheader, Bullet* bullet, real32 collideLineDist, sdword collSide);
	inline fn_univBulletCollidedWithTarget univBulletCollidedWithTarget = (fn_univBulletCollidedWithTarget)0x00546A20;

	typedef int (*fn_lodLevelGet)(void* spaceObj, vector* camera, vector* ship);
	inline fn_lodLevelGet lodLevelGet = (fn_lodLevelGet)0x004A4D70;

	typedef BOOL(*fn_etgFrequencyExceeded)(etgeffectstatic* stat);
	inline fn_etgFrequencyExceeded etgFrequencyExceeded = (fn_etgFrequencyExceeded)0x00470BD0;

	typedef bool (*fn_univUpdate)(real32 phystimeelapsed);
	inline fn_univUpdate univUpdate = (fn_univUpdate)0x0054C3F0;

	typedef bool (*fn_SaveGame)(const char* filename);
	inline fn_SaveGame SaveGame = (fn_SaveGame)0x004FDAF0;

	typedef void (*fn_clCommandMessage)(const char CommandMessage[MAX_MESSAGE_LENGTH], udword flags);
	inline fn_clCommandMessage clCommandMessage = (fn_clCommandMessage)0x004AC820;

	typedef BOOL(*fn_glCapNT)();
	inline fn_glCapNT glCapNT = (fn_glCapNT)0x00559AC0;

	typedef sdword (*fn_etgFunctionCall)(Effect* effect, struct etgeffectstatic* stat, ubyte* opcode);
	inline fn_etgFunctionCall etgFunctionCall = (fn_etgFunctionCall)0x0046E7B0;

	typedef void (*fn_etgEffectDelete)(Effect* effect);
	inline fn_etgEffectDelete etgEffectDelete = (fn_etgEffectDelete)0x0046A6F0;

	typedef sdword (*fn_etgNParticleBlocksSet)(struct etgeffectstatic* stat, ubyte* dest, char* opcodeString, char* params, char* ret);
	inline fn_etgNParticleBlocksSet etgNParticleBlocksSet = (fn_etgNParticleBlocksSet)0x0046E110;

	typedef void(*fn_trCramRAMComputeAndScale)();
	inline fn_trCramRAMComputeAndScale trCramRAMComputeAndScale = (fn_trCramRAMComputeAndScale)0x00589A50;

	typedef udword (*fn_rinDeviceCRC)();
	inline fn_rinDeviceCRC rinDeviceCRC = (fn_rinDeviceCRC)0x0057DCF0;

	typedef void(*fn_mainRescaleMainWindow)();
	inline fn_mainRescaleMainWindow mainRescaleMainWindow = (fn_mainRescaleMainWindow)0x0055FC70;

	typedef void (*fn_opOptionsAccept)(char* name, featom* atom);
	inline fn_opOptionsAccept opOptionsAccept = (fn_opOptionsAccept)0x004D7E30;

	typedef void (*fn_glCapStartup)();
	inline fn_glCapStartup glCapStartup = (fn_glCapStartup)0x00559BE0;

	typedef sdword(*fn_soundStartDSound)(HWND hWnd);
	inline fn_soundStartDSound soundStartDSound = (fn_soundStartDSound)0x005808E0;

	typedef int (*fn_fqEqualize)(float* aBlock, float* aEq);
	inline fn_fqEqualize fqEqualize = (fn_fqEqualize)0x00558F20;
}

// Global/static variables in the game executable.
namespace Globals
{
	inline float* HYPERSPACE_SLICE_RATE = (float*)0x0089410C;
	inline int* RegenerateRURate = (int*)0x0088F580;
	inline udword* universe_univUpdateCounter = (udword*)0x00B62BAC;
	inline float* TimerResolutionMax = (float*)0x0072A614;

	// ETG constants embedded in the data segment
	inline float* etgEffectorCI_UniverseUpdateRate = (float*)0x0072A82C;
	inline float* ETG_UpdateRoundOff = (float*)0x0072A818; // (UNIVERSE_UPDATE_PERIOD / 2.0f) = 0.03125

	inline textentryhandle* mgResourceInjectionIntervalEntryBox = (textentryhandle*)0x00A4C6C4;
	inline textentryhandle* mgResourceLumpSumIntervalEntryBox = (textentryhandle*)0x00A4C6CC;

	inline CaptainGameInfo* tpGameCreated = (CaptainGameInfo*)0x00886028;

	// Command line options
	inline bool32* showBorder = (bool32*)0x008B8628;
	inline bool32* fullScreen = (bool32*)0x008B8638;
	inline bool32* enableAVI = (bool32*)0x008B8610;
	inline bool32* glCapCompiledVertexArray = (bool32*)0x00A72548;

	inline bool32* multiPlayerGame = (bool32*)0x00A43C94;
	inline bool32* singlePlayerGame = (bool32*)0x00A43C9C;

	inline Universe* universe = (Universe*)0x00B62A00;

	inline float* taskFrequency = (float*)0x00BB3260;

	inline SinglePlayerGameInfo* singlePlayerGameInfo = (SinglePlayerGameInfo*)0x00C0A5C0;

	inline LARGE_INTEGER* realtimeremainder = (LARGE_INTEGER*)0x00A6D9A0;

	inline LONGLONG* utyTimerDivisor = (LONGLONG*)0x00AB7520;

	inline float* etgTotalTimeElapsed = (float*)0x00C41C74;

	inline udword* opDeviceCRC = (udword*)0x00A4CC20;

	// Resolution/renderer selection
	inline char* glToSelect = (char*)0x008B8644;
	inline sdword* MAIN_WindowWidth = (sdword*)0x008B85E8;
	inline sdword* MAIN_WindowHeight = (sdword*)0x008B85EC;
	inline sdword* MAIN_WindowDepth = (sdword*)0x008B85F0;
	inline real32* rndAspectRatio = (real32*)0x00B377A4;

	inline sdword** SongNumberLoadGameOffset = (sdword**)0x00892670;

	// DirectSound globals
	inline LPDIRECTSOUND* lpDirectSound = (LPDIRECTSOUND*)0x00A75F28;
	inline LPDIRECTSOUNDBUFFER* lpPrimaryBuffer = (LPDIRECTSOUNDBUFFER*)0x00A75F2C;
	inline bool32* bDirectSoundCertified = (bool32*)0x00A75F3C;
	inline WAVEFORMATEX* waveFormatEX = (WAVEFORMATEX*)0x00B06C80;
	inline bool32* coopDSound = (bool32*)0x00A74DC8;
}