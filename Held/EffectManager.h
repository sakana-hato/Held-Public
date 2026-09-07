#pragma once
#include "Config.h"

/// <summary>
/// Effekseer エフェクトの再生を管理するシングルトン。
/// </summary>
class EffectManager
{
public:
	/// <summary>
	/// シングルトンインスタンスを取得する
	/// </summary>
	static EffectManager& Instance();

	/// <summary>
	/// Effekseer を初期化する
	/// </summary>
	/// <param name="maxParticles"></param>同時に扱えるパーティクルの上限
	void Init(int maxParticles = Config::Effect::MAX_PARTICLES);

	/// <summary>
	/// Effekseer を終了する
	/// </summary>
	void End();

	/// <summary>
	/// エフェクトをワールド座標で再生する。
	/// </summary>
	/// <param name="effectHandle">	</param>エフェクトのハンドル
	/// <param name="pos">			</param>再生位置
	/// <param name="scale">		</param>大きさ
	/// <param name="rotRad">		</param>XYZ回転（ラジアン）
	int Play(int effectHandle, const VECTOR& pos, float scale = 1.0f, const VECTOR& rotRad = { 0.0f, 0.0f, 0.0f });

	/// <summary>
	/// 再生中インスタンスの座標を更新する
	/// </summary>
	void SetPosition(int instance, const VECTOR& pos);

	/// <summary>
	/// 再生中インスタンスの回転を更新する
	/// </summary>
	void SetRotation(int instance, const VECTOR& rotRad);

	/// <summary>
	/// 再生中インスタンスの大きさを更新する
	/// </summary>
	void SetScale(int instance, float scale);

	/// <summary>
	/// エフェクト再生速度を変更する
	/// </summary>
	/// <param name="instance">	</param>エフェクト
	/// <param name="speed">	</param>速度
	void SetSpeed(int instance, float speed);

	/// <summary>
	/// 指定インスタンスを停止する
	/// </summary>
	void Stop(int instance);

	/// <summary>
	/// 再生中の全インスタンスを停止する
	/// </summary>
	void StopAll();

	/// <summary>
	/// インスタンスがまだ再生中かどうか
	/// </summary>
	bool IsPlaying(int instance) const;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()const;

private:
	EffectManager()									= default;
	EffectManager(const EffectManager&)				= delete;
	EffectManager& operator=(const EffectManager&)	= delete;

	std::vector<int> activeInstances;	// 再生中インスタンスの一覧
	bool initialized = false;			// Init()済みかどうか
};