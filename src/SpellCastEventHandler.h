#pragma once

#include "InputEventHandler.h"
#include "MenuHandler.h"
#include "SwitchManager.h"

class SpellCastEventHandler : public RE::BSTEventSink<RE::TESSpellCastEvent>
{
public:
	void HandleShout(RE::TESObjectREFR* caster, RE::SpellItem* casted_power);

	virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* a_event, RE::BSTEventSource<RE::TESSpellCastEvent>*)
	{
		auto caster = a_event->object.get();
		if (!caster || !caster->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (!a_event || !a_event->spell) {
			return RE::BSEventNotifyControl::kContinue;
		}

		RE::SpellItem* casted_spell = RE::TESForm::LookupByID<RE::SpellItem>(a_event->spell);
		auto spell_type = casted_spell->GetSpellType();
		if (spell_type == RE::MagicSystem::SpellType::kVoicePower) {
			HandleShout(caster, casted_spell);
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static SpellCastEventHandler singleton;
		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();

		if (!ScriptEventSource) {
			logger::error("Script event source not found");
			return false;
		}

		ScriptEventSource->AddEventSink(&singleton);

		logger::info("Registered {}", typeid(singleton).name());

		return true;
	}
};
