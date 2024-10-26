#pragma once

#include <shared_mutex>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class SwitchManager
{
public:
	std::vector<RE::TESForm*> switch_powers;
	std::unordered_set<RE::TESForm*> switch_set;

	bool enable_shouts = true;
	bool enable_lesser_powers = true;
	bool enable_powers = true;

	std::map<RE::SpellItem*, int> recast_powers;

	json power_storage;
	std::shared_mutex storage_mtx;
	
	std::shared_mutex settings_mtx;

	static SwitchManager* GetSingleton()
	{
		static SwitchManager singleton;
		return &singleton;
	}

	void LogFavoritePowers();
	void AddFavoritePower(RE::TESForm* power, std::unordered_set<RE::TESForm*>& current_favorites);
	std::unordered_set<RE::TESForm*> GetCurrentFavoritedPowers(RE::BSTArray<RE::TESForm*>& favorite_powers);
	std::vector<RE::TESForm*> GetUnfavoritedPowers(std::unordered_set<RE::TESForm*>& current_favorites);
	void RemoveUnfavoritedPowers(std::vector<RE::TESForm*>& powers_to_remove);
	void ProcessFavoritePowers();

	bool HasSwitchPower(RE::TESForm* power);

	void AdvancePower(RE::TESForm*& chosen_power, RE::TESForm* current_power, int& increment);

	RE::TESForm* FindNextPower(RE::TESForm* power, int start_idx, int increment);
	RE::TESForm* FindNextPowerUsingIncrement(RE::TESForm* power, int start_idx, int increment);
	RE::TESForm* GetNextPower(RE::TESForm* power, int increment);

	void ImportSettings();

	void UpdateStorage();

	bool ExportStorage(SKSE::SerializationInterface* a_intfc, json& storage, std::string& error_message);
	bool LoadStorage(json& power_storage);

	bool SerializeSave(SKSE::SerializationInterface* a_intfc);
	bool SerializeSave(SKSE::SerializationInterface* a_intfc, uint32_t a_type, uint32_t a_version);
	bool DeserializeLoad(SKSE::SerializationInterface* a_intfc);
	void Revert();

protected:
	SwitchManager() = default;
	SwitchManager(const SwitchManager&) = delete;
	SwitchManager(SwitchManager&&) = delete;
	virtual ~SwitchManager() = default;

	auto operator=(const SwitchManager&) -> SwitchManager& = delete;
	auto operator=(SwitchManager&&) -> SwitchManager& = delete;
};
