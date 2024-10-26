#include "ShoutRecoveryHandler.h"
#include "SpellCastEventHandler.h"

RE::BGSEquipSlot* GetVoiceSlot()
{
	using func_t = decltype(&GetVoiceSlot);
	const REL::Relocation<func_t> func{ RELOCATION_ID(23153, 23610) };
	return func();
}

void HandleShoutCooldown(RE::TESObjectREFR* caster)
{
	RE::Actor* player = caster->As<RE::Actor>();
	auto& runtimeData = player->GetActorRuntimeData();
	auto* process = runtimeData.currentProcess;
	if (process) {
		auto* high_data = process->high;
		if (high_data) {
			auto recovery = high_data->voiceRecoveryTime;
		}
	}
}

void SpellCastEventHandler::HandleShout(RE::TESObjectREFR* caster, RE::SpellItem* casted_power)
{
	RE::Actor* player = caster->As<RE::Actor>();
	RE::TESShout* shout = player->GetCurrentShout();
	if (shout) {
		ShoutRecoveryHandler::GetSingleton()->AsyncSetupCatch(shout);
	}
}