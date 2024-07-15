#pragma once

// Reverse engineered structures for Cataclysm's new skirmish AI

#define NUM_AIS_TEAMS 34

struct AISTeam
{
	MaxSelection sel[10];
	ubyte unk[0x78];
	udword teamsize; // Seems team size related, but differs from totalteamsize.
};

struct AISTeamEntry
{
	AISTeam team;
	udword totalteamsize;
};

namespace AISTeamType
{
	enum
	{
		Recon = 0,
		ReconEnemy = 1,
		Interceptor = 2,
		InterceptorEnemy = 3,
		Sentinel = 6,
		Main = 10,
		MothershipModules = 11,
		CarrierSupport1 = 12,
		CarrierSupport2 = 13,
		MothershipSupport = 14,
		Carrier = 17,
		Mothership = 19,
		MothershipEnemy = 20,
		Processor = 21,
		Mimic = 23,
		CruiseMissile = 26,
		Leech = 27,
		LeechEnemy = 28,
		DFGFrigate = 29,
		DFGFrigateEnemy = 30,
		Worker = 31,
		CrystalSalvage = 33,
	};
}

static_assert(offsetof(AISTeamEntry, team.teamsize) == 0x9D08);
static_assert(offsetof(AISTeamEntry, totalteamsize) == 0x9D0C);
static_assert(sizeof(AISTeamEntry) == 0x9D10);

void aisFleetUpdate();