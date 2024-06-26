#include "pch.h"

#include "Cataclysm.h"
#include "PatchSaveGame.h"

void ApplySaveGamePatches(Assembler& assembler, Config& config)
{
    // Normally, this structure contains the offset of the current SongNumber.
    // The base game has a bug where it will read these offsets and zero them out, right after loading the "pre game" data which contains the SongNumber.
    // This redirects the game to a dummy static variable instead, so that the SongNumber is not zeroed out.
    static sdword songNumberDummy;
    *Globals::SongNumberLoadGameOffset = &songNumberDummy;
}