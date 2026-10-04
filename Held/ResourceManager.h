#pragma once
#include "Precompiled.h"

/// <summary>
/// 素材を読み込むリソースマネージャー(json)
/// </summary>
class ResourceManager
{
public:
	//シングルトン
	static ResourceManager& Instance()
	{
		static ResourceManager inst;
		return inst;
	}

	/// <summary>
	/// jsonの中身のリソースを読み込む
	/// </summary>
	/// <param name="jsonPath"></param>使うjsonの名前
	bool LoadJson(const char* jsonPath);

	/// <summary>
	/// リソース全開放
	/// </summary>
	void UnloadAll();

	int Model(const std::string& id)const;	// 3Dモデル
	int Image(const std::string& id)const;	// 画像
	int Sound(const std::string& id)const;	// 音
	int Sound3D(const std::string& id)const;//3D音
	int Effect(const std::string& id) const;// エフェクト
	std::string FontName(const std::string& id) const;
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ResourceManager()	= default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ResourceManager()	= default;

	//コピー禁止
	ResourceManager(const ResourceManager&) = default;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&&) = delete;

	//データ管理
	std::unordered_map<std::string, int> models;
	std::unordered_map<std::string, int> images;
	std::unordered_map<std::string, int> sounds;
	std::unordered_map<std::string, int> sounds3d;
	std::unordered_map<std::string, int> effects;
	
	
	std::unordered_map<std::string, std::string> fontNames;
	std::vector<std::string> fontPaths;
};