#include "ShoutRecoveryHandler.h"

float ShoutRecoveryHandler::GetCurrentGameTimeFromCalendar()
{
	auto calendar = RE::Calendar::GetSingleton();
	return calendar->GetCurrentGameTime() * 3600 * 24 / calendar->GetTimescale();
}

RE::HighProcessData* ShoutRecoveryHandler::GetPlayerData() {
	auto player = RE::PlayerCharacter::GetSingleton()->As<RE::Actor>();
	auto process = player->GetActorRuntimeData().currentProcess;
	if (process) {
		auto high_data = process->high;
		if (high_data) {
			return high_data;
		}
	}
	return nullptr;
}

void ShoutRecoveryHandler::SetShoutCooldown(RE::TESShout* shout) {
	auto player_data = GetPlayerData();
	if (player_data) {
		//auto player = RE::PlayerCharacter::GetSingleton();
		//RE::TESShout* current_shout = (shout != nullptr) ? shout : player->GetCurrentShout();
		if (shout_cooldowns.contains(shout) && shout_times.contains(shout)) {
			float previous_time = shout_times[shout];
			float current_time = GetCurrentGameTimeFromCalendar();
			float elapsed_time = current_time - previous_time;
			float remaining_time = shout_cooldowns[shout] - elapsed_time;
			float new_recovery_time = remaining_time < 0.0f ? 0.0f : remaining_time;
			AsyncWalk(new_recovery_time);
		} else {
			//player_data->voiceRecoveryTime = 0.0f;
			AsyncWalk(0.0f);
		}
	}
}

void ShoutRecoveryHandler::AsyncRun(RE::TESShout* shout)
{
	auto ThreadFunc = [this](RE::TESShout* shout) -> void {
		AsyncCatch(shout);
	};
	std::jthread thread(ThreadFunc, shout);
	thread.detach();
}

void ShoutRecoveryHandler::AsyncCatch(RE::TESShout* shout)
{
	float delay = 10.0f;
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
	const auto task_interface = SKSE::GetTaskInterface();

	if (task_interface != nullptr) {
		auto player = RE::PlayerCharacter::GetSingleton();
		if (player && player->GetActorRuntimeData().currentProcess &&
			player->GetActorRuntimeData().currentProcess->InHighProcess()) {
			task_interface->AddTask([this, player, shout]() -> void {
				shout_cooldowns[shout] = player->GetVoiceRecoveryTime();
				shout_times[shout] = GetCurrentGameTimeFromCalendar();
				//logger::info("Catch {}: Recovery = {}, Time = {}", shout->fullName.data(), shout_cooldowns[shout], shout_times[shout]);
			});
		}
	}
}

void ShoutRecoveryHandler::AsyncWalk(float recovery_time)
{
	auto ThreadFunc = [this](float recovery_time) -> void {
		AsyncCheck(recovery_time);
	};
	std::jthread thread(ThreadFunc, recovery_time);
	thread.detach();
}

void ShoutRecoveryHandler::AsyncCheck(float recovery_time) {
	float delay = 20.0f;
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
	const auto task_interface = SKSE::GetTaskInterface();

	if (task_interface != nullptr) {
		auto player = RE::PlayerCharacter::GetSingleton();
		if (player && player->GetActorRuntimeData().currentProcess &&
			player->GetActorRuntimeData().currentProcess->InHighProcess()) {
			task_interface->AddTask([this, recovery_time]() -> void {
				GetPlayerData()->voiceRecoveryTime = recovery_time;
				logger::info("Set: {}", recovery_time);
				//SetShoutCooldown(nullptr);
			});
		}
	}
}

void ShoutRecoveryHandler::UpdateStorage() {
	shout_storage.clear();
	if (shout_cooldowns.size() > 0) {
		for (auto& element : shout_cooldowns) {
			auto shout_id = element.first->formID;
			shout_storage.push_back(std::to_string(shout_id));
			shout_storage.push_back(std::to_string(element.second));
			shout_storage.push_back(std::to_string(shout_times[element.first]));
		}
	}
}

void ShoutRecoveryHandler::LoadStorage()
{
	Revert();
	RE::TESShout* current_shout = nullptr;
	int current_idx = 0;
	for (auto& item : shout_storage.items()) {
		std::string storage_key = item.value().get<std::string>();
		if (current_idx == 0) {
			current_shout = RE::TESForm::LookupByID(static_cast<RE::FormID>(std::stoul(storage_key)))->As<RE::TESShout>();
		} else if (current_idx == 1) {
			shout_cooldowns[current_shout] = std::stof(storage_key);
		} else if (current_idx == 2) {
			shout_times[current_shout] = std::stof(storage_key);
		}
		current_idx = (current_idx + 1) % 3;
	}
}

void ShoutRecoveryHandler::LogCooldowns() {
	if (shout_cooldowns.size() > 0) {
		for (auto& element : shout_cooldowns) {
			logger::info("{}, Recovery = {}, Time = {}", element.first->fullName.data(), element.second, shout_times[element.first]);
		}
	} else {
		logger::info("No shouts found");
	}
}

void ShoutRecoveryHandler::LogStorage() {
	for (auto& item : shout_storage.items()) {
		std::string storage_key = item.value().get<std::string>();
		logger::info("{}", storage_key);
	}
}

bool ShoutRecoveryHandler::ExportStorage(SKSE::SerializationInterface* a_intfc)
{
	if (!Serialization::Save(a_intfc, shout_storage)) {
		logger::error("Failed to write storage");
		return false;
	}
	return true;
}

bool ShoutRecoveryHandler::SerializeSave(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version)
{
	if (!a_intfc->OpenRecord(a_type, a_version)) {
		logger::error("Failed to open records!");
		return false;
	}
	return SerializeSave(a_intfc);
}

bool ShoutRecoveryHandler::SerializeSave(SKSE::SerializationInterface* a_intfc)
{
	UpdateStorage();
	//LogStorage();
	return ExportStorage(a_intfc);
}

bool ShoutRecoveryHandler::DeserializeLoad(SKSE::SerializationInterface* a_intfc)
{
	shout_storage.clear();
	if (!Serialization::Load(a_intfc, shout_storage)) {
		logger::info("Failed to load power switches!");
		return false;
	}
	LoadStorage();
	//LogCooldowns();
	return true;
}

void ShoutRecoveryHandler::Revert() {
	shout_cooldowns.clear();
	shout_times.clear();
}