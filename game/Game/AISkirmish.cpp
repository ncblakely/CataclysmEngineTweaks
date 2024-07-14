#include "pch.h"

#include "Cataclysm.h"
#include "AIPlayer.h"
#include "AISkirmish.h"

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
			if (aisTeams[AISTeamType::Mothership].team.teamsize > 0 && supportModuleCount < 12 && !*aiHasSupportModuleQueued)
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
				auto supportLeader = aisTeams[AISTeamType::MothershipSupport].team.sel[0].ShipPtr[0];
				if (aisTeams[AISTeamType::Mothership].totalteamsize > 0 && supportModuleCount < 12 && !*aiHasSupportModuleQueued)
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
	}
}