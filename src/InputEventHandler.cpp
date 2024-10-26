#include "InputEventHandler.h"

void InputEventHandler::HandleMenuInput(RE::InputEvent* const* a_event)
{
	if (a_event) {
		for (RE::InputEvent* given_input = *a_event; given_input; given_input = given_input->next) {
			if (given_input->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) {
				continue;
			}
			const RE::ButtonEvent* given_button = given_input->AsButtonEvent();
			if (given_button->IsPressed() || given_button->IsHeld()) {
				continue;
			}

			const RE::IDEvent* given_id = given_input->AsIDEvent();
			const auto& given_user = given_id->userEvent;
			const auto user_events = RE::UserEvents::GetSingleton();

			if (given_user == user_events->toggleFavorite || given_user == user_events->yButton) {
				manager->ProcessFavoritePowers();
				break;
			}
		}
	}
}