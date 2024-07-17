#pragma once

// Reverse engineered structures for Cataclysm's new skirmish AI

constexpr int NUM_AIS_TEAMS = 34;
constexpr int MAX_TEAM_SELECTIONS = 10;

struct AISTeamSelection
{
	MaxSelection sel;
	vector center;
};

struct AISTeamEntry
{
	AISTeamSelection selection[MAX_TEAM_SELECTIONS];
	udword numselections;
	sdword totalteamsize;
};

enum AITeamType
{
	// sRecon
	// bRecon
	TEAM_Recon = 0,
	TEAM_ReconEnemy = 1,

	// sAcolyte
	// sACV
	// bAcolyte
	// bInterceptor
	// bCloakedFighter
	// bAttackBomber
	// bACV
	TEAM_Fighter = 2,
	TEAM_FighterEnemy = 3,

	// sMultiBeamFrigate
	// bMultiGunCorvette
	// bMultiBeamFrigate
	TEAM_AntiFighter = 4,
	TEAM_AntiFighterEnemy = 5,

	// sSentinel
	// bSentinel
	TEAM_Sentinel = 6,
	TEAM_SentinelEnemy = 7,

	// sRammingFrigate
	// sHiveFrigate
	// sDreadnought
	// sDestroyer
	// bHeavyCorvette
	// bHiveFrigate
	// bIonArrayFrigate
	// bHeavyCruiser
	// bRammingFrigate
	// bCruiseMissile
	// bMissileCorvette
	TEAM_MediumCombat = 8,
	TEAM_MediumCombatEnemy = 9,

	TEAM_Main = 10, // Guessing on the name for this one. Needs more investigation

	// sMothershipSpecial
	// sMothershipEngineering
	// sMothershipArmour
	// sMothershipMicro
	// sMothershipWeapons
	// sMothershipDockingBay
	// sMothershipLowerSpine
	// sMothershipBigGun
	TEAM_MothershipModules = 11,

	// sCarrierSupport
	TEAM_CarrierSupport1 = 12, // For carrier 1
	TEAM_CarrierSupport2 = 13, // For carrier 2

	// sMothershipSupport
	// bMothershipSupport
	TEAM_MothershipSupport = 14,
	TEAM_MothershipSupportEnemy = 16,

	// sMothershipBigGun
	TEAM_MothershipBigGun = 15,

	// sCarrier
	// bCarrier
	TEAM_Carrier = 17,
	TEAM_CarrierEnemy = 18,

	// sMothership
	// bMothership
	TEAM_Mothership = 19,
	TEAM_MothershipEnemy = 20,

	// sProcessor
	// bProcessor
	TEAM_Processor = 21,
	TEAM_ProcessorEnemy = 22,

	// sMimic
	// sMCV
	TEAM_Mimic = 23,
	TEAM_MimicEnemy1 = 24, // sub_53F9F0 determines which enemy group is assigned
	TEAM_MimicEnemy2 = 25,

	// bCruiseMissile
	TEAM_CruiseMissile = 26,

	// sLeech
	TEAM_Leech = 27,
	TEAM_LeechEnemy = 28,

	// bDFGFrigate
	TEAM_DFGFrigate = 29,
	TEAM_DFGFrigateEnemy = 30,

	// sWorker
	// bWorker
	TEAM_Worker = 31,
	TEAM_WorkerEnemy = 32,

	// dCrystal0
	// dCrystal1
	TEAM_CrystalSalvage = 33,
};

ASSERT_OFFSET(AISTeamEntry, numselections, 0x9D08);
ASSERT_OFFSET(AISTeamEntry, totalteamsize, 0x9D0C);
static_assert(sizeof(AISTeamEntry) == 0x9D10);

// Check for overflow between the first and last AI team entry in the executable
static_assert(sizeof(AISTeamEntry)* NUM_AIS_TEAMS == (0xA2D980 - 0x008DFD60), "AI team info overflow");

void DumpAITeams();
void aisFleetUpdate();