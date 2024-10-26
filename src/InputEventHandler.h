#pragma once

#include "MenuHandler.h"
#include "SwitchManager.h"


class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:
	SwitchManager* manager = SwitchManager::GetSingleton();

	void HandleMenuInput(RE::InputEvent* const* a_event);

	virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
	{
		if (MagicMenuTracker::is_menu_opened) {
			HandleMenuInput(a_event);
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static InputEventHandler singleton;

		auto input_manager = RE::BSInputDeviceManager::GetSingleton();
		if (!input_manager) {
			logger::error("Input event source not found");
			return false;
		}
		input_manager->AddEventSink(&singleton);
		logger::info("Registered {}", typeid(singleton).name());
		
		return true;
	}
};