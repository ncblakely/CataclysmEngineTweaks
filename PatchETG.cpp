#include "pch.h"
#include "Main.h"
#include "PatchETG.h"
#include "Cataclysm.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ETG debug hooks. Not enabled in release builds.

static Functions::fn_etgFunctionCall orig_etgFunctionCall = nullptr;
static sdword etgFunctionCall(Effect* effect, etgeffectstatic* stat, ubyte* opcode)
{
	etgfunctioncall* functionOpcode = (etgfunctioncall*)opcode;

	std::string debugString = fmt::format(
		"\netgFunctionCall: name = {}, function = 0X{:x}, nParameters = {}",
		stat->name,
		(udword)functionOpcode->function,
		functionOpcode->nParameters);

	if (functionOpcode->nParameters > 0)
	{
		debugString += "\n{";

		for (udword i = 0; i < functionOpcode->nParameters; i++)
		{
			debugString += fmt::format("\n\t{} = {}", functionOpcode->parameter[i].type, functionOpcode->parameter[i].param);
		}

		debugString += "\n}";
	}

	spdlog::info(debugString);
	spdlog::default_logger()->flush();

	return orig_etgFunctionCall(effect, stat, opcode);
}

static Functions::fn_etgEffectDelete orig_etgEffectDelete = nullptr;
static void etgEffectDelete(Effect* effect)
{
	auto effectstatic = (etgeffectstatic*)effect->staticinfo;

	spdlog::info("etgEffectDelete: name = {}\n{{", effectstatic->name);
	orig_etgEffectDelete(effect);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

sdword etgNParticleBlocksSet(struct etgeffectstatic* stat, ubyte* dest, char* opcodeString, char* params, char* ret)
{
	sdword nScanned = sscanf(params, "%d", &stat->nParticleBlocks);

	// Some effects that create sprites per-frame will end up using more particle blocks than they have space allocated for at higher tick rates.
	// Increase the size of the particle block array to compensate.
	// TODO: This is more of a hack than a proper fix. Effect speed probably shouldn't be modified by the tick rate, but capping it to 16 FPS results in the
	// same choppy effect animation as in the original game.

	stat->nParticleBlocks <<= g_Config.GetUniverseUpdateRateShift();
	if (nScanned != 1)
	{
		spdlog::error("Error scanning number of particle blocks from '{}'", params);
		return(0);
	}

	return(0);
}

static BOOL etgFrequencyExceeded(etgeffectstatic* stat)
{
	// Always display all ETG effects in high detail mode
	return FALSE;
}

static void CreateAndEnableHooks(Assembler& assembler, Config& config)
{
	void* _discard = nullptr;

#if 0
	CreateAndEnableHook(Functions::etgFunctionCall, etgFunctionCall, &orig_etgFunctionCall);
	CreateAndEnableHook(Functions::etgEffectDelete, etgEffectDelete, &orig_etgEffectDelete);
#endif
	CreateAndEnableHook(Functions::etgNParticleBlocksSet, etgNParticleBlocksSet, &_discard);

	if (g_Config.IsHighDetailModeEnabled())
	{
		CreateAndEnableHook(Functions::etgFrequencyExceeded, etgFrequencyExceeded, &_discard);
	}
}

void ApplyETGPatches(Assembler& assembler, Config& config)
{
	if (g_Config.IsHighDetailModeEnabled())
	{
		CreateAndEnableHooks(assembler, config);
	}
}