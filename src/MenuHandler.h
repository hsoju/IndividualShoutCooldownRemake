#pragma once

namespace MagicMenuTracker
{
	bool is_menu_opened = false;
}

class MenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (a_event->menuName == RE::MagicMenu::MENU_NAME) {
			MagicMenuTracker::is_menu_opened = a_event->opening;
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static bool Register()
	{
		static MenuHandler singleton;

		auto ui = RE::UI::GetSingleton();
		if (!ui) {
			logger::error("UI event source not found");
			return false;
		}

		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&singleton);
		
		logger::info("Registered {}", typeid(singleton).name());

		return true;
	}
};