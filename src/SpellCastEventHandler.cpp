#include "ShoutRecoveryHandler.h"
#include "SpellCastEventHandler.h"

void SpellCastEventHandler::HandleShout(RE::TESObjectREFR* caster, RE::SpellItem* casted_power)
{
	RE::Actor* player = caster->As<RE::Actor>();
	RE::TESShout* shout = player->GetCurrentShout();
	if (shout) {
		ShoutRecoveryHandler::GetSingleton()->AsyncSetupCatch(shout);
	}
}