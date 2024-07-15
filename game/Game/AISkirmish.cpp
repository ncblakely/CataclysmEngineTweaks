#include "pch.h"

#include "Cataclysm.h"
#include "AIPlayer.h"
#include "AISkirmish.h"

#define MAX_SUPPORT_MODULES 12

void aisFleetUpdate()
{
	using namespace Functions;
	using namespace Globals;

	Player* player = aiCurrentAIPlayer->player;
	Ship* playerMothership = player->PlayerMothership;

	if (playerMothership && playerMothership->staticinfo->shipclass == CLASS_Mothership)
	{
		if (player->race == RACE_Beast)
		{
			udword supportModuleCount = aisTeams[AISTeamType::MothershipSupport].team.teamsize;
			if (aisTeams[AISTeamType::Mothership].team.teamsize > 0 && supportModuleCount < MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
			{
				Ship* leader = aisTeams[AISTeamType::Mothership].team.sel[0].ShipPtr[0];

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
				if (!selNumShipsInSelection(aisTeams[AISTeamType::Main].team.sel, sMothershipDockingBay) 
					&& !selNumShipsInSelection(aisTeams[AISTeamType::SMothershipModules].team.sel, sMothershipDockingBay))
				{
					aisRequestShip(player, sMothershipDockingBay, 0);
				}

				if (!selNumShipsInSelection(aisTeams[AISTeamType::Main].team.sel, sMothershipMicro)
					&& !selNumShipsInSelection(aisTeams[AISTeamType::SMothershipModules].team.sel, sMothershipMicro))
				{
					aisRequestShip(player, sMothershipMicro, 0);
				}

				if (!selNumShipsInSelection(aisTeams[AISTeamType::Main].team.sel, sMothershipWeapons)
					&& !selNumShipsInSelection(aisTeams[AISTeamType::SMothershipModules].team.sel, sMothershipWeapons))
				{
					aisRequestShip(player, sMothershipWeapons, 0);
				}

				if (!selNumShipsInSelection(aisTeams[AISTeamType::Main].team.sel, sMothershipSpecial)
					&& !selNumShipsInSelection(aisTeams[AISTeamType::SMothershipModules].team.sel, sMothershipSpecial))
				{
					aisRequestShip(player, sMothershipSpecial, 0);
				}

				if (!selNumShipsInSelection(aisTeams[AISTeamType::Main].team.sel, sMothershipArmour)
					&& !selNumShipsInSelection(aisTeams[AISTeamType::SMothershipModules].team.sel, sMothershipArmour))
				{
					aisRequestShip(player, sMothershipArmour, 0);
				}

				if (!selNumShipsInSelection(aisTeams[AISTeamType::Main].team.sel, sMothershipBigGun)
					&& !selNumShipsInSelection(aisTeams[AISTeamType::SMothershipModules].team.sel, sMothershipBigGun))
				{
					aisRequestShip(player, sMothershipBigGun, 5000);
				}

				auto supportModuleCount = aisTeams[AISTeamType::MothershipSupport].totalteamsize;
				if (aisTeams[AISTeamType::Mothership].totalteamsize > 0 && supportModuleCount < MAX_SUPPORT_MODULES && !*aiHasSupportModuleQueued)
				{
					Ship* leader = aisTeams[AISTeamType::Mothership].team.sel[0].ShipPtr[0];
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
				if (rmCanBuildShip(player, bCarrier, RACE_Beast) && (int)aisTeams[AISTeamType::Carrier].totalteamsize < 2)
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
				if (rmCanBuildShip(player, sCarrier, 1) && (int)aisTeams[AISTeamType::Carrier].totalteamsize < 2)
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
}