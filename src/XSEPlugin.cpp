#include "SpellCastEventHandler.h"
#include "EquipEventHandler.h"
#include "InputEventHandler.h"
#include "MenuHandler.h"

bool SetupEquipHandler() {
	EquipEventHandler::Register();
	return true;
}

bool SetupSettings() {
	//SwitchManager::GetSingleton()->ImportSettings();
	return true;
}

bool SetupSpellCastHandler()
{
	SpellCastEventHandler::Register();
	return true;
}

bool SetupFavoritesTracker()
{
	//MenuHandler::Register();
	//InputEventHandler::Register();
	return true;
}