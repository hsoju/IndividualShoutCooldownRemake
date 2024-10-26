#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Serialization
{
	enum : std::uint32_t
	{
		kSerializationVersion = 1,
		kUniqueID = 'ISCD'
	};

	void SaveCallback(SKSE::SerializationInterface* a_intfc);
	void LoadCallback(SKSE::SerializationInterface* a_intfc);
	bool Save(SKSE::SerializationInterface* a_intfc, json& root);
	bool LoadForm(SKSE::SerializationInterface* a_intfc, json& parsedJson, std::string& oldFormIDs);
	bool Load(SKSE::SerializationInterface* a_intfc, json& parsedJson);
	void RevertCallback(SKSE::SerializationInterface*);
}