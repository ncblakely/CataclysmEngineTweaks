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
    return 0; 
}

static BOOL etgFrequencyExceeded(etgeffectstatic* stat)
{
    // Always display all ETG effects in high detail mode
    return FALSE;
}

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
    void* _discard = nullptr;

    CreateAndEnableHook(Functions::lodLevelGet, lodLevelGet, &orig_lodLevelGet);
    CreateAndEnableHook(Functions::etgFrequencyExceeded, lodLevelGet, &_discard);
}

void ApplyLODPatches(Assembler& assembler, Config& config)
{
    if (g_Config.IsHighDetailModeEnabled())
    {
        CreateAndEnableHooks(assembler, config);
    }
}