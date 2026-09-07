#include "DxLib.h"
#include "Precompiled.h"
#include "ResourceManager.h"
#include "EffekseerForDXLib.h"

using json = nlohmann::json;

//json‚Ì’†g‚ğmap‚É“ü‚ê‚éì‹Æ
namespace
{
	template <class Loader>
	void LoadSection(const json& j, const char* section,
		std::unordered_map<std::string, int>& out, Loader loader)
	{
		if (!j.contains(section)) return;

		for (auto& item : j[section].items())
		{
			const std::string& id = item.key();
			const std::string  path = item.value().get<std::string>();
			out[id] = loader(path.c_str());
		}
	}
}

bool ResourceManager::LoadJson(const char* jsonPath)
{
	std::ifstream ifs(jsonPath);
	if (!ifs)
	{
		return false;
	}

	json json;
	try
	{
		ifs >>json ;
	}
	catch (...)
	{
		return false; 
	}

	LoadSection(json, "models", models, [](const char* path) {return MV1LoadModel(path); });
	LoadSection(json, "images", images, [](const char* path) {return LoadGraph(path); });
	LoadSection(json, "sounds", sounds, [](const char* path) {return LoadSoundMem(path); });
	LoadSection(json, "effects", effects, [](const char* path) {return LoadEffekseerEffect(path); });

	return true;
}

void ResourceManager::UnloadAll()
{
	for (auto& kv : models)
	{
		if (kv.second >= 0)
		{
			MV1DeleteModel(kv.second);
		}
	}

	for (auto& kv : images)
	{
		if (kv.second >= 0)
		{
			DeleteGraph(kv.second);
		}
	}

	for (auto& kv : sounds)
	{
		if (kv.second >= 0)
		{
			DeleteSoundMem(kv.second);
		}
	}

	for (auto& kv : effects)
	{
		DeleteEffekseerEffect(kv.second);
	}

	models.clear();
	images.clear();
	sounds.clear();
	effects.clear();
}

int ResourceManager::Model(const std::string& id)const
{
	auto it = models.find(id);
	return (it != models.end()) ? it->second : -1;
}

int ResourceManager::Image(const std::string& id)const
{
	auto it = images.find(id);
	return (it!=images.end())? it->second : -1;
}

int ResourceManager::Sound(const std::string& id)const
{
	auto it = sounds.find(id);
	return (it != sounds.end()) ? it->second : -1;
}

int ResourceManager::Effect(const std::string& id) const
{
	auto it = effects.find(id);
	return (it != effects.end()) ? it->second : -1;
}