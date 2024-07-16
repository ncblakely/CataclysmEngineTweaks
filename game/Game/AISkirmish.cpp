/*=============================================================================
  Reverse-engineered reimplementation of Cataclysm's skirmish AI.

  This is intended to serve as a baseline or reference for future improvements
  to the AI.
=============================================================================*/

#include "pch.h"

#include "Cataclysm.h"
#include "AIPlayer.h"
#include "AISkirmish.h"

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

#define HIGH_RESOURCE_THRESHOLD 50000

#define MAX_CARRIERS 2

#define CARRIER_MAX_SUPPORT_MODULES 6
#define MOTHERSHIP_MAX_SUPPORT_MODULES 12

using namespace Functions;
using namespace Globals;

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

	for (udword i = 0; i < aisTeams[AISTeamType::Main].numselections; i++)
	{
		MaxSelection* sel = &aisTeams[AISTeamType::Main].selection[i].sel;
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

static void RequestShip(Player* player, ShipType shipType, sdword buildCost)
{
	aiplayerLog("Requesting ship {} with build cost {}", ShipTypeToStr(shipType), buildCost);

	aisRequestShip(player, shipType, buildCost);
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
			udword supportModuleCount = aisTeams[AISTeamType::MothershipSupport].totalteamsize;
			if (aisTeams[AISTeamType::Mothership].totalteamsize > 0 && supportModuleCount < MOTHERSHIP_MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
			{
				Ship* leader = aisTeams[AISTeamType::Mothership].selection[0].sel.ShipPtr[0];

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
				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipDockingBay)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipDockingBay))
				{
					RequestShip(player, sMothershipDockingBay, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipMicro)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipMicro))
				{
					RequestShip(player, sMothershipMicro, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipWeapons)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipWeapons))
				{
					RequestShip(player, sMothershipWeapons, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipSpecial)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipSpecial))
				{
					RequestShip(player, sMothershipSpecial, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipArmour)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipArmour))
				{
					RequestShip(player, sMothershipArmour, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipBigGun)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipBigGun))
				{
					RequestShip(player, sMothershipBigGun, 5000);
				}

				auto supportModuleCount = aisTeams[AISTeamType::MothershipSupport].totalteamsize;
				if (aisTeams[AISTeamType::Mothership].totalteamsize > 0 && supportModuleCount < MOTHERSHIP_MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
				{
					Ship* leader = aisTeams[AISTeamType::Mothership].selection[0].sel.ShipPtr[0];
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
				if (rmCanBuildShip(player, bCarrier, RACE_Beast) && (int)aisTeams[AISTeamType::Carrier].totalteamsize < MAX_CARRIERS)
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
					&& !aisTeams[AISTeamType::Processor].totalteamsize)
				{
					shipToBuild = bProcessor;
					buildCost = 1000;
				}
			}
			else
			{
				if (rmCanBuildShip(player, sCarrier, 1) && (int)aisTeams[AISTeamType::Carrier].totalteamsize < MAX_CARRIERS)
				{
					shipToBuild = sCarrier;
					buildCost = 2000;
					RequestShip(player, shipToBuild, buildCost);
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
				else if (rmCanBuildShip(player, sProcessor, 1) && player->resourceUnits > 1000 && !aisTeams[AISTeamType::Processor].totalteamsize)
				{
					shipToBuild = sProcessor;
					buildCost = 1000;
				}
			}

			RequestShip(player, shipToBuild, buildCost);
		}
	}

	Ship* carrier1 = aiCarrier1Ship;
	if (carrier1
		&& carrier1->playerowner->race == RACE_Sect
		&& (int)aisTeams[AISTeamType::CarrierSupport1].totalteamsize < CARRIER_MAX_SUPPORT_MODULES
		&& !*aiHasCarrier1ModuleQueued
		&& rmCanBuildShip(player, sCarrierSupport, 1))
	{
		clWrapCreateShip(&universe->mainCommandLayer, sCarrierSupport, player->race, player->playerIndex, carrier1);
	}

	Ship* carrier2 = aiCarrier2Ship;
	if (carrier2
		&& carrier2->playerowner->race == RACE_Sect
		&& (int)aisTeams[AISTeamType::CarrierSupport2].totalteamsize < CARRIER_MAX_SUPPORT_MODULES
		&& !*aiHasCarrier2ModuleQueued
		&& rmCanBuildShip(player, sCarrierSupport, 1))
	{
		clWrapCreateShip(&universe->mainCommandLayer, sCarrierSupport, player->race, player->playerIndex, carrier2);
	}

	udword workerCount = aisTeams[AISTeamType::Worker].totalteamsize;

	workerCount += player->race == RACE_Beast 
		? GetUnassignedShipCount(bWorker)
		: GetUnassignedShipCount(sWorker);

	udword workerTargetCount;
	sdword resourceUnits = aiCurrentAIPlayer->player->resourceUnits;
	switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
	{
		case AI_BEG:
			workerTargetCount = resourceUnits > HIGH_RESOURCE_THRESHOLD ? 2 : 5;
			break;
		case AI_INT:
			workerTargetCount = resourceUnits > HIGH_RESOURCE_THRESHOLD ? 4 : 8;
			break;
		default: // AI_ADV
			workerTargetCount = resourceUnits > HIGH_RESOURCE_THRESHOLD ? 6 : 12;
			break;
	}

	ShipType workerType = player->race == RACE_Sect ? sWorker : bWorker;
	if (workerCount <= workerTargetCount)
	{
		RequestShip(player, workerType, 0);
	}

	udword reconTargetCount;
	switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
	{
		case AI_BEG:
			reconTargetCount = 5;
			break;
		case AI_INT:
			reconTargetCount = 8;
			break;
		default: // AI_ADV
			reconTargetCount = 12;
			break;
	}

	udword enemyReconCount = aisTeams[AISTeamType::ReconEnemy].totalteamsize;
	if (enemyReconCount < reconTargetCount)
	{
		reconTargetCount = enemyReconCount;
	}

	if (aisTeams[AISTeamType::Recon].totalteamsize < reconTargetCount)
	{
		udword reconCount =
			aisTeams[AISTeamType::Recon].totalteamsize;

		reconCount += player->race == RACE_Beast
			? GetUnassignedShipCount(bRecon)
			: GetUnassignedShipCount(sRecon);

		ShipType reconType = player->race == RACE_Sect ? sRecon : bRecon;
		if (reconTargetCount > reconCount)
		{
			RequestShip(player, reconType, 50);
		}
	}

	if (player->race == RACE_Beast)
	{
		udword dfgFrigateTargetCount;
		switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
		{
			case AI_BEG:
				dfgFrigateTargetCount = 0;
				break;
			case AI_INT:
				dfgFrigateTargetCount = 1;
				break;
			default: // AI_ADV
				dfgFrigateTargetCount = 2;
				break;
		}

		udword dfgFrigateCount = 
			aisTeams[AISTeamType::DFGFrigate].totalteamsize +
			GetUnassignedShipCount(bDFGFrigate);

		if (dfgFrigateCount < dfgFrigateTargetCount)
		{
			RequestShip(player, bDFGFrigate, 2000);
		}
	}
	
	if (player->race == RACE_Sect)
	{
		udword leechTargetCount;
		switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
		{
			case AI_BEG:
				leechTargetCount = 0;
				break;
			case AI_INT:
				leechTargetCount = 10;
				break;
			default: // AI_ADV
				leechTargetCount = 20;
				break;
		}

		udword leechCount = 
			aisTeams[AISTeamType::Leech].totalteamsize +
			GetUnassignedShipCount(sLeech);

		if (leechCount < leechTargetCount)
		{
			RequestShip(player, sLeech, 100);
		}
	}

	if (player->race == RACE_Sect)
	{
		udword mimicTargetCount;
		switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
		{
			case AI_BEG:
				mimicTargetCount = 0;
				break;
			case AI_INT:
				mimicTargetCount = 0;
				break;
			default: // AI_ADV
				mimicTargetCount = 20;
				break;
		}

		// MCV count is not checked here. Original game bug?
		udword mimicCount = 
			aisTeams[AISTeamType::Mimic].totalteamsize +
			GetUnassignedShipCount(sMimic);

		if (mimicCount < mimicTargetCount)
		{
			RequestShip(player, sMimic, 150);
			RequestShip(player, sMCV, 300);
		}
	}

	if (player->race == RACE_Beast)
	{
		udword cruiseMissileTargetCount;
		switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
		{
			case AI_BEG:
				cruiseMissileTargetCount = 0;
				break;
			case AI_INT:
				cruiseMissileTargetCount = 0;
				break;
			default: // AI_ADV
				cruiseMissileTargetCount = 10;
				break;
		}

		udword cruiseMissileCount = 
			aisTeams[AISTeamType::CruiseMissile].totalteamsize +
			GetUnassignedShipCount(bCruiseMissile);

		if (cruiseMissileCount < cruiseMissileTargetCount)
		{
			RequestShip(player, bCruiseMissile, 100);
		}
	}

	udword enemyInterceptorCount = aisTeams[AISTeamType::InterceptorEnemy].totalteamsize;
	if (enemyInterceptorCount > 15)
	{
		aiplayerLog("Want to build more interceptors, enemy has {}", enemyInterceptorCount);

		if (player->race == RACE_Beast)
		{
			RequestShip(player, bMultiBeamFrigate, 800);
			RequestShip(player, bMultiGunCorvette, 400);
		}
		else
		{
			RequestShip(player, sMultiBeamFrigate, 800);
		}

LABEL_230:

		udword interceptorTargetCount;
		switch (aiCurrentAIPlayer->aiplayerDifficultyLevel)
		{
			case AI_BEG:
				interceptorTargetCount = 25;
				break;
			case AI_INT:
				interceptorTargetCount = 35;
				break;
			default: // AI_ADV
				interceptorTargetCount = 60;
				break;
		}

		// Why does this return?
		if (aisTeams[AISTeamType::Interceptor].totalteamsize >= interceptorTargetCount)
		{
			aiplayerLog("Already have enough interceptors, have {}", aisTeams[AISTeamType::Interceptor].totalteamsize);
			return;
		}

		udword interceptorCount = aisTeams[AISTeamType::Interceptor].totalteamsize;
		if (player->race == RACE_Beast)
		{
			interceptorCount += GetUnassignedShipCount(bAcolyte);
			interceptorCount += GetUnassignedShipCount(bInterceptor);
			interceptorCount += GetUnassignedShipCount(bCloakedFighter);
			interceptorCount += GetUnassignedShipCount(bAttackBomber);
			interceptorCount += GetUnassignedShipCount(bACV);
		}
		else
		{
			interceptorCount += GetUnassignedShipCount(sAcolyte);
			interceptorCount += GetUnassignedShipCount(sACV);
		}

		if (interceptorCount < interceptorTargetCount)
		{
			if (player->race == RACE_Beast)
			{
				RequestShip(player, bInterceptor, 100);

				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_BEG)
				{
					RequestShip(player, bAttackBomber, 150);
					RequestShip(player, bCloakedFighter, 150);
				}

				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_INT)
				{
					RequestShip(player, bAcolyte, 150);
					RequestShip(player, bACV, 300);
				}
			}
			else // RACE_Sect
			{
				RequestShip(player, sAcolyte, 150);
				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_BEG)
				{
					RequestShip(player, sACV, 300);
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
					return; // FIXME
			}

			aiplayerLog("Building a medium ship");

			assert(mediumShipToBuild >= 0);
			ShipStaticInfo* shipstatic = GetShipStaticInfo(mediumShipToBuild);
			RequestShip(player, mediumShipToBuild, shipstatic->buildCost);
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
					return; // FIXME
			}

			aiplayerLog("Building a medium ship");

			assert(mediumShipToBuild >= 0);
			ShipStaticInfo* shipstatic = GetShipStaticInfo(mediumShipToBuild);
			RequestShip(player, mediumShipToBuild, shipstatic->buildCost);
		}
		else
		{
			couldNotBuildMedium = true;
		}
	}

	if ((player->supportUnitsUsed + *aiSupportUnitsPending + 50) <= player->supportUnitsMax
		&& !couldNotBuildMedium)
	{
		aiplayerLog("Building more interceptors. I have {} support units used, {} pending, and {} max", 
			player->supportUnitsUsed, 
			*aiSupportUnitsPending, 
			player->supportUnitsMax);

		// TODO: Should be possible to eliminate this goto
		goto LABEL_230;
	}

	if (aisTeams[AISTeamType::Interceptor].numselections > 0)
	{
		if (aiCurrentAIPlayer->aiplayerDifficultyLevel >= AI_ADV)
		{
			aiplayerLog("Setting interceptor group 0 to kamikaze");

			clWrapSetKamikaze(&universe->mainCommandLayer, (SelectCommand*)&aisTeams[AISTeamType::Interceptor].selection->sel);
		}
		else
		{
			aiplayerLog("Scuttling interceptor groups 0, 1");
			clWrapScuttle(&universe->mainCommandLayer, (SelectCommand*)&aisTeams[AISTeamType::Interceptor].selection->sel);
			clWrapScuttle(&universe->mainCommandLayer, (SelectCommand*)&aisTeams[AISTeamType::Interceptor].selection->sel);
		}
	}
}