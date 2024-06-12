/*=============================================================================
    Name    : MultiplayerGame.c
    Purpose : Logic for the multiplayergame setup.

    Created 6/22/1998 by ddunlop
    Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#include "pch.h"

#include "Cataclysm.h"

#include "FeFlow.h"
#include "UIcontrols.h"

#define MAX_NUM_LENGTH          12

using namespace Functions;
using namespace Globals;

// nb: there was a tricky compiler optimization in these that made replacing the functions simpler.
void mgResourceInjectionInterval(char* name, featom* atom)
{
    char    temp[20];
    udword  timemins;

    if (FEFIRSTCALL(atom))
    {
        // initialize button here
        *mgResourceInjectionIntervalEntryBox = (textentryhandle)atom->pData;
        timemins = (udword)((tpGameCreated->resourceInjectionInterval) / (60 * (udword)g_Config.GetUniverseUpdateRate()));
        sprintf(temp, "%d", timemins);
        uicTextEntrySet(*mgResourceInjectionIntervalEntryBox, temp, strlen(temp) + 1);
        uicTextBufferResize(*mgResourceInjectionIntervalEntryBox, MAX_NUM_LENGTH - 2);
        uicTextEntryInit(*mgResourceInjectionIntervalEntryBox, UICTE_NumberEntry);
        mgGameTypesOtherButtonPressed();
        return;
    }

    if (atom == nullptr)
    {
        return;
    }

    switch (uicTextEntryMessage(atom))
    {
        case CM_LoseFocus:
        case CM_AcceptText:
            sscanf((*mgResourceInjectionIntervalEntryBox)->textBuffer, "%d", &timemins);
            // change the units of minutes into Univupdate slices
            tpGameCreated->resourceInjectionInterval = timemins * 60 * (udword)g_Config.GetUniverseUpdateRate();
            if (uicTextEntryMessage(atom) == CM_AcceptText) feToggleButtonSet(const_cast<char*>("MG_ResourceInjections"), TRUE);
            break;
        case CM_GainFocus:
            break;
    }
    mgGameTypesOtherButtonPressed();
}

void mgResourceLumpSumInterval(char* name, featom* atom)
{
    char    temp[20];
    udword  timemins;

    if (FEFIRSTCALL(atom))
    {
        // initialize button here
        *mgResourceLumpSumIntervalEntryBox = (textentryhandle)atom->pData;
        timemins = (udword)(tpGameCreated->resourceLumpSumTime / (60 * (udword)g_Config.GetUniverseUpdateRate()));
        sprintf(temp, "%d", timemins);
        uicTextEntrySet(*mgResourceLumpSumIntervalEntryBox, temp, strlen(temp) + 1);
        uicTextBufferResize(*mgResourceLumpSumIntervalEntryBox, MAX_NUM_LENGTH - 2);
        uicTextEntryInit(*mgResourceLumpSumIntervalEntryBox, UICTE_NumberEntry);
        mgGameTypesOtherButtonPressed();
        return;
    }

    switch (uicTextEntryMessage(atom))
    {
        case CM_LoseFocus:
        case CM_AcceptText:
            sscanf((*mgResourceLumpSumIntervalEntryBox)->textBuffer, "%d", &timemins);
            tpGameCreated->resourceLumpSumTime = timemins * 60 * (udword)g_Config.GetUniverseUpdateRate();
            if (uicTextEntryMessage(atom) == CM_AcceptText) feToggleButtonSet(const_cast<char*>("MG_ResourceLumpSum"), TRUE);
            break;
        case CM_GainFocus:
            break;
    }

    mgGameTypesOtherButtonPressed();
}