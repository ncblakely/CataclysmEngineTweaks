/*=============================================================================
    Name    : Universe.h
    Purpose : Definitions for Universe.c

    This module will be able to create mission spheres, and the universe
    conceptually contains all of the mission spheres in a game.

    It will also contain information global to all mission spheres, such as the
    static properties of the different ship types.

    Created 6/19/1997 by gshaw
    Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#ifndef ___UNIVERSE_H
#define ___UNIVERSE_H

#include "types.h"
#include "spaceobj.h"
#include "objtypes.h"
#include "camera.h"
#include "cameracommand.h"
#include "commandlayer.h"
#ifndef STATVIEWER_PROGRAM
#include "star3d.h"
#include "ConsMgr.h"
#include "globals.h"

#include "gamestats.h"
#endif
#include "researchapi.h"
#include "objtypes.h"
#include "shipselect.h"

/*=============================================================================
    Switches:
=============================================================================*/
#ifndef HW_Release

#define UNIVERSE_TURBOPAUSE_DEBUG       1       //enable turbo and pause modes
#define UNIVERSE_TURBORECORD_ONLY       0
#define UNIV_SHIP_LOADFREE_LOG          1       //spit out a list of what ships were loaded and freed
#define UNIV_LOAD_FREE_FILENAME         "loadfree.log"

#else //HW_Debug

#define UNIVERSE_TURBORECORD_ONLY       1
#define UNIVERSE_TURBOPAUSE_DEBUG       1       //enable turbo and pause modes
#define UNIV_SHIP_LOADFREE_LOG          0       //spit out a list of what ships were loaded and freed

#endif //HW_Debug

/*=============================================================================
    Defines:
=============================================================================*/

#define UNIVERSE_UPDATE_RATE        16
#define UNIVERSE_UPDATE_PERIOD      (1.0f/(real32)UNIVERSE_UPDATE_RATE)
#define UNIVERSE_UPDATE_STACK_SIZE  100000

#define RENDER_STACK_SIZE 100000

#define UNIV_NUMBER_WORLDS          3           //number of planets we can keep track of

/*=============================================================================
    Types:
=============================================================================*/

#define MAXNUM_RESOURCETYPES   10

// for regrowing resources
typedef struct ResourceDistribution
{
    uword probResources[MAXNUM_RESOURCETYPES][2];
    uword cumResources[MAXNUM_RESOURCETYPES][2];
    uword sumTotal[2];
} ResourceDistribution;

typedef enum ResourceVolumeType {
    ResourceSphereType,
    ResourceCylinderType,
    ResourceRectangleType
} ResourceVolumeType;

typedef struct ResourceVolume
{
    Node link;
    ResourceVolumeType resourceVolumeType;
    vector centre;
    real32 radius;
    sdword number;
    sdword actualnumber;
    udword attributes;
    sword attributesParam;
    ubyte attributesPad[2];
    ObjType resourceObjType;
    ResourceDistribution resourceDistribution;

    real32 length;      // only valid for ResourceCylinderType and ResourceRectangleType
    real32 roty;
    real32 rotz;
} ResourceVolume;

typedef enum PlayerState
{
    PLAYER_ALIVE = 0,
    PLAYER_DEAD
} PlayerState;

typedef struct Player
{
    Ship *PlayerMothership;
    sdword resourceUnits;
    udword unk;
    sdword supportUnitsMax;
    sdword supportUnitsUsed;
    ShipRace race;
    PlayerState playerState;
    struct AIPlayer *aiPlayer;      // if non-null, then player has a computer AI Player
    udword autoLaunch;
    uword playerIndex;
    ubyte sensorLevel;
    ubyte bounty;                   //range from 0 to 100
    real32 timeMoShipAttacked;
    sdword totalships;                   // total ships for player
    sdword shiptotals[TOTAL_NUM_SHIPS];  // totals for each type of ship
    sdword classtotals[NUM_CLASSES];
    PlayerResearchInfo researchinfo;    // research information for player
    uword Allies;                       // bitmask of players that i am allied with
    uword AllianceProposals;
    udword AllianceProposalsTimeout;    // timeout counter for proposals to expire!
    sdword initialShipCost;				// total ship cost at game start
} Player;

// TODO: This is significantly larger in Cataclysm. Needs reversing.
// static_assert(sizeof(Player) == 0x6530u);

ASSERT_OFFSET(Player, PlayerMothership, 0x0);
ASSERT_OFFSET(Player, resourceUnits, 0x4);
ASSERT_OFFSET(Player, supportUnitsMax, 0xC);
ASSERT_OFFSET(Player, supportUnitsUsed, 0x10);
ASSERT_OFFSET(Player, race, 0x14);
ASSERT_OFFSET(Player, aiPlayer, 0x1C);
ASSERT_OFFSET(Player, playerIndex, 0x24);
ASSERT_OFFSET(Player, timeMoShipAttacked, 0x28);
ASSERT_OFFSET(Player, totalships, 0x2C);
ASSERT_OFFSET(Player, shiptotals, 0x30);
ASSERT_OFFSET(Player, classtotals, 0x274);

typedef struct
{
    CommandLayer mainCommandLayer;      // everything goes through main command layer
    CameraCommand mainCameraCommand;    // camera command (focus stack)

    LinkedList RenderList;      // Linked list of all objects (SpaeceObj's to be rendered)
    LinkedList MinorRenderList;
    BOOL dontUpdateRenderList;

    sdword unknown1; // @CATA: TODO: This structure has changed; this may not be the correct position for this
    real32 phystimeelapsed;
    real32 totaltimeelapsed;
    real32 realtimeelapsed; // @CATA: New timer keyed off QueryPerformanceCounter

    real32 radius;

    LinkedList SpaceObjList;
    LinkedList MinorSpaceObjList;
    LinkedList effectList;
    LinkedList ShipList;
    LinkedList BulletList;
    LinkedList ResourceList;
    LinkedList DerelictList;
    LinkedList ImpactableList;
    LinkedList MissileList;
    LinkedList MineFormationList;

    LinkedList DeleteResourceList;
    LinkedList DeleteDerelictList;
    LinkedList DeleteShipList;
    LinkedList DeleteMissileList;

    LinkedList collBlobList;

    // tactics stuff
    LinkedList RetreatList;
    // LinkedList AttackMemory; // @CATA: Either RetreatList or AttackMemory was removed

    // regrow resources stuff
    LinkedList ResourceVolumeList;

    GrowSelection HousekeepShipList;       // special GrowSelect which can have NULL's in it
    GrowSelection unk1;

    Star3dInfo *star3dinfo; 
    udword univUpdateCounter;
    color backgroundColor;
    uword resourceNumber;
    uword derelictNumber;
    uword shipNumber;
    uword missileNumber;

    real32 wintime;             // time to display the win game message
    real32 quittime;            // time to quit the game

    udword lasttimeadded;       // for resource injections

    uword shipMaxUnits[TOTAL_NUM_SHIPS];

    sbyte aiplayerEnemy[MAX_MULTIPLAYER_PLAYERS];
    BOOL  aiplayerProcessing;

    sdword CapitalShipCaptured;     // bit field of players who have had a ship captured
    sdword PlayerWhoWon;            // bit field of players who have captured a enemy's ship

    sdword bounties;
    sdword bountySize;

    GameStats gameStats;
    BYTE unk3[0x1F8];
    BOOL DerelictTech;              //variable that needs to be saved that controls the salvagability of technology holding derelicts

    real32 crateTimer;
    udword numCratesInWorld;

    bool32 collUpdateAllBlobs;        // put in here so it gets saved with SaveGame

    uword numPlayers;
    uword curPlayerIndex;
    Player *curPlayerPtr;
    Player players[MAX_MULTIPLAYER_PLAYERS+1];      // players[MAX_MULTIPLAYER_PLAYER] is a enemy of all players and used for autoguns, etc.
    Derelict *world[UNIV_NUMBER_WORLDS];        //list of planets
} Universe;

static_assert(offsetof(Universe, phystimeelapsed) == 0xBC);
static_assert(offsetof(Universe, totaltimeelapsed) == 0xC0);
static_assert(offsetof(Universe, realtimeelapsed) == 0xC4);
static_assert(offsetof(Universe, radius) == 0xC8);
static_assert(offsetof(Universe, effectList) == 0xE4);
static_assert(offsetof(Universe, ShipList) == 0xF0);
static_assert(offsetof(Universe, HousekeepShipList) == 0x198);
static_assert(offsetof(Universe, unk1) == 0x1A0);
static_assert(offsetof(Universe, star3dinfo) == 0x1A8);
static_assert(offsetof(Universe, univUpdateCounter) == 0x1AC);
static_assert(offsetof(Universe, wintime) == 0x1BC);
static_assert(offsetof(Universe, quittime) == 0x1C0);
static_assert(offsetof(Universe, lasttimeadded) == 0x1C4);
static_assert(offsetof(Universe, aiplayerEnemy) == 0x2EA);
static_assert(offsetof(Universe, aiplayerProcessing) == 0x2F4);
static_assert(offsetof(Universe, CapitalShipCaptured) == 0x2F8);
static_assert(offsetof(Universe, PlayerWhoWon) == 0x2FC);
static_assert(offsetof(Universe, bounties) == 0x300);
static_assert(offsetof(Universe, bountySize) == 0x304);
static_assert(offsetof(Universe, gameStats) == 0x308);
static_assert(offsetof(Universe, gameStats.universeRUWorth) == 0x328);
static_assert(offsetof(Universe, numCratesInWorld) == 0x4AF0);
static_assert(offsetof(Universe, numPlayers) == 0x4AF8);
static_assert(offsetof(Universe, players) == 0x4B00);

#if 0
extern GrowSelection ClampedShipList;       // special GrowSelect which can have NULL's in it

#endif


typedef void (*univstatcallback)(StaticInfo *info, ObjType objType);

/*=============================================================================
    Public Variables
=============================================================================*/

#if 0
extern Universe universe;
extern AsteroidStaticInfo asteroidStaticInfos[NUM_ASTEROIDTYPES];
extern DustCloudStaticInfo dustcloudStaticInfos[NUM_DUSTCLOUDTYPES];
extern GasCloudStaticInfo gascloudStaticInfos[NUM_GASCLOUDTYPES];
extern DerelictStaticInfo derelictStaticInfos[NUM_DERELICTTYPES];
extern NebulaStaticInfo nebulaStaticInfos[NUM_NEBULATYPES];

extern Camera defaultCamera;

extern MissileStaticInfo missileStaticInfos[NUM_RACES];
extern MissileStaticInfo mineStaticInfos[2];

extern struct SphereStaticInfo *sphereStaticInfo;

extern real32 maxMineCollSphereSize;
//extern real32 maxShipCollSphereSize;
//extern real32 maxResourceCollSphereSize;
//extern real32 maxDerelictCollSphereSize;
//extern real32 maxCollSphereSize;

extern meshdata *defaultmesh;

extern uword RacesAllowedForGivenShip[TOTAL_NUM_SHIPS];

#if UNIVERSE_TURBOPAUSE_DEBUG
extern bool universeTurbo;
#endif
extern bool universePause;

extern sdword cdMaxShipsAllowed;            // max number of ships allowed
extern sdword cdLimitCaps[TOTAL_NUM_SHIPS]; // max number of ships per player allowed
extern sdword cdClassCaps[NUM_CLASSES];     // max number of ships per class allowed
extern bool   cdEnabled;                    // flag specifying whether unit caps enabled

extern lodinfo *defaultlod;                 // remove later when all stubs are taken out
extern void *defaultmex;                    // remove later when all stubs are taken out
extern bool universeForceDefaultShip;       //flag for forcing ships to be loaded as rave borgs

#if UNIV_SHIP_LOADFREE_LOG
extern bool univLoadFreeLog;
#endif

extern real32 rowAvoidBy[TOTAL_NUM_SHIPS];
extern sbyte rightOfWays[TOTAL_NUM_SHIPS];

#ifdef DEBUG_GAME_STATS

typedef struct
{
    sdword numPlayers;
    char names[MAX_MULTIPLAYER_PLAYERS+1][20];
}GameStatsDebugHeader;

void gameStatsInitLog();
void gameStatsUpdateLogFile(sdword framenum);

#endif
/*=============================================================================
    Functions:
=============================================================================*/

void universeClosePlayers(void);
void universeInit(void);
void universeClose(void);
void universeReset(void);
void universeSetZeroPlayers(void);
void universeStaticInit(void);
void universeStaticClose(void);
void universeUpdateTask(void);
void universeSwitchToNextPlayer(void);
void universeSwitchToPlayer(uword playerIndex);

ShipRace GetValidRaceForShipType(ShipType shiptype);    // returns valid race for shiptype, -1 if none found
ShipStaticInfo *GetShipStaticInfo(ShipType shiptype,ShipRace shiprace);
ShipStaticInfo *GetShipStaticInfoValidate(ShipType shiptype,ShipRace shiprace); // same as above, but makes sure data loaded
ShipStaticInfo *GetShipStaticInfoSafe(ShipType shiptype,ShipRace shiprace); // same as above, but always safe (returns NULL if fails)
void InitStatShipInfo(ShipStaticInfo *statinfo,ShipType type,ShipRace race);
void CloseStatShipInfo(ShipStaticInfo *statinfo);
void CloseStatAsteroidInfo(AsteroidStaticInfo *asteroidStatInfo);
void CloseStatDerelictInfo(DerelictStaticInfo *derelictStatInfo);
void CloseStatDustCloudInfo(DustCloudStaticInfo *dustcloudStatInfo);
void CloseStatGasCloudInfo(GasCloudStaticInfo *gascloudStatInfo);
void CloseStatMissileInfo(MissileStaticInfo *missileStatInfo);
void CloseStatMineInfo(MissileStaticInfo *mineStatInfo);
void CloseStatEffectInfo(etgeffectstatic *effectStatInfo);
void InitStatDerelictInfoByPath(DerelictStaticInfo *derelictStatInfo, DerelictType derelicttype, char *directory, char *shipFile);

void unitCapCreateShip(Ship *ship, Player *player);
void unitCapDeleteShip(Ship *ship, Player *player);
bool unitCapCanCreateShip(ShipType ship, shipsinprogress *factory, shipavailable *cmShipsAvail);
void unitCapInitialize(Player *player);
void unitCapEnable(void);
void unitCapDisable(void);

void universeRealTimeTweak(SelectCommand *selection);
void universeDefaultTeamColors(void);
extern void universeFlagRaceNeeded(ShipRace shiprace, bool8 bNeeded);
void universeFlagEverythingNeeded(void);
void universeSaveEverythingNeeded(void);
void universeLoadEverythingNeeded(void);
void universeRaceStaticsScan(ShipRace shiprace, univstatcallback forShips, univstatcallback forMisc);
void universeAllStaticsScan(univstatcallback forShips, univstatcallback forDerelicts, univstatcallback forResources, univstatcallback forMisc);

void resourceInjectionTask(void);
void resourceLumpSum(void);

void initGameStats(Universe *universe);
void gameStatsAddShip(Ship *ship,sdword playerIndex);
void writeGameStatsToFile(char *filename);
void gameStatsShipDied(Ship *ship);
void damageDoneToPlayerByPlayer(sdword DamagedIndex, sdword damageDoerIndex, real32 damage);
void gameStatsCalcShipCostTotals(void);

bool isStrikeCraft(ShipType type);

void unitCapInitStatics(udword numplayers);

#endif

#endif //___UNIVERSE_H

