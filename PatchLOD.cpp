#include "pch.h"
#include "Main.h"
#include "PatchLOD.h"
#include "Cataclysm.h"

// This function was changed significantly from HW1 and just returns an int LOD level now.
static Functions::fn_lodLevelGet orig_lodLevelGet = nullptr;
static int lodLevelGet(void* spaceObj, vector* camera, vector* ship)
{
    SpaceObj* obj = (SpaceObj*)spaceObj;

    // Skip all of this and just set and return the highest LOD level.
    obj->currentLOD = 0;

    vecSub(obj->cameraDistanceVector, *camera, *ship);
    obj->cameraDistanceSquared = vecMagnitudeSquared(obj->cameraDistanceVector);
    return 0; 
}

static void trCramRAMComputeAndScale()
{
    // Do nothing
}

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
    void* _discard = nullptr;

    CreateAndEnableHook(Functions::lodLevelGet, lodLevelGet, &orig_lodLevelGet);

    // Not working yet, disable for now
    /*
    if (g_Config.IsUnlimitedVideoMemoryEnabled())
    {
        CreateAndEnableHook(Functions::trCramRAMComputeAndScale, &trCramRAMComputeAndScale, &_discard);
    }
    */
}

void ApplyLODPatches(Assembler& assembler, Config& config)
{
    if (g_Config.HighDetailMode)
    {
        CreateAndEnableHooks(assembler, config);
    }
}