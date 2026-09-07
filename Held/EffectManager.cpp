#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include "EffectManager.h"
#include "Config.h"

EffectManager& EffectManager::Instance()
{
	static EffectManager inst;
	return inst;
}

void EffectManager::Init(int maxParticles)
{
	//二重初期化を防ぐ
	if (initialized)
	{
		return;
	}

	//Effekseerの初期化
	if (Effekseer_Init(maxParticles) == -1)
	{
		return;
	}

	//デバイスロスト時のコールバックを設定
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	//Zバッファを有効にして、モデルとの前後関係を正しく描く
	SetUseZBuffer3D		(TRUE);
	SetWriteZBuffer3D	(TRUE);

	initialized = true;
}

void EffectManager::End()
{
	if (!initialized)
	{
		return;
	}

	StopAll();
	Effkseer_End();
	initialized = false;
}

int EffectManager::Play(int effectHandle, const VECTOR& pos, float scale, const VECTOR& rotRad)
{
	//安全措置
	if (!initialized || effectHandle < 0)
	{
		return -1;
	}

	const int instance = PlayEffekseer3DEffect(effectHandle);// 再生インスタンス
	if (instance < 0)
	{
		return -1;
	}

	SetPosPlayingEffekseer3DEffect		(instance, pos.x, pos.y, pos.z);
	SetScalePlayingEffekseer3DEffect	(instance, scale, scale, scale);
	SetRotationPlayingEffekseer3DEffect	(instance, rotRad.x, rotRad.y, rotRad.z);

	activeInstances.push_back(instance); // StopAll()で一括停止できるよう控えておく
	return instance;
}

void EffectManager::SetPosition(int instance, const VECTOR& pos)
{
	//安全措置
	if (instance < 0)
	{
		return;
	}

	SetPosPlayingEffekseer3DEffect(instance, pos.x, pos.y, pos.z);
}

void EffectManager::SetRotation(int instance, const VECTOR& rotRad)
{
	//安全措置
	if (instance < 0)
	{
		return;
	}

	SetRotationPlayingEffekseer3DEffect(instance, rotRad.x, rotRad.y, rotRad.z);
}

void EffectManager::SetScale(int instance, float scale)
{
	//安全措置
	if (instance < 0)
	{
		return;
	}

	SetScalePlayingEffekseer3DEffect(instance, scale, scale, scale);
}

void EffectManager::SetSpeed(int instance, float speed)
{
	//安全措置
	if (instance < 0)
	{
		return;
	}

	SetSpeedPlayingEffekseer3DEffect(instance, speed);
}

void EffectManager::Stop(int instance)
{
	//安全措置
	if (instance < 0)
	{
		return;
	}

	StopEffekseer3DEffect(instance);
}

void EffectManager::StopAll()
{
	//安全措置
	if (!initialized)
	{
		return;
	}

	for (int instance : activeInstances)
	{
		StopEffekseer3DEffect(instance);
	}
	activeInstances.clear();
}

bool EffectManager::IsPlaying(int instance) const
{
	//安全措置
	if (instance < 0)
	{
		return false;
	}

	return IsEffekseer3DEffectPlaying(instance) == 0;
}

void EffectManager::Update()
{
	//安全措置
	if (!initialized)
	{
		return;
	}

	UpdateEffekseer3D();

	//再生が終わったインスタンスをリストから外す
	activeInstances.erase(std::remove_if(activeInstances.begin(), activeInstances.end(),[](int instance) { return IsEffekseer3DEffectPlaying(instance) != 0; }),activeInstances.end());
}

void EffectManager::Draw()const
{
	//安全措置
	if (!initialized)
	{
		return;
	}

	Effekseer_Sync3DSetting();
	DrawEffekseer3D();
}