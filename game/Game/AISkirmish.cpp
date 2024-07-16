#include "pch.h"

#include "Cataclysm.h"
#include "AIPlayer.h"
#include "AISkirmish.h"

#define HIGH_RESOURCE_THRESHOLD 50000

#define MAX_CARRIERS 2

#define CARRIER_MAX_SUPPORT_MODULES 6
#define MOTHERSHIP_MAX_SUPPORT_MODULES 12

std::string DumpSelection(const MaxSelection& sel, int selectionIndex)
{
	std::string output = "\n";

	output += fmt::format(
		"\t Selection {} | Num ships: {}\n",
		selectionIndex,
		sel.numShips);

	output += "\n";
	for (int shipIndex = 0; shipIndex < sel.numShips; shipIndex++)
	{
		Ship* ship = sel.ShipPtr[shipIndex];

		output += fmt::format(
			"\t\t Ship {} | Type {} | Class {}\n\n",
			shipIndex,
			ship->shiptype,
			ship->staticinfo->shipclass);
	}

	return output;
}

void DumpAITeams()
{
	using namespace Globals;

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
			message += DumpSelection(sel, selectionIndex);

		}

		spdlog::info(message);
	}
}

inline udword GetUnassignedShipCount(ShipType shipType, udword* queuedCount)
{
	using namespace Globals;

	udword shipCount = *queuedCount;

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

	return shipCount;
}

void aisFleetUpdate()
{
	using namespace Functions;
	using namespace Globals;

	Player* player = aiCurrentAIPlayer->player;
	Ship* playerMothership = player->PlayerMothership;

	// DumpAITeams();

	if (playerMothership && playerMothership->staticinfo->shipclass == CLASS_Mothership)
	{
		if (player->race == RACE_Beast)
		{
			udword supportModuleCount = aisTeams[AISTeamType::MothershipSupport].numselections;
			if (aisTeams[AISTeamType::Mothership].numselections > 0 && supportModuleCount < MOTHERSHIP_MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
			{
				Ship* leader = aisTeams[AISTeamType::Mothership].selection[0].sel.ShipPtr[0];

				if (rmCanBuildShip(player, bMothershipSupport, 1))
				{
					spdlog::info("AI PLAYER: Building support modules");

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
					aisRequestShip(player, sMothershipDockingBay, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipMicro)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipMicro))
				{
					aisRequestShip(player, sMothershipMicro, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipWeapons)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipWeapons))
				{
					aisRequestShip(player, sMothershipWeapons, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipSpecial)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipSpecial))
				{
					aisRequestShip(player, sMothershipSpecial, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipArmour)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipArmour))
				{
					aisRequestShip(player, sMothershipArmour, 0);
				}

				if (!selNumShipsInSelection(&aisTeams[AISTeamType::Main].selection[0].sel, sMothershipBigGun)
					&& !selNumShipsInSelection(&aisTeams[AISTeamType::MothershipModules].selection[0].sel, sMothershipBigGun))
				{
					aisRequestShip(player, sMothershipBigGun, 5000);
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
					aisRequestShip(player, shipToBuild, buildCost);
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

			aisRequestShip(player, shipToBuild, buildCost);
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
		? GetUnassignedShipCount(bWorker, dword_8DFD28)
		: GetUnassignedShipCount(sWorker, dword_8DFC9C);

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
		aisRequestShip(player, workerType, 0);
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
			? GetUnassignedShipCount(bRecon, dword_8DFCE8)
			: GetUnassignedShipCount(sRecon, dword_8DFC64);

		ShipType reconType = player->race == RACE_Sect ? sRecon : bRecon;
		if (reconTargetCount > reconCount)
		{
			aisRequestShip(player, reconType, 50);
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
			GetUnassignedShipCount(bDFGFrigate, dword_8DFD18);

		if (dfgFrigateCount < dfgFrigateTargetCount)
		{
			aisRequestShip(player, bDFGFrigate, 2000);
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
			GetUnassignedShipCount(sLeech, dword_8DFC70);

		if (leechCount < leechTargetCount)
		{
			aisRequestShip(player, sLeech, 100);
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
			GetUnassignedShipCount(sMimic, dword_8DFC6C);

		if (mimicCount < mimicTargetCount)
		{
			aisRequestShip(player, sMimic, 150);
			aisRequestShip(player, sMCV, 300);
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
			GetUnassignedShipCount(bCruiseMissile, dword_8DFD4C);

		if (cruiseMissileCount < cruiseMissileTargetCount)
		{
			aisRequestShip(player, bCruiseMissile, 100);
		}
	}

	if (aisTeams[AISTeamType::InterceptorEnemy].totalteamsize > 15)
	{
		if (player->race == RACE_Beast)
		{
			aisRequestShip(player, bMultiBeamFrigate, 800);
			aisRequestShip(player, bMultiGunCorvette, 400);
		}
		else
		{
			aisRequestShip(player, sMultiBeamFrigate, 800);
		}

// LABEL_230:

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
			return;

		udword interceptorCount = aisTeams[AISTeamType::Interceptor].totalteamsize;
		if (player->race == RACE_Beast)
		{
			interceptorCount += GetUnassignedShipCount(bAcolyte, dword_8DFCEC);
			interceptorCount += GetUnassignedShipCount(bInterceptor, dword_8DFCF0);
			interceptorCount += GetUnassignedShipCount(bCloakedFighter, dword_8DFCF0);
			interceptorCount += GetUnassignedShipCount(bAttackBomber, dword_8DFCF8);
			interceptorCount += GetUnassignedShipCount(bACV, dword_8DFCFC);
		}
		else
		{
			interceptorCount += GetUnassignedShipCount(sAcolyte, dword_8DFC68);
			interceptorCount += GetUnassignedShipCount(sACV, dword_8DFC74);
		}

		if (interceptorCount < interceptorTargetCount)
		{
			if (player->race == RACE_Beast)
			{
				aisRequestShip(player, bInterceptor, 100);

				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_BEG)
				{
					aisRequestShip(player, bAttackBomber, 150);
					aisRequestShip(player, bCloakedFighter, 150);
				}

				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_INT)
				{
					aisRequestShip(player, bAcolyte, 150);
					aisRequestShip(player, bACV, 300);
				}
			}
			else // RACE_Sect
			{
				aisRequestShip(player, sAcolyte, 150);
				if (aiCurrentAIPlayer->aiplayerDifficultyLevel > AI_BEG)
				{
					aisRequestShip(player, sACV, 300);
				}
			}
		}

		return;
	}

	bool32 v82;
	ShipType mediumShipToBuild = -1;
	if (player->race == RACE_Beast)
	{
		if (!rmCanBuildShip(player, bHeavyCruiser, 1 || *aiHasExternalConstruction))
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
		}
		else
		{
			v82 = true;
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
		}
		else
		{
			v82 = true;
		}
	}

	assert(mediumShipToBuild >= 0);
	ShipStaticInfo* shipstatic = GetShipStaticInfo(mediumShipToBuild);
	aisRequestShip(player, mediumShipToBuild, shipstatic->buildCost);

	// FIXME: Add scuttle/kamikaze logic
}