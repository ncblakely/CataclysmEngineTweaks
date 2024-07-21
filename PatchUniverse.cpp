#include "pch.h"

#include "GameState.h"
#include "PatchUniverse.h"

#include "Cataclysm.h"
#include "particle.h"

using namespace fmt;

///////////////////////////////////////////////////////////////
// Prototypes for replicated game code

void mgResourceInjectionInterval(char* name, featom* atom);
void mgResourceLumpSumInterval(char* name, featom* atom);

///////////////////////////////////////////////////////////////
// Timestep constants

// Default HS speed factor for Cataclysm (note: calculated using the overridden HYPERSPACE_SLICE_RATE of 0.025 from the script files, instead of the engine default of 0.075).
// 16 FPS engine update rate * 2.5 = 40 FPS effective hyperspace update rate
const float HyperspaceUpdateSpeedFactor = 2.5;

static float s_AdjustedHyperspaceSliceRate = 0.025f;
static int s_AdjustedRegenerateRURate = 255;

// Local copies of config variables so they can be referenced in naked functions
static float s_UniverseUpdateRateFloat;
static udword s_UniverseUpdateRate;
static float s_UniverseUpdatePeriod;
static udword s_UniverseUpdateRateFactor;
static udword s_UniverseUpdateRateShift;
static sdword s_TimerResolutionMax;

///////////////////////////////////////////////////////////////

// macro to adapt the original update rate to the higher one
// (Credit for this goes to the Gardens of Kadesh project.)
#define UNIVERSE_WOODPECKER(r, f) (((*Globals::universe_univUpdateCounter % s_UniverseUpdateRateFactor) == 0) && (((*Globals::universe_univUpdateCounter >> s_UniverseUpdateRateShift) & r) == (f)))

typedef void (*fn_univUpdateReset)();
static fn_univUpdateReset orig_univUpdateReset;
static void univUpdateReset()
{
	spdlog::info("Universe reset called");

	if (g_Config.GetUniverseUpdateRateShift() > 0)
	{
		*Globals::HYPERSPACE_SLICE_RATE = s_AdjustedHyperspaceSliceRate;
		*Globals::RegenerateRURate = s_AdjustedRegenerateRURate;

		*Globals::etgEffectorCI_UniverseUpdateRate = -s_UniverseUpdateRateFloat;
		*Globals::ETG_UpdateRoundOff = s_UniverseUpdatePeriod / 2.0f;
	}

	g_GameState.ResetAutosaveTimer();

	orig_univUpdateReset();
}

static void scriptSetFramesCB(char* directory, char* field, void* dataToFillIn)
{
	sscanf(field, "%d", (sdword*)dataToFillIn);
	*((sdword*)dataToFillIn) *= (sdword)g_Config.GetUniverseUpdateRate();
}

static void* orig_CollectResourcesHarvestRateCheck;
static udword s_currentShipHarvestRate; // Local copy of the current ship's harvest rate
static void __declspec(naked) CollectResourcesHarvestRateCheck()
{
	__asm
	{
		pushad
		mov eax, [ecx+0x25C]
		mov [s_currentShipHarvestRate], eax
	}
	// s_currentShipHarvestRate = shipstatic->harvestRate;

	// Check if we need to collect this frame
	if (UNIVERSE_WOODPECKER(s_currentShipHarvestRate, 0))
	{
		__asm
		{
			popad
			jmp Instructions::collectResources
		}
	}
	else
	{
		__asm
		{
			popad
			jmp Instructions::skipCollectResources
		}
	}
}

static Functions::fn_mistrailUpdate orig_mistrailUpdate;
static void mistrailUpdate(missiletrail* trail, vector* position)
{
	if ((*Globals::universe_univUpdateCounter % g_Config.GetUniverseUpdateRateFactor()) == 0)
	{
		orig_mistrailUpdate(trail, position);
	}
}

static Functions::fn_trailSegmentsRead orig_trailSegmentsRead;
static void trailSegmentsRead(char* directory, char* field, void* dataToFillIn)
{
	sdword tpNSegments;
	sscanf(field, "%d", &tpNSegments);

	// Adjust the number of segments for the new update rate
	tpNSegments <<= g_Config.GetUniverseUpdateRateShift();

	orig_trailSegmentsRead(directory, (char*)fmt::format("{}", tpNSegments).c_str(), dataToFillIn);
}

static Functions::fn_beastMothershipSelfDamage orig_BeastMothershipSelfDamage;
static void beastMothershipSelfDamage(ShipStaticInfo* shipstatic)
{
	if ((*Globals::universe_univUpdateCounter % g_Config.GetUniverseUpdateRateFactor()) == 0)
	{
		orig_BeastMothershipSelfDamage(shipstatic);
	}
}

udword REGROW_RESOURCES_CHECK_RATE = 255;
udword REGROW_RESOURCES_CHECK_FRAME = 86;
static void __declspec(naked) startUnivCheckRegrowResources()
{
	__asm pushad

	if (UNIVERSE_WOODPECKER(REGROW_RESOURCES_CHECK_RATE, REGROW_RESOURCES_CHECK_FRAME))
	{
		Functions::univCheckRegrowResources();
	}

	__asm 
	{
		popad
		jmp Instructions::endUnivCheckRegrowResources
	}
}

constexpr sdword REFRESH_RESEARCH_RATE = 15;
constexpr sdword REFRESH_RESEARCH_FRAME = 13;
static void __declspec(naked) startRmUpdateResearch()
{
	__asm pushad

	if (UNIVERSE_WOODPECKER(REFRESH_RESEARCH_RATE, REFRESH_RESEARCH_FRAME))
	{
		Functions::rmUpdateResearch();
	}

	__asm
	{
		popad
		jmp Instructions::endRmUpdateResearch
	}
}

static void __declspec(naked) aiUpdateExternalShipBuild_CalculateTimerDuration()
{
	__asm 
	{
		// EAX = ShipStaticInfo->buildTime
		push edx
		xor edx, edx
		mov ecx, dword ptr ds: [s_UniverseUpdateRate] // Divide by the new update rate
		div ecx
		pop edx
		jmp Instructions::aiUpdateExternalShipBuild_SetTimerDuration
	}
}

bool IsMothershipOrModule(ShipType shiptype)
{
	switch (shiptype)
	{
		case sMothership:
		case sMothershipSpecial:
		case sMothershipEngineering:
		case sMothershipArmour:
		case sMothershipMicro:
		case sMothershipWeapons:
		case sMothershipDockingBay:
		case bMothership:
		case bMothershipDockingBay:
		case bMothershipSupport:
		case bMothershipEnginesLower:
			return true;
		default:
			return false;
	}
}

static Functions::fn_univBulletCollidedWithTarget orig_univBulletCollidedWithTarget;
static void univBulletCollidedWithTarget(int unknown, SpaceObjRotImpTarg* target, StaticHeader* targetstaticheader, Bullet* bullet, real32 collideLineDist, sdword collSide)
{
	// Hack to fix self-damage caused by buggy firing arcs on Somtaaw/Beast motherships
	if (target &&
		target->objtype == OBJ_ShipType &&
		bullet->owner)
	{
		Ship* bulletOwner = bullet->owner;
		Ship* targetShip = (Ship*)target;
		Player* targetOwner = targetShip->playerowner;
		Player* bulletOwnerPlayer = bulletOwner->playerowner;

		if (IsMothershipOrModule(targetShip->shiptype) &&
			targetOwner == bulletOwnerPlayer)
		{
			// Skip collision
			return;
		}
	}
	// Adjust for higher update rates: skip the bullet collision for beam weapons if we aren't on the correct frame
	if (bullet->bulletType == BULLET_Beam)
	{
		if ((*Globals::universe_univUpdateCounter % g_Config.GetUniverseUpdateRateFactor()) == 0)
		{
			orig_univBulletCollidedWithTarget(unknown, target, targetstaticheader, bullet, collideLineDist, collSide);
		}
	}
	else
	{
		orig_univBulletCollidedWithTarget(unknown, target, targetstaticheader, bullet, collideLineDist, collSide);
	}
}


static Functions::fn_singlePlayerGameUpdate orig_singlePlayerGameUpdate;
static void singlePlayerGameUpdate()
{
#ifdef _DEBUG
	using namespace Globals;

	spdlog::info("***Tick***\n"
		"realtimeelapsed = {}\n"
		"phystimeelapsed = {}\n"
		"totaltimeelapsed = {}\n"
		"univUpdateCounter = {}\n",
		universe->realtimeelapsed,
		universe->phystimeelapsed,
		universe->totaltimeelapsed,
		universe->univUpdateCounter);
#endif

	orig_singlePlayerGameUpdate();
}

static void CalculateTimestepConstants(Config& config)
{
	// Copy configs to local statics
	s_UniverseUpdateRateFloat = g_Config.GetUniverseUpdateRate();
	s_UniverseUpdatePeriod = g_Config.GetUniverseUpdatePeriod();
	s_UniverseUpdateRateFactor = g_Config.GetUniverseUpdateRateFactor();
	s_UniverseUpdateRateShift = g_Config.GetUniverseUpdateRateShift();
	s_UniverseUpdateRate = (udword)g_Config.GetUniverseUpdateRate();

	// Update game constants
	s_AdjustedHyperspaceSliceRate = 1.0f / (s_UniverseUpdateRateFloat * HyperspaceUpdateSpeedFactor); // Hyperspace slice rate should be inverse of 2.5x update rate
	s_AdjustedRegenerateRURate = (16 * (int)s_UniverseUpdateRateFloat) - 1; // RUs should regenerate once every 16s
	s_TimerResolutionMax = g_Config.GetTimerResolutionMax();
}

typedef BOOL(WINAPI* fn_QueryPerformanceCounter)(LARGE_INTEGER* lpPerformanceCount);
fn_QueryPerformanceCounter orig_QueryPerformanceCounter;

// Hook to normalize QueryPerformanceCounter values.
// This corrects for an already-existing bug in the game where high PC uptimes cause the new QueryPerformanceCounter()-based universe timer 
// added in Cataclysm to stop advancing.
// Most notably, this could cause bugs like the initial fade in/fade out at the start or end of an SP mission failing to complete.
// This required a very high uptime to occur at 16 FPS, but it will occur with much lower uptimes at 64 FPS or higher.
// 
// See this blog post for a good explanation of the issue: https://cookieplmonster.github.io/2018/08/07/high-resolution-timers-and-uptime-headaches/
// The "right" way to fix this would be to change universe.realtimeelapsed to a double, but due to the high number of references to it in the game code,
// this would be a lot of work.
static BOOL WINAPI QueryPerformanceCounterHook(LARGE_INTEGER* lpPerformanceCount)
{
	static bool queryPerformanceCounterCalled = false;
	static LARGE_INTEGER initialPerformanceCounter;

	orig_QueryPerformanceCounter(lpPerformanceCount);

	// On the first call, save the initial performance counter value
	if (!queryPerformanceCounterCalled)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		initialPerformanceCounter.QuadPart = lpPerformanceCount->QuadPart - frequency.QuadPart;

		queryPerformanceCounterCalled = true;
	}

	// Subtract the initial value to always effectively start from 0 regardless of PC uptime.
	lpPerformanceCount->QuadPart = lpPerformanceCount->QuadPart - initialPerformanceCounter.QuadPart;

	return TRUE;
}

// Install hooks
static void CreateAndEnableHooks(Assembler& assembler)
{
	void* _discard = nullptr;

	// Function hooks
	CreateAndEnableHook(Functions::univUpdateReset, univUpdateReset, &orig_univUpdateReset);
	CreateAndEnableHook(Functions::scriptSetFramesCB, scriptSetFramesCB, &_discard);
	CreateAndEnableHook(Functions::mgResourceInjectionInterval, mgResourceInjectionInterval, &_discard);
	CreateAndEnableHook(Functions::mgResourceLumpSumInterval, mgResourceLumpSumInterval, &_discard);
	CreateAndEnableHook(Functions::beastMothershipSelfDamage, beastMothershipSelfDamage, &orig_BeastMothershipSelfDamage);
	CreateAndEnableHook(Functions::univBulletCollidedWithTarget, univBulletCollidedWithTarget, &orig_univBulletCollidedWithTarget);
	CreateAndEnableHook(Functions::singlePlayerGameUpdate, singlePlayerGameUpdate, &orig_singlePlayerGameUpdate);
	CreateAndEnableHook(Functions::trailSegmentsRead, trailSegmentsRead, &orig_trailSegmentsRead);

	// Mid-function hooks
	CreateAndEnableHook(Instructions::CollectResources_HarvestRateCheck, CollectResourcesHarvestRateCheck, &_discard);
	CreateAndEnableHook(Instructions::startUnivCheckRegrowResources, startUnivCheckRegrowResources, &_discard);
	CreateAndEnableHook(Instructions::startRmUpdateResearch, startRmUpdateResearch, &_discard);
	CreateAndEnableHook(Instructions::aiUpdateExternalShipBuild_CalculateTimerDuration, aiUpdateExternalShipBuild_CalculateTimerDuration, &_discard);

	// External library hooks
	CreateAndEnableHook(&QueryPerformanceCounter, &QueryPerformanceCounterHook, &orig_QueryPerformanceCounter);
}

static void PatchEtgDefineAt(Assembler& assembler)
{
	// Replace some embedded constants for ETG_UpdateRoundOff / 2
	// (can't update the data segment constant because it's used in other places)
	static float s_ETGUpdateRoundOffDividedBy2 = s_UniverseUpdatePeriod / 2.0f / 2.0f;
	std::string etgUpdateRoundOffHex = IntToHex((sdword)&s_ETGUpdateRoundOffDividedBy2);

	assembler.Write(fmt::format("fcom dword ptr ds:[{}]", etgUpdateRoundOffHex), Instructions::etgDefineAt_UpdateRate1);
	assembler.Write(fmt::format("fsub dword ptr ds:[{}]", etgUpdateRoundOffHex), Instructions::etgDefineAt_UpdateRate2);
	assembler.Write(fmt::format("fld dword ptr ds:[{}]", etgUpdateRoundOffHex), Instructions::etgDefineAt_UpdateRate3);
}

static void PatchNisUpdateTask(Assembler& assembler)
{
	std::string universeUpdatePeriodAddressStr = IntToHex(sdword(&s_UniverseUpdatePeriod));

	assembler.Write(fmt::format("fadd dword ptr ds:[{}]", universeUpdatePeriodAddressStr), Instructions::nisUpdateTask_UniverseUpdatePeriod1);
	assembler.Write(fmt::format("fcomp dword ptr ds:[{}]", universeUpdatePeriodAddressStr), Instructions::nisUpdateTask_UniverseUpdatePeriod2);
	assembler.Write(fmt::format("fmul dword ptr ds:[{}]", universeUpdatePeriodAddressStr), Instructions::nisUpdateTask_UniverseUpdatePeriod3);
	assembler.Write(fmt::format("fadd dword ptr ds:[{}]", universeUpdatePeriodAddressStr), Instructions::nisUpdateTask_UniverseUpdatePeriod4);
}

static void PatchUtyGameSystemsInit(Assembler& assembler, std::string& updatePeriodValueStr)
{
	// Register universe update task with new frequency
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::utyGameSystemsInit_UNIVERSE_UPDATE_PERIOD);

	// Update timer frequency
	std::string timerResolutionHex = IntToHex(s_TimerResolutionMax);
	assembler.Write(fmt::format("push {}", timerResolutionHex), Instructions::utyGameSystemsInit_TimerResolutionMax);
}

static void PatchNisStartup(Assembler& assembler, std::string& updatePeriodValueStr)
{
	// Register NIS update task with new frequency
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::nisStartup_UNIVERSE_UPDATE_PERIOD);
}

static void PatchNisUpdateTask(Assembler& assembler, std::string& updatePeriodAddressStr)
{
	// Update NIS cinematics at new frequency
	assembler.Write(fmt::format("mov dword ptr ds:[0x00A4CAB4], {}", updatePeriodAddressStr), Instructions::nisUpdateTask_UNIVERSE_UPDATE_PERIOD_1);
	assembler.Write(fmt::format("push {}", updatePeriodAddressStr), Instructions::nisUpdateTask_UNIVERSE_UPDATE_PERIOD_2);
}

static void PatchBabyTasksStartup(Assembler& assembler, std::string& updatePeriodHex)
{
	// Register "baby tasks" with new frequency
	assembler.Write(fmt::format("push {}", updatePeriodHex), Instructions::BabyTasksStartup_UNIVERSE_UPDATE_PERIOD);
}

static void PatchUnivUpdate(Assembler& assembler, std::string& updatePeriodValueStr)
{
	// Push modified UNIVERSE_UPDATE_PERIOD in calls to univUpdate
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::univUpdate1);
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::univUpdate2);
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::univUpdate3);
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::univUpdate4);
	assembler.Write(fmt::format("push {}", updatePeriodValueStr), Instructions::univUpdate5);
}

static void PatchUnivUpdateReset(Assembler& assembler, std::string& updatePeriodValueStr)
{
	// Set new update rate when resetting universe
	assembler.Write(fmt::format("mov dword ptr ds:[0x00B62ABC], {}", updatePeriodValueStr), Instructions::univUpdateReset_UNIVERSE_UPDATE_PERIOD);
}

static void PatchGunShipFirePower(Assembler& assembler, std::string& updatePeriodValueStr)
{
	// Set new update rate for gunship fire power
	assembler.Write(fmt::format("mov dword ptr ss:[esp+0x1C], {}", updatePeriodValueStr), Instructions::gunShipFirePower_UniverseUpdatePeriod1);
	assembler.Write(fmt::format("mov dword ptr ss:[esp+0x1C], {}", updatePeriodValueStr), Instructions::gunShipFirePower_UniverseUpdatePeriod2);
}

void ApplyUniversePatches(Assembler& assembler, Config& config)
{
	if (config.GetUniverseUpdateRateShift() == 0)
	{
		return;
	}

	CalculateTimestepConstants(config);

	float universeUpdatePeriod = config.GetUniverseUpdatePeriod();
	std::string updatePeriodValueStr = FloatToHex(universeUpdatePeriod);

	PatchUnivUpdate(assembler, updatePeriodValueStr);
	PatchUtyGameSystemsInit(assembler, updatePeriodValueStr);
	PatchNisStartup(assembler, updatePeriodValueStr);
	PatchNisUpdateTask(assembler, updatePeriodValueStr);
	PatchBabyTasksStartup(assembler, updatePeriodValueStr);
	PatchUnivUpdateReset(assembler, updatePeriodValueStr);
	PatchEtgDefineAt(assembler);
	PatchGunShipFirePower(assembler, updatePeriodValueStr);
	PatchNisUpdateTask(assembler);

	*Globals::TimerResolutionMax = (float)s_TimerResolutionMax;

	CreateAndEnableHooks(assembler);

	spdlog::info("Update rate patches applied");
}