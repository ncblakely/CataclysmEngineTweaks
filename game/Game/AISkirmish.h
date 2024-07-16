#pragma once

// Reverse engineered structures for Cataclysm's new skirmish AI

constexpr int NUM_AIS_TEAMS = 34;
constexpr int MAX_TEAM_SELECTIONS = 10;

struct AISTeamSelection
{
	MaxSelection sel;
	udword unk1;
	udword unk2;
	udword unk3;
};

struct AISTeamEntry
{
	AISTeamSelection selection[MAX_TEAM_SELECTIONS];
	udword numselections;
	udword totalteamsize;
};

namespace AISTeamType
{
	enum
	{
		// sRecon
		// bRecon
		Recon = 0,
		ReconEnemy = 1,

		// sAcolyte
		// sACV
		// bAcolyte
		// bInterceptor
		// bCloakedFighter
		// bAttackBomber
		// bACV
		Interceptor = 2,
		InterceptorEnemy = 3,

		// sMultiBeamFrigate
		// bMultiGunCorvette
		// bMultiBeamFrigate
		AntiFighter = 4,
		AntiFighterEnemy = 5,

		// sSentinel
		// bSentinel
		Sentinel = 6,
		SentinelEnemy = 7,

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
		MediumCombat = 8,
		MediumCombatEnemy = 9,

		Main = 10, // Guessing on the name for this one. Needs more investigation

        // sMothershipSpecial
		// sMothershipEngineering
		// sMothershipArmour
		// sMothershipMicro
		// sMothershipWeapons
		// sMothershipDockingBay
		// sMothershipLowerSpine
		// sMothershipBigGun
		MothershipModules = 11,

		// sCarrierSupport
		CarrierSupport1 = 12, // For carrier 1
		CarrierSupport2 = 13, // For carrier 2

		// sMothershipSupport
		// bMothershipSupport
		MothershipSupport = 14,
		MothershipSupportEnemy = 16,

		// sMothershipBigGun
		MothershipBigGun = 15,

		// sCarrier
		// bCarrier
		Carrier = 17,
		CarrierEnemy = 18,

		// sMothership
		// bMothership
		Mothership = 19,
		MothershipEnemy = 20,

		// sProcessor
		// bProcessor
		Processor = 21,
		ProcessorEnemy = 22,

		// sMimic
		// sMCV
		Mimic = 23,
		MimicEnemy1 = 24, // sub_53F9F0 determines which enemy group is assigned
		MimicEnemy2 = 25,

		// bCruiseMissile
		CruiseMissile = 26,

		// sLeech
		Leech = 27,
		LeechEnemy = 28,

		// bDFGFrigate
		DFGFrigate = 29,
		DFGFrigateEnemy = 30,

		// sWorker
		// bWorker
		Worker = 31,
		WorkerEnemy = 32,

		// dCrystal0
		// dCrystal1
		CrystalSalvage = 33,
	};
}

ASSERT_OFFSET(AISTeamEntry, numselections, 0x9D08);
ASSERT_OFFSET(AISTeamEntry, totalteamsize, 0x9D0C);
static_assert(sizeof(AISTeamEntry) == 0x9D10);

// Check for overflow between the first and last AI team entry in the executable
static_assert(sizeof(AISTeamEntry)* NUM_AIS_TEAMS == (0xA2D980 - 0x008DFD60), "AI team info overflow");

void DumpAITeams();
void aisFleetUpdate();