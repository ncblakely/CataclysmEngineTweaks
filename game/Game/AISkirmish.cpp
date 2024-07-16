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

void aisFleetUpdate()
{
	using namespace Functions;
	using namespace Globals;

	Player* player = aiCurrentAIPlayer->player;
	Ship* playerMothership = player->PlayerMothership;

	DumpAITeams();

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
							if (shipstatic->canReceiveShipsForRetire)
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
			int weight = 0;
			if (player->race == RACE_Beast)
			{
				if (rmCanBuildShip(player, bCarrier, RACE_Beast) && (int)aisTeams[AISTeamType::Carrier].totalteamsize < MAX_CARRIERS)
				{
					shipToBuild = bCarrier;
					weight = 2000;
				}
				else if (rmCanBuildShip(player, bHeavyCruiser, 1) && player->resourceUnits > 2000)
				{
					shipToBuild = bHeavyCruiser;
					weight = 3000;
				}
				else if (rmCanBuildShip(player, bProcessor, 1)
					&& player->resourceUnits > 1000
					&& !aisTeams[AISTeamType::Processor].totalteamsize)
				{
					shipToBuild = bProcessor;
					weight = 1000;
				}
			}
			else
			{
				if (rmCanBuildShip(player, sCarrier, 1) && (int)aisTeams[AISTeamType::Carrier].totalteamsize < MAX_CARRIERS)
				{
					shipToBuild = sCarrier;
					weight = 2000;
					aisRequestShip(player, shipToBuild, weight);
				}
				else if (rmCanBuildShip(player, sDreadnought, 1) && player->resourceUnits > 2000)
				{
					shipToBuild = sDreadnought;
					weight = 3000;
				}
				else if (rmCanBuildShip(player, sDestroyer, 1) && player->resourceUnits > 1000)
				{
					shipToBuild = sDestroyer;
					weight = 1500;
				}
				else if (rmCanBuildShip(player, sProcessor, 1) && player->resourceUnits > 1000 && !aisTeams[AISTeamType::Processor].totalteamsize)
				{
					shipToBuild = sProcessor;
					weight = 1000;
				}
			}

			aisRequestShip(player, shipToBuild, weight);
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

	udword workerCount;
	if (player->race == RACE_Beast)
	{
		workerCount = *dword_8DFD28;

		udword count = aisTeams[AISTeamType::Main].numselections;
		for (int i = 0; i < count; i++)
		{
			MaxSelection* sel = &aisTeams[AISTeamType::Main].selection[i].sel;
			for (udword i = 0; i < sel->numShips; i++)
			{
				Ship* ship = sel->ShipPtr[i];
				if (ship->shiptype == bWorker)
				{
					workerCount++;
				}
			}
		}

		workerCount += aisTeams[AISTeamType::Worker].totalteamsize;
	}
	else // RACE_Sect
	{
		workerCount = *dword_8DFC9C;

		udword count = aisTeams[AISTeamType::Main].numselections;
		for (int i = 0; i < count; i++)
		{
			MaxSelection* sel = &aisTeams[AISTeamType::Main].selection[i].sel;
			for (udword i = 0; i < sel->numShips; i++)
			{
				Ship* ship = sel->ShipPtr[i];
				if (ship->shiptype == sWorker)
				{
					workerCount++;
				}
			}
		}

		workerCount += aisTeams[AISTeamType::Worker].totalteamsize;
	}

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
}