#pragma once

#include "ShoutRecoveryHandler.h"


class EquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
		if (!a_event || !a_event->actor || !a_event->actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto form = RE::TESForm::LookupByID(a_event->baseObject);
		if (!form || form->GetFormType() != RE::FormType::Shout) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto shout = form->As<RE::TESShout>();
		ShoutRecoveryHandler::GetSingleton()->SetShoutCooldown(shout);

		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static EquipEventHandler singleton;

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