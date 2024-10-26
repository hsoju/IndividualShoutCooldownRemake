#include "SpellCastEventHandler.h"
#include "EquipEventHandler.h"

bool SetupEquipHandler() {
	EquipEventHandler::Register();
	return true;
}

bool SetupSpellCastHandler()
{
	SpellCastEventHandler::Register();
	return true;
}