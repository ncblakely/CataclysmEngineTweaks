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

	typedef void (*fn_trailUpdate)(shiptrail* trail, vector* position);
	inline fn_trailUpdate trailUpdate = (fn_trailUpdate)0x005A4E10;

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

	typedef bool (*fn_univUpdate)(real32 phystimeelapsed);
	inline fn_univUpdate univUpdate = (fn_univUpdate)0x0054C3F0;

	typedef bool (*fn_SaveGame)(const char* filename);
	inline fn_SaveGame SaveGame = (fn_SaveGame)0x004FDAF0;

	typedef void (*fn_clCommandMessage)(const char CommandMessage[MAX_MESSAGE_LENGTH], udword flags);
	inline fn_clCommandMessage clCommandMessage = (fn_clCommandMessage)0x004AC820;

	typedef BOOL(*fn_glCapNT)();
	inline fn_glCapNT glCapNT = (fn_glCapNT)0x00559AC0;
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

	inline BOOL* showBorder = (BOOL*)0x008B8628;
	inline BOOL* fullScreen = (BOOL*)0x008B8638;
	inline BOOL* enableAVI = (BOOL*)0x008B8610;

	inline BOOL* singlePlayerGame = (BOOL*)0x00A43C9C;

	inline Universe* universe = (Universe*)0x00B62A00;

	inline float* taskFrequency = (float*)0x00BB3260;

	inline SinglePlayerGameInfo* singlePlayerGameInfo = (SinglePlayerGameInfo*)0x00C0A5C0;

	inline LARGE_INTEGER* realtimeremainder = (LARGE_INTEGER*)0x00A6D9A0;

	inline LONGLONG* utyTimerDivisor = (LONGLONG*)0x00AB7520;
}