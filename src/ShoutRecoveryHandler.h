#pragma once

#include <shared_mutex>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class ShoutRecoveryHandler
{
public:
	std::map<RE::TESShout*, float> shout_cooldowns;
	std::map<RE::TESShout*, float> shout_times;

	
	json shout_storage;
	std::shared_mutex storage_mtx;

	static ShoutRecoveryHandler* GetSingleton()
	{
		static ShoutRecoveryHandler singleton;
		return &singleton;
	}

	float GetCurrentGameTimeFromCalendar();

	RE::HighProcessData* GetPlayerData();

	void SetShoutCooldown(RE::TESShout* shout);
	
	void AsyncRun(RE::TESShout* shout);
	void AsyncCatch(RE::TESShout* shout);

	void AsyncWalk(float recovery_time);
	void AsyncCheck(float recovery_time);

	void LogCooldowns();
	void LogStorage();

	void UpdateStorage();
	void LoadStorage();
	bool ExportStorage(SKSE::SerializationInterface* a_intfc);
	bool SerializeSave(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version);
	bool SerializeSave(SKSE::SerializationInterface* a_intfc);
	bool DeserializeLoad(SKSE::SerializationInterface* a_intfc);
	void Revert();

protected:
	ShoutRecoveryHandler() = default;
	ShoutRecoveryHandler(const ShoutRecoveryHandler&) = delete;
	ShoutRecoveryHandler(ShoutRecoveryHandler&&) = delete;
	virtual ~ShoutRecoveryHandler() = default;

	auto operator=(const ShoutRecoveryHandler&) -> ShoutRecoveryHandler& = delete;
	auto operator=(ShoutRecoveryHandler&&) -> ShoutRecoveryHandler& = delete;
};