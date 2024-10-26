#include "Serialization.h"
#include "ShoutRecoveryHandler.h"

namespace Serialization
{
	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		auto handler = ShoutRecoveryHandler::GetSingleton();

		handler->storage_mtx.lock();

		if (!handler->SerializeSave(a_intfc, Serialization::kUniqueID, Serialization::kSerializationVersion)) {
			logger::error("Failed to save data!\n");
		}

		handler->storage_mtx.unlock();
	}

	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		auto handler = ShoutRecoveryHandler::GetSingleton();
		handler->storage_mtx.lock();

		uint32_t type;
		uint32_t version;
		uint32_t length;
		bool loaded = false;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != Serialization::kSerializationVersion) {
				logger::error("Loaded data is out of date!");
				continue;
			}

			switch (type) {
			case Serialization::kUniqueID:
				if (!handler->DeserializeLoad(a_intfc))
					logger::info("Failed to load data!\n");
				else
					loaded = true;
				break;
			default:
				logger::error(FMT_STRING("Unrecognized signature type! {}"), type);
				break;
			}
		}
		handler->storage_mtx.unlock();
	}

	bool Save(SKSE::SerializationInterface* a_intfc, json& root)
	{
		std::string elem = root.dump();
		std::size_t size = elem.length();

		if (!a_intfc->WriteRecordData(size)) {
			logger::error("Failed to write size of record data!");
			return false;
		} else {
			if (!a_intfc->WriteRecordData(elem.data(), static_cast<uint32_t>(elem.length()))) {
				logger::error("Failed to write element!");
				return false;
			}
			logger::debug(FMT_STRING("Serialized {}"), elem);
		}
		return true;
	}
	
	bool LoadForm(SKSE::SerializationInterface* a_intfc, json& parsedJson, std::string& oldFormIDs)
	{
		RE::FormID oldFormID = static_cast<RE::FormID>(std::stoul(oldFormIDs));
		RE::FormID newFormID = 0;
		if (oldFormID != 0 && a_intfc->ResolveFormID(oldFormID, newFormID)) {
			std::string newFormIDs = std::to_string(newFormID);
			parsedJson.push_back(newFormIDs);
			return true;
		} else {
			logger::debug(FMT_STRING("Discarded removed form {:X}"), oldFormID);
			return false;
		}
	}

	bool Load(SKSE::SerializationInterface* a_intfc, json& parsedJson)
	{
		//logger::info("Got to full load");
		std::size_t size;
		if (!a_intfc->ReadRecordData(size)) {
			logger::error("Failed to load size!");
			return false;
		}

		std::string elem;
		elem.resize(size);

		if (!a_intfc->ReadRecordData(elem.data(), static_cast<uint32_t>(size))) {
			logger::error("Failed to load element!");
			return false;
		} else {
			logger::debug(FMT_STRING("Deserialized {}"), elem);
			json temporaryJson = json::parse(elem);
			bool previous_success = false;
			int current_idx = 0;
			for (auto& el : temporaryJson.items()) {
				std::string storage_key = el.value().get<std::string>();
				try {
					if (current_idx == 0) {
						previous_success = LoadForm(a_intfc, parsedJson, storage_key);
					} else if (previous_success) {
						parsedJson.push_back(storage_key);
					}
				} catch (std::invalid_argument const&) {
					logger::error("Bad input: std::invalid_argument thrown");
					previous_success = false;
				} catch (std::out_of_range const&) {
					logger::error("Integer overflow: std::out_of_range thrown");
					previous_success = false;
				}
				current_idx = (current_idx + 1) % 3;
			}
		}
		return true;
	}


	void RevertCallback(SKSE::SerializationInterface*)
	{
		auto handler = ShoutRecoveryHandler::GetSingleton();
		handler->Revert();
	}
}