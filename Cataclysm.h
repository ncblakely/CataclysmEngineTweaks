/// Main header for defines and constants based on the Cataclysm executable.

#pragma once

#include "trails.h"
#include "rinit.h"
#include "UIcontrols.h"
#include "spaceobj.h"
#include "universe.h"
#include "singleplayer.h"
#include "glcaps.h"
#include "AISkirmish.h"

#define DEFINE_ADDRESS(name, address) \
	inline void* name = (void*)address

#define DEFINE_FUNCTION(name, ret, signature, address) \
	typedef ret (*fn_##name) signature; \
	inline fn_##name name = (fn_##name)##address

#define DEF_VAR(type, p) type p = (type)a1

// Generic pointer template to make the syntax for dereferencing multiple levels of indirection a bit less cumbersome
template <typename T>
class GamePointer
{
public:
	__forceinline explicit GamePointer(unsigned int offset) { m_offset = (T**)offset; }

	__forceinline T* operator->() const { return *m_offset; }
	__forceinline operator T* () const { return *m_offset; }

protected:
	T** m_offset;
};

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

	DEFINE_ADDRESS(partRenderBillSystem_SetPointFiltering, 0x004DD43E);
	DEFINE_ADDRESS(partRenderBillSystem_AfterSetPointFiltering, 0x004DD468);

	DEFINE_ADDRESS(LoadMission_AdjustTextureMemoryLimit, 0x00589AEA);
	DEFINE_ADDRESS(LoadMission_AfterTextureMemoryLimitAdjustment, 0x00589B1F);

	DEFINE_ADDRESS(WindowProc_JumpIfF12KeyUp, 0x00562401);
	DEFINE_ADDRESS(WindowProc_NormalKeyUpEvent, 0x00562943);

	DEFINE_ADDRESS(allianceFormWith_UniverseUpdatePeriod, 0x0042090D);

	// AI external ship building
	DEFINE_ADDRESS(aiUpdateExternalShipBuild_CalculateTimerDuration, 0x00543F48);
	DEFINE_ADDRESS(aiUpdateExternalShipBuild_SetTimerDuration, 0x00543F51);

	// .big file
	DEFINE_ADDRESS(utyGameSystemsPreInit_UpdateBigFileName1, 0x005ABB21);
	DEFINE_ADDRESS(utyGameSystemsPreInit_UpdateBigFileName2, 0x005ABB3E);
	DEFINE_ADDRESS(utyGameSystemsPreInit_UpdateBigFileName3, 0x005ABB5C);
}

// Addresses of game functions.
namespace Functions
{
	DEFINE_FUNCTION(aisFleetUpdate, void, (), 0x00431750);
	DEFINE_FUNCTION(aisRequestShip, void, (Player* player, ShipType shipType, sdword buildCost), 0x00431510);
	DEFINE_FUNCTION(allianceFormWith, void, (udword playerindex), 0x00420880);
	DEFINE_FUNCTION(beastMothershipSelfDamage, void, (ShipStaticInfo* shipstatic), 0x005BC900);
	DEFINE_FUNCTION(clCommandMessage, void, (const char CommandMessage[MAX_MESSAGE_LENGTH], udword flags), 0x004AC820);
	DEFINE_FUNCTION(clWrapCreateShip, void, (CommandLayer* comlayer, ShipType shipType, ShipRace shipRace, uword playerIndex, ShipPtr creator), 0x00452F30);
	DEFINE_FUNCTION(etgEffectDelete, void, (Effect* effect), 0x0046A6F0);
	DEFINE_FUNCTION(etgFrequencyExceeded, bool32, (etgeffectstatic* stat), 0x00470BD0);
	DEFINE_FUNCTION(etgFunctionCall, sdword, (Effect* effect, struct etgeffectstatic* stat, ubyte* opcode), 0x0046E7B0);
	DEFINE_FUNCTION(etgNParticleBlocksSet, sdword, (struct etgeffectstatic* stat, ubyte* dest, char* opcodeString, char* params, char* ret), 0x0046E110);
	DEFINE_FUNCTION(feToggleButtonSet, void, (char* name, sdword bPressed), 0x00475990);
	DEFINE_FUNCTION(fqEqualize, int, (float* aBlock, float* aEq), 0x00558F20);
	DEFINE_FUNCTION(glCapNT, bool32, (), 0x00559AC0);
	DEFINE_FUNCTION(glCapStartup, void, (), 0x00559BE0);
	DEFINE_FUNCTION(lodLevelGet, int, (void* spaceObj, vector* camera, vector* ship), 0x004A4D70);
	DEFINE_FUNCTION(mainRescaleMainWindow, void, (), 0x0055FC70);
	DEFINE_FUNCTION(memAlloc, void*, (size_t size), 0x006725D0);
	DEFINE_FUNCTION(memFree, void, (void* ptr), 0x00673080);
	DEFINE_FUNCTION(mgGameTypesOtherButtonPressed, void, (), 0x004C2590);
	DEFINE_FUNCTION(mgResourceInjectionInterval, void, (char* name, featom* atom), 0x004BDE80);
	DEFINE_FUNCTION(mgResourceLumpSumInterval, void, (char* name, featom* atom), 0x004BE090);
	DEFINE_FUNCTION(mistrailUpdate, void, (missiletrail* trail, vector* position), 0x005A4D90);
	DEFINE_FUNCTION(opOptionsAccept, void, (char* name, featom* atom), 0x004D7E30);
	DEFINE_FUNCTION(rinDeviceCRC, udword, (), 0x0057DCF0);
	DEFINE_FUNCTION(rinEnumDisplayModes_cb, HRESULT, (LPDDSURFACEDESC2 ddsd, LPVOID lpContext), 0x0057E5C0);
	DEFINE_FUNCTION(rinEnumPrimaryDisplayModes_cb, HRESULT, (LPDDSURFACEDESC2 ddsd, LPVOID lpContext), 0x0057E710);
	DEFINE_FUNCTION(rinSortModes, void, (rdevice* dev), 0x0057DE00);
	DEFINE_FUNCTION(rmCanBuildShip, bool32, (Player* player, ShipType type, int p3), 0x004F2630);
	DEFINE_FUNCTION(rmUpdateResearch, void, (), 0x004F2A20);
	DEFINE_FUNCTION(SaveGame, bool32, (const char* filename), 0x004FDAF0);
	DEFINE_FUNCTION(scriptSet, void, (const char* directory, const char* filename, scriptEntry info[]), 0x00526420);
	DEFINE_FUNCTION(scriptSetFramesCB, void, (char* directory, char* field, void* dataToFillIn), 0x0053AC70);
	DEFINE_FUNCTION(scriptSetTweakableGlobals, void, (), 0x00527820);
	DEFINE_FUNCTION(scriptSetUdwordCB, void, (char* directory, char* field, void* dataToFillIn), 0x00525C20);
	DEFINE_FUNCTION(ShipTypeToStr, char*, (ShipType shiptype), 0x004D6BC0);
	DEFINE_FUNCTION(selNumShipsInSelection, int, (MaxSelection* sel, ShipType shipType), 0x005118D0);
	DEFINE_FUNCTION(singlePlayerGameUpdate, void, (), 0x005197D0);
	DEFINE_FUNCTION(soundStartDSound, sdword, (HWND hWnd), 0x005808E0);
	DEFINE_FUNCTION(speechEventQueue, sdword, (void* object, sdword event, sdword var, sdword variation, sdword actornum, sdword playernum, sdword linkto, real32 timeout, sword volume), 0x00522560);
	DEFINE_FUNCTION(trailSegmentsRead, void, (char* directory, char* field, void* dataToFillIn), 0x005A7A70);
	DEFINE_FUNCTION(trCramRAMComputeAndScale, void, (), 0x00589A50);
	DEFINE_FUNCTION(uicTextBufferResize, void, (textentryhandle entry, sdword size), 0x005388F0);
	DEFINE_FUNCTION(uicTextEntryInit, void, (textentryhandle entry, udword flags), 0x00538A30);
	DEFINE_FUNCTION(uicTextEntrySet, void, (textentryhandle entry, char* text, sdword cursorPos), 0x00538980);
	DEFINE_FUNCTION(univBulletCollidedWithTarget, void, (int unknown, SpaceObjRotImpTarg* target, StaticHeader* targetstaticheader, Bullet* bullet, real32 collideLineDist, sdword collSide), 0x00546A20);
	DEFINE_FUNCTION(univCheckRegrowResources, void, (), 0x0054C270);
	DEFINE_FUNCTION(univUpdate, bool32, (real32 phystimeelapsed), 0x0054C3F0);
	DEFINE_FUNCTION(univUpdateReset, void, (), 0x0054ACD0);
	DEFINE_FUNCTION(UpdateMidLevelHyperspacingShips, void, (), 0x00519700);
	DEFINE_FUNCTION(ranRandomFn, udword, (sdword ranIndex), 0x004EF860);
	DEFINE_FUNCTION(GetShipStaticInfo, ShipStaticInfo*, (ShipType shiptype), 0x0053EA50);
	DEFINE_FUNCTION(clWrapSetKamikaze, void, (CommandLayer* comlayer, SelectCommand* selectcom), 0x00453820);
	DEFINE_FUNCTION(clWrapScuttle, void, (CommandLayer* comlayer, SelectCommand* selectcom), 0x00453390);
	DEFINE_FUNCTION(clWrapBuildShip, void, (CommandLayer* comlayer, ShipType shipType, ShipRace shipRace, uword playerIndex, ShipPtr creator), 0x00452FA0);
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

	inline scriptStructEntry* ShipStaticScriptTable = (scriptStructEntry*)0x008B2428;

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

	// Version number
	inline char* networkVersion = (char*)0x008B8850;
	inline char* minorBuildVersion = (char*)0x008B8860;

	inline sdword* dbgInt3Enabled = (sdword*)0x0087922C;

	// AI
	inline udword* aiBuildingShip = (udword*)0x008DFC64; // Array, TOTAL_STD_SHIPS
	inline GamePointer<Ship> aiCarrier1Ship(0x008DFC38);
	inline GamePointer<Ship> aiCarrier2Ship(0x008DFC3C);
	inline GamePointer<AIPlayer> aiCurrentAIPlayer(0x008DF604);
	inline AISTeamEntry* aisTeams = (AISTeamEntry*)0x008DFD60; // Array, NUM_AIS_TEAMS
	inline bool32* aiHasExternalConstruction = (bool32*)0x00A2D984;
	inline bool32* aiHasCarrier1ModuleQueued = (bool32*)0x00A2D98C;
	inline bool32* aiHasSupportModuleQueued = (bool32*)0x00A2D994;
	inline bool32* aiHasCarrier2ModuleQueued = (bool32*)0x00A2D9A8;
	inline sdword* aiSupportUnitsPending = (sdword*)0x00A2D9B4;
}