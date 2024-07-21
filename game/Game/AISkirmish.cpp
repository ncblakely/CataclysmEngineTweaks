/*=============================================================================
  Tweakable version of Cataclysm's skirmish AI.

  This is intended to serve as a baseline or reference for future improvements
  to the AI.
=============================================================================*/

#include "pch.h"

#include "Cataclysm.h"
#include "AIPlayer.h"
#include "AISkirmish.h"

using namespace Functions;
using namespace Globals;

#ifdef _DEBUG
#define aiplayerLog(fmt, ...) \
	spdlog::info("AI {} ({} / D {}): " ## fmt, \
		Globals::aiCurrentAIPlayer->player->playerIndex, \
		Globals::aiCurrentAIPlayer->player->race ? "Beast" : "Sect", \
		(sdword)Globals::aiCurrentAIPlayer->aiplayerDifficultyLevel, \
		__VA_ARGS__); 
#else
#define aiplayerLog(x)    {;}
#endif

#define MAX_CARRIERS 2

#define CARRIER_MAX_SUPPORT_MODULES 6
#define MOTHERSHIP_MAX_SUPPORT_MODULES 12

static udword AIS_MIMIC_TARGET_COUNT[AI_NUM_LEVELS] = {0, 0, 20};
static udword AIS_SCOUT_TARGET_COUNT[AI_NUM_LEVELS] = {5, 8, 12};
static udword AIS_CRUISE_MISSILE_TARGET_COUNT[AI_NUM_LEVELS] = {0, 0, 10};
static udword AIS_FIGHTER_TARGET_COUNT[AI_NUM_LEVELS] = {25, 35, 60};
static udword AIS_DFG_TARGET_COUNT[AI_NUM_LEVELS] = {0, 1, 2};
static udword AIS_LEECH_TARGET_COUNT[AI_NUM_LEVELS] = {0, 10, 20};
static udword AIS_WORKER_NORMAL_TARGET_COUNT[AI_NUM_LEVELS] = {5, 8, 12};
static udword AIS_WORKER_HIGH_RESOURCE_TARGET_COUNT[AI_NUM_LEVELS] = {2, 4, 6};

static udword AIS_HIGH_RESOURCE_THRESHOLD = 50000;

static udword AIS_FIGHTER_BUILD_THRESHOLD = 15;

scriptEntry AISkirmishTweaks[] =
{
	// Ship target counts
	makeEntry(AIS_MIMIC_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_MIMIC_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_MIMIC_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_SCOUT_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_SCOUT_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_SCOUT_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_CRUISE_MISSILE_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_CRUISE_MISSILE_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_CRUISE_MISSILE_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_FIGHTER_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_FIGHTER_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_FIGHTER_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_DFG_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_DFG_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_DFG_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_LEECH_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_LEECH_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_LEECH_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_WORKER_NORMAL_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_WORKER_NORMAL_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_WORKER_NORMAL_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),
	makeEntry(AIS_WORKER_HIGH_RESOURCE_TARGET_COUNT[AI_BEG], Functions::scriptSetUdwordCB),
	makeEntry(AIS_WORKER_HIGH_RESOURCE_TARGET_COUNT[AI_INT], Functions::scriptSetUdwordCB),
	makeEntry(AIS_WORKER_HIGH_RESOURCE_TARGET_COUNT[AI_ADV], Functions::scriptSetUdwordCB),

	// Resource tweaks
	makeEntry(AIS_HIGH_RESOURCE_THRESHOLD, Functions::scriptSetUdwordCB),

	// Strategic tweaks
	makeEntry(AIS_FIGHTER_BUILD_THRESHOLD, Functions::scriptSetUdwordCB),

	endEntry
};

std::string DumpSelection(const AISTeamSelection& selection, int selectionIndex)
{
	std::string output = "\n";

	const MaxSelection& sel = selection.sel;

	output += fmt::format(
		"\t Selection {} | Num ships: {}\n"
		"\t Center position: {} {} {}\n",
		selectionIndex,
		sel.numShips,
		selection.center.x,
		selection.center.y,
		selection.center.z);

	output += "\n";
	for (int shipIndex = 0; shipIndex < sel.numShips; shipIndex++)
	{
		Ship* ship = sel.ShipPtr[shipIndex];

		output += fmt::format(
			"\t\t Ship {} ({}): Position {} {} {} \n\n",
			shipIndex,
			ShipTypeToStr(ship->shiptype),
			ship->posinfo.position.x,
			ship->posinfo.position.y, 
			ship->posinfo.position.z);
	}

	return output;
}

void DumpAITeams()
{
	for (int teamIndex = 0; teamIndex < NUM_AIS_TEAMS; teamIndex++)
	{
		const AISTeamEntry& teamEntry = aisTeams[teamIndex];

		int numSelections = teamEntry.numselections;

		std::string message = fmt::format(
			"AI team {} | Size: {} | Total team size: {}\n",
			teamIndex, 
			teamEntry.numselections, 
			teamEntry.totalteamsize);

		for (int selectionIndex = 0; selectionIndex < numSelections; selectionIndex++)
		{
			const AISTeamSelection& selection = teamEntry.selection[selectionIndex];

			const MaxSelection& sel = selection.sel;
			message += DumpSelection(selection, selectionIndex);

		}

		spdlog::info(message);
	}
}

static inline udword GetUnassignedShipCount(ShipType shipType)
{
	assert(shipType >= STD_FIRST_SHIP && shipType <= STD_LAST_SHIP);

	// Get the number of ships under construction
	udword shipCount = aiBuildingShip[shipType];

	for (udword i = 0; i < aisTeams[TEAM_Main].numselections; i++)
	{
		MaxSelection* sel = &aisTeams[TEAM_Main].selection[i].sel;
		for (sdword i = 0; i < sel->numShips; i++)
		{
			Ship* ship = sel->ShipPtr[i];
			if (ship->shiptype == sWorker)
			{
				shipCount++;
			}
		}
	}

	aiplayerLog("Unassigned ship count for {}: {}, queued was {}", ShipTypeToStr(shipType), shipCount, aiBuildingShip[shipType]);

	return shipCount;
}

static void aisBuildShip(Player* player, ShipType shipType, int buildCost)
{
	ShipStaticInfo* shipstatic = GetShipStaticInfo(shipType);
	sdword resourceUnits = player->resourceUnits;
	udword supportUnitsRequiredForShip = shipstatic->supportUnits;
	sdword supportUnitsUsed = *aiSupportUnitsPending + player->supportUnitsUsed + supportUnitsRequiredForShip;
	sdword resourceUnitsRemaining = resourceUnits;
	if (supportUnitsUsed < player->supportUnitsMax && resourceUnits > buildCost)
	{
		if (GetShipStaticInfo(shipType)->externalBuild)
		{
			aiplayerLog("Building external ship {}", ShipTypeToStr(shipType));

			if (aisTeams[TEAM_Mothership].totalteamsize > 0)
			{
				if (rmCanBuildShip(player, shipType, 1))
					clWrapCreateShip(
						&universe->mainCommandLayer,
						shipType,
						player->race,
						player->playerIndex,
						aisTeams[TEAM_Mothership].selection[0].sel.ShipPtr[0]);
				return;
			}
		}
		else if (aisTeams[TEAM_Mothership].totalteamsize > 0 && rmCanBuildShip(player, shipType, 1))
		{
			aiplayerLog("Building internal ship {}", ShipTypeToStr(shipType));

			clWrapBuildShip(
				&universe->mainCommandLayer,
				shipType,
				player->race,
				player->playerIndex,
				aisTeams[TEAM_Mothership].selection[0].sel.ShipPtr[0]);
			supportUnitsUsed += supportUnitsRequiredForShip;
			resourceUnitsRemaining = resourceUnits - buildCost;
		}
		if (rmCanBuildShip(player, shipType, 0))
		{
			sdword carrierIndex = 0;
			Ship* carrier = nullptr;
			if (supportUnitsUsed < player->supportUnitsMax)
			{
				for (sdword carrierIndex = 0; carrierIndex < aisTeams[TEAM_Carrier].totalteamsize; carrierIndex++)
				{
					int carriersConsidered = 0;
					for (udword selectionIndex = 0; selectionIndex < aisTeams[TEAM_Carrier].numselections; selectionIndex++)
					{
						MaxSelection* sel = &aisTeams[TEAM_Carrier].selection[selectionIndex].sel;
						for (int shipIndex = 0; shipIndex < sel->numShips; shipIndex++)
						{
							if (carrierIndex == carriersConsidered)
							{
								carrier = sel->ShipPtr[shipIndex];
								break;
							}

							carriersConsidered++;
						}
					}

					if (!carrier || resourceUnitsRemaining <= buildCost)
						break;

					aiplayerLog("Also building ship {} on carrier {}", ShipTypeToStr(shipType), carrierIndex + 1);
					clWrapBuildShip(
						&universe->mainCommandLayer,
						shipType,
						player->race,
						player->playerIndex,
						carrier);

					supportUnitsUsed += supportUnitsRequiredForShip;

					if (supportUnitsUsed >= player->supportUnitsMax)
						break;
				}
			}
		}
	}
}

static void BuildShip(Player* player, ShipType shipType, sdword buildCost)
{
	aiplayerLog("Requesting ship {} with build cost {}", ShipTypeToStr(shipType), buildCost);

	aisBuildShip(player, shipType, buildCost);
}

void aisFleetUpdate()
{
	Player* player = aiCurrentAIPlayer->player;
	Ship* playerMothership = player->PlayerMothership;

	// DumpAITeams();

	if (playerMothership && playerMothership->staticinfo->shipclass == CLASS_Mothership)
	{
		if (player->race == RACE_Beast)
		{
			udword supportModuleCount = aisTeams[TEAM_MothershipSupport].totalteamsize;
			if (aisTeams[TEAM_Mothership].totalteamsize > 0 && supportModuleCount < MOTHERSHIP_MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
			{
				Ship* leader = aisTeams[TEAM_Mothership].selection[0].sel.ShipPtr[0];

				if (rmCanBuildShip(player, bMothershipSupport, 1))
				{
					aiplayerLog("Building support modules, current count {}", supportModuleCount);

					clWrapCreateShip(
						&universe->mainCommandLayer,
						bMothershipSupport,
						player->race,
						player->playerIndex,
						leader);
				}
			}
		}
		else
		{
			if (player->resourceUnits > 0)
			{
				// Build sect command ship modules
				if (!selNumShipsInSelection(&aisTeams[TEAM_Main].selection[0].sel, sMothershipDockingBay)
					&& !selNumShipsInSelection(&aisTeams[TEAM_MothershipModules].selection[0].sel, sMothershipDockingBay))
				{
					BuildShip(player, sMothershipDockingBay, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[TEAM_Main].selection[0].sel, sMothershipMicro)
					&& !selNumShipsInSelection(&aisTeams[TEAM_MothershipModules].selection[0].sel, sMothershipMicro))
				{
					BuildShip(player, sMothershipMicro, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[TEAM_Main].selection[0].sel, sMothershipWeapons)
					&& !selNumShipsInSelection(&aisTeams[TEAM_MothershipModules].selection[0].sel, sMothershipWeapons))
				{
					BuildShip(player, sMothershipWeapons, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[TEAM_Main].selection[0].sel, sMothershipSpecial)
					&& !selNumShipsInSelection(&aisTeams[TEAM_MothershipModules].selection[0].sel, sMothershipSpecial))
				{
					BuildShip(player, sMothershipSpecial, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[TEAM_Main].selection[0].sel, sMothershipArmour)
					&& !selNumShipsInSelection(&aisTeams[TEAM_MothershipModules].selection[0].sel, sMothershipArmour))
				{
					BuildShip(player, sMothershipArmour, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[TEAM_Main].selection[0].sel, sMothershipBigGun)
					&& !selNumShipsInSelection(&aisTeams[TEAM_MothershipModules].selection[0].sel, sMothershipBigGun))
				{
					BuildShip(player, sMothershipBigGun, 5000);
				}

				auto supportModuleCount = aisTeams[TEAM_MothershipSupport].totalteamsize;
				if (aisTeams[TEAM_Mothership].totalteamsize > 0 && supportModuleCount < MOTHERSHIP_MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
				{
					Ship* leader = aisTeams[TEAM_Mothership].selection[0].sel.ShipPtr[0];
					if (rmCanBuildShip(player, sMothershipSupport, 1))
					{
						clWrapCreateShip(
							&universe->mainCommandLayer,
							sMothershipSupport,
							player->race,
							player->playerIndex,
							leader);
					}
				}
			}
		}

		bool hasExternalConstruction = *aiHasExternalConstruction;
		if (!hasExternalConstruction)
		{
			Node* todoNode = universe->mainCommandLayer.todolist.head;
			while (todoNode)
			{
				CommandToDo* todoCommand = (CommandToDo*)todoNode->structptr;
				if (todoCommand->ordertype.order == COMMAND_LAUNCHSHIP) // Name of this enum is probably wrong, seems to be external construction
				{
					int numShips = todoCommand->selection->numShips;
					if (numShips > 0)
					{
						Ship* ship = todoCommand->selection->ShipPtr[0];
						if (ship->playerowner == player)
						{
							ShipStaticInfo* shipstatic = ship->staticinfo;
							if (shipstatic->externalBuild)
							{
								if (shipstatic->shipclass != CLASS_Component)
								{
									aiplayerLog("I now have external construction queued for {}", ShipTypeToStr(shipstatic->shiptype));

									hasExternalConstruction = true;
									*aiHasExternalConstruction = true;
								}
							}
						}
					}
				}
				todoNode = todoNode->next;
			}
		}

		if (!hasExternalConstruction)
		{
			ShipType shipToBuild = 0;
			sdword buildCost = 0;
			if (player->race == RACE_Beast)
			{
				if (rmCanBuildShip(player, bCarrier, RACE_Beast) && (int)aisTeams[TEAM_Carrier].totalteamsize < MAX_CARRIERS)
				{
					shipToBuild = bCarrier;
					buildCost = 2000;
				}
				else if (rmCanBuildShip(player, bHeavyCruiser, 1) && player->resourceUnits > 2000)
				{
					shipToBuild = bHeavyCruiser;
					buildCost = 3000;
				}
				else if (rmCanBuildShip(player, bProcessor, 1)
					&& player->resourceUnits > 1000
					&& !aisTeams[TEAM_Processor].totalteamsize)
				{
					shipToBuild = bProcessor;
					buildCost = 1000;
				}
			}
			else
			{
				if (rmCanBuildShip(player, sCarrier, 1) && (int)aisTeams[TEAM_Carrier].totalteamsize < MAX_CARRIERS)
				{
					shipToBuild = sCarrier;
					buildCost = 2000;
					BuildShip(player, shipToBuild, buildCost);
				}
				else if (rmCanBuildShip(player, sDreadnought, 1) && player->resourceUnits > 2000)
				{
					shipToBuild = sDreadnought;
					buildCost = 3000;
				}
				else if (rmCanBuildShip(player, sDestroyer, 1) && player->resourceUnits > 1000)
				{
					shipToBuild = sDestroyer;
					buildCost = 1500;
				}
				else if (rmCanBuildShip(player, sProcessor, 1) && player->resourceUnits > 1000 && !aisTeams[TEAM_Processor].totalteamsize)
				{
					shipToBuild = sProcessor;
					buildCost = 1000;
				}
			}

			BuildShip(player, shipToBuild, buildCost);
		}
	}

	Ship* carrier1 = aiCarrier1Ship;
	if (carrier1
		&& carrier1->playerowner->race == RACE_Sect
		&& (int)aisTeams[TEAM_CarrierSupport1].totalteamsize < CARRIER_MAX_SUPPORT_MODULES
		&& !*aiHasCarrier1ModuleQueued
		&& rmCanBuildShip(player, sCarrierSupport, 1))
	{
		clWrapCreateShip(&universe->mainCommandLayer, sCarrierSupport, player->race, player->playerIndex, carrier1);
	}

	Ship* carrier2 = aiCarrier2Ship;
	if (carrier2
		&& carrier2->playerowner->race == RACE_Sect
		&& (int)aisTeams[TEAM_CarrierSupport2].totalteamsize < CARRIER_MAX_SUPPORT_MODULES
		&& !*aiHasCarrier2ModuleQueued
		&& rmCanBuildShip(player, sCarrierSupport, 1))
	{
		clWrapCreateShip(&universe->mainCommandLayer, sCarrierSupport, player->race, player->playerIndex, carrier2);
	}

	udword workerCount = aisTeams[TEAM_Worker].totalteamsize;

	workerCount += player->race == RACE_Beast 
		? GetUnassignedShipCount(bWorker)
		: GetUnassignedShipCount(sWorker);

	sdword resourceUnits = aiCurrentAIPlayer->player->resourceUnits;
	udword workerTargetCount = resourceUnits > AIS_HIGH_RESOURCE_THRESHOLD ?
		AIS_WORKER_HIGH_RESOURCE_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel] :
		AIS_WORKER_NORMAL_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];

	ShipType workerType = player->race == RACE_Sect ? sWorker : bWorker;
	if (workerCount <= workerTargetCount)
	{
		BuildShip(player, workerType, 0);
	}

	udword scoutTargetCount = AIS_SCOUT_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];

	udword enemyScoutCount = aisTeams[TEAM_ScoutEnemy].totalteamsize;
	if (enemyScoutCount < scoutTargetCount)
	{
		scoutTargetCount = enemyScoutCount;
	}

	if ((udword)aisTeams[TEAM_Scout].totalteamsize < scoutTargetCount)
	{
		udword scoutCount =
			aisTeams[TEAM_Scout].totalteamsize;

		scoutCount += player->race == RACE_Beast
			? GetUnassignedShipCount(bRecon)
			: GetUnassignedShipCount(sRecon);

		ShipType reconType = player->race == RACE_Sect ? sRecon : bRecon;
		if (scoutTargetCount > scoutCount)
		{
			BuildShip(player, reconType, 50);
		}
	}

	if (player->race == RACE_Beast)
	{
		udword dfgFrigateTargetCount = AIS_DFG_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];

		udword dfgFrigateCount = 
			aisTeams[TEAM_DFGFrigate].totalteamsize +
			GetUnassignedShipCount(bDFGFrigate);

		if (dfgFrigateCount < dfgFrigateTargetCount)
		{
			BuildShip(player, bDFGFrigate, 2000);
		}
	}
	
	if (player->race == RACE_Sect)
	{
		udword leechTargetCount = AIS_LEECH_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];

		udword leechCount = 
			aisTeams[TEAM_Leech].totalteamsize +
			GetUnassignedShipCount(sLeech);

		if (leechCount < leechTargetCount)
		{
			BuildShip(player, sLeech, 100);
		}
	}

	if (player->race == RACE_Sect)
	{
		udword mimicTargetCount = AIS_MIMIC_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];

		// MCV count is not checked here. Original game bug?
		udword mimicCount = 
			aisTeams[TEAM_Mimic].totalteamsize +
			GetUnassignedShipCount(sMimic);

		if (mimicCount < mimicTargetCount)
		{
			BuildShip(player, sMimic, 150);
			BuildShip(player, sMCV, 300);
		}
	}

	if (player->race == RACE_Beast)
	{
		udword cruiseMissileTargetCount = AIS_CRUISE_MISSILE_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];

		udword cruiseMissileCount = 
			aisTeams[TEAM_CruiseMissile].totalteamsize +
			GetUnassignedShipCount(bCruiseMissile);

		if (cruiseMissileCount < cruiseMissileTargetCount)
		{
			BuildShip(player, bCruiseMissile, 100);
		}
	}

	udword enemyFighterCount = aisTeams[TEAM_FighterEnemy].totalteamsize;
	if (enemyFighterCount > AIS_FIGHTER_BUILD_THRESHOLD)
	{
		aiplayerLog("Want to build more fighters, enemy has {}", enemyFighterCount);

		if (player->race == RACE_Beast)
		{
			BuildShip(player, bMultiBeamFrigate, 800);
			BuildShip(player, bMultiGunCorvette, 400);
		}
		else
		{
			BuildShip(player, sMultiBeamFrigate, 800);
		}

build_fighters:
		udword fighterTargetCount = AIS_FIGHTER_TARGET_COUNT[aiCurrentAIPlayer->aiplayerDifficultyLevel];
		if ((udword)aisTeams[TEAM_Fighter].totalteamsize >= fighterTargetCount)
		{
			aiplayerLog("Already have enough fighters, have {}", aisTeams[TEAM_Fighter].totalteamsize);
			return;
		}

		udword fighterCount = aisTeams[TEAM_Fighter].totalteamsize;
		if (player->race == RACE_Beast)
		{
			fighterCount += GetUnassignedShipCount(bAcolyte);
			fighterCount += GetUnassignedShipCount(bInterceptor);
			fighterCount += GetUnassignedShipCount(bCloakedFighter);
			fighterCount += GetUnassignedShipCount(bAttackBomber);
			fighterCount += GetUnassignedShipCount(bACV);
		}
		else
		{
			fighterCount += GetUnassignedShipCount(sAcolyte);
			fighterCount += GetUnassignedShipCount(sACV);
		}

		if (fighterCount < fighterTargetCount)
		{
			if (player->race == RACE_Beast)
			{
				BuildShip(player, bInterceptor, 100);

				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_BEG)
				{
					BuildShip(player, bAttackBomber, 150);
					BuildShip(player, bCloakedFighter, 150);
				}

				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_INT)
				{
					BuildShip(player, bAcolyte, 150);
					BuildShip(player, bACV, 300);
				}
			}
			else // RACE_Sect
			{
				BuildShip(player, sAcolyte, 150);
				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_BEG)
				{
					BuildShip(player, sACV, 300);
				}
			}
		}

		return;
	}

	bool32 couldNotBuildMedium = false;
	ShipType mediumShipToBuild = 0;
	if (player->race == RACE_Beast)
	{
		if (!rmCanBuildShip(player, bHeavyCruiser, 1) || *aiHasExternalConstruction)
		{
			switch (ranRandomFn(6) % 7)
			{
				case 0:
					mediumShipToBuild = bHeavyCorvette;
					break;
				case 1:
					mediumShipToBuild = bHiveFrigate;
					break;
				case 2:
				case 3:
					mediumShipToBuild = bIonArrayFrigate;
					break;
				case 4:
					mediumShipToBuild = bRammingFrigate;
					break;
				case 5:
					mediumShipToBuild = bCruiseMissile;
					break;
				case 6:
					mediumShipToBuild = bMissileCorvette;
					break;
				default:
					return; // TODO: Correct to return here?
			}

			aiplayerLog("Building medium ship {}", ShipTypeToStr(mediumShipToBuild));

			assert(mediumShipToBuild > STD_FIRST_SHIP);

			ShipStaticInfo* shipstatic = GetShipStaticInfo(mediumShipToBuild);
			BuildShip(player, mediumShipToBuild, shipstatic->buildCost);
		}
		else
		{
			couldNotBuildMedium = true;
		}
	}
	else
	{
		if (!rmCanBuildShip(player, sDreadnought, 1) && !rmCanBuildShip(player, sDestroyer, 1) || *aiHasExternalConstruction)
		{
			switch (ranRandomFn(6) % 6)
			{
				case 0:
					mediumShipToBuild = sRammingFrigate;
					break;
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
					mediumShipToBuild = sHiveFrigate;
					break;
				default:
					return;
			}

			aiplayerLog("Building medium ship {}", ShipTypeToStr(mediumShipToBuild));

			assert(mediumShipToBuild >= 0);
			ShipStaticInfo* shipstatic = GetShipStaticInfo(mediumShipToBuild);
			BuildShip(player, mediumShipToBuild, shipstatic->buildCost);
		}
		else
		{
			couldNotBuildMedium = true;
		}
	}

	if ((player->supportUnitsUsed + *aiSupportUnitsPending + 50) <= player->supportUnitsMax
		&& !couldNotBuildMedium)
	{
		aiplayerLog("Building more fighters. I have {} support units used, {} pending, and {} max", 
			player->supportUnitsUsed, 
			*aiSupportUnitsPending, 
			player->supportUnitsMax);

		// TODO: Should be possible to eliminate this goto
		goto build_fighters;
	}

	if (aisTeams[TEAM_Fighter].numselections > 0)
	{
		if (aiCurrentAIPlayer->aiplayerDifficultyLevel >= AI_ADV)
		{
			aiplayerLog("Setting fighter group 0 to kamikaze");

			clWrapSetKamikaze(&universe->mainCommandLayer, (SelectCommand*)&aisTeams[TEAM_Fighter].selection->sel);
		}
		else
		{
			aiplayerLog("Scuttling fighter groups 0, 1");
			clWrapScuttle(&universe->mainCommandLayer, (SelectCommand*)&aisTeams[TEAM_Fighter].selection->sel);
			clWrapScuttle(&universe->mainCommandLayer, (SelectCommand*)&aisTeams[TEAM_Fighter].selection->sel);
		}
	}
}