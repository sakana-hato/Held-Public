#include "DxLib.h"
#include "Precompiled.h"
#include "ResourceManager.h"
#include "EffekseerForDXLib.h"

using json = nlohmann::json;

//jsonの中身をmapに入れる作業
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

	if (json.contains("fonts"))
	{
		for (auto& item : json["fonts"].items())
		{
			const std::string& id		= item.key();
			const auto& fontDef			= item.value();
			const std::string fontPath	= fontDef.value("path", "");
			const std::string fontName	= fontDef.value("name", "");

			//otf をシステムに登録
			if (!fontPath.empty())
			{
				AddFontResourceEx(fontPath.c_str(), FR_PRIVATE, nullptr);
				fontPaths.push_back(fontPath);
			}

			//フォント名を保持（使う側が取得する）
			fontNames[id] = fontName;
		}
	}

	if (json.contains("sounds3d"))
	{
		SetCreate3DSoundFlag(TRUE);   

		for (auto& item : json["sounds3d"].items())
		{
			const std::string& id	= item.key();
			const std::string  path = item.value().get<std::string>();
			sounds3d[id]			= LoadSoundMem(path.c_str());
		}

		SetCreate3DSoundFlag(FALSE); 
	}
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

	for (const auto& path : fontPaths)
	{
		RemoveFontResourceEx(path.c_str(), FR_PRIVATE, nullptr);
	}

	for (auto& kv : sounds3d)
	{
		if (kv.second >= 0)
		{
			DeleteSoundMem(kv.second);
		}
	}
	
	models.clear();
	images.clear();
	sounds.clear();
	sounds3d.clear();
	effects.clear();
	fontPaths.clear();
	fontNames.clear();
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

int ResourceManager::Sound3D(const std::string& id) const
{
	auto it = sounds3d.find(id);
	return (it != sounds3d.end()) ? it->second : -1;
}

int ResourceManager::Effect(const std::string& id) const
{
	auto it = effects.find(id);
	return (it != effects.end()) ? it->second : -1;
}

std::string ResourceManager::FontName(const std::string& id) const
{
	auto it = fontNames.find(id);
	return (it != fontNames.end()) ? it->second : "";
}