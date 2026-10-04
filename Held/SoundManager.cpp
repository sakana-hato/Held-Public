#include "DxLib.h"
#include "ResourceManager.h"
#include "Config.h"
#include "SoundManager.h"

const char* SoundManager::BgmKey(BgmId id)const
{
	switch (id)
	{
	case BgmId::Normal:
		return "bgm_normal";
	case BgmId::Battle:
		return "bgm_battle";
	case BgmId::Boss:
		return "bgm_boss";
	case BgmId::Cave:
		return "bgm_cave";
	case BgmId::Title:
		return "bgm_title";
	default:            
		return "";
	}
}

const char* SoundManager::SeKey(SeId id)const
{
	switch (id)
	{
	case SeId::Attack: 
		return "se_attack";
	case SeId::Attack_hit:
		return "se_attack_hit";
	case SeId::Dodge: 
		return "se_dodge";
	case SeId::DrawKatana: 
		return "se_draw_katana";
	case SeId::SheatheKatana:
		return "se_sheathe_katana";
	case SeId::EnemyHit:
		return "se_enemy_hit";
	case SeId::FootstepWalk:
		return "se_walk";
	case SeId::FootstepRun:
		return "se_run";
	case SeId::DoorOpen:
		return "se_door_open";
	case SeId::DoorClose:
		return "se_door_close";
	case SeId::JustDodge:
		return "se_just_dodge";
	case SeId::JumpVoice1:
		return "se_jump_voice1";
	case SeId::JumpVoice2:
		return "se_jump_voice2";
	case SeId::JumpVoice3:
		return "se_jump_voice3";
	case SeId::CounterRush:
		return "se_counter_rush";
	case SeId::CounterFinish:
		return "se_counter_finish";
	case SeId::JumpAttackLand:
		return "se_jump_attack_land";
	case SeId::TargetLock:
		return "se_target_lock";
	case SeId::TargetUnlock:
		return "se_target_unlock";
	case SeId::BossRoar:
		return "se_boss_roar";
	case SeId::BossJumpLand:
		return "se_boss_jump_land";
	case SeId::BossPunch:
		return "se_boss_punch";
	case SeId::BossSword:
		return "se_boss_sword";
	case SeId::BossCharge:
		return "se_boss_charge";
	case SeId::TitleVoice:
		return "se_title_voice";
	default:           
		return "";
	}
}

int SoundManager::ToDxVolume(float vol)const
{
	if (vol < 0.0f)
	{
		vol = 0.0f;
	}
	if (vol > 1.0f)
	{
		vol = 1.0f;
	}
	return static_cast<int>(vol * 255.0f);
}

void SoundManager::ApplySeVolume()const
{
	const int dxVol = ToDxVolume(seVolume);

	for (int i = 0; i < static_cast<int>(SeId::Count); ++i)
	{
		const int handle = ResourceManager::Instance().Sound(SeKey(static_cast<SeId>(i)));
		if (handle >= 0)
		{
			ChangeVolumeSoundMem(dxVol, handle);
		}
	}
}

void SoundManager::PlayBgm(BgmId id)
{
	//同じBGMが流れないように
	if (id == currentBgmId && currentBgmHandle >= 0&&CheckSoundMem(currentBgmHandle)==1)
	{
		return;
	}

	StopBgm();

	const int handle = ResourceManager::Instance().Sound(BgmKey(id));//登録済みのサウンドハンドル

	//未登録は再生しない
	if (handle < 0)
	{
		return;
	}

	ChangeVolumeSoundMem(ToDxVolume(bgmVolume), handle);
	PlaySoundMem(handle, DX_PLAYTYPE_LOOP,TRUE);

	currentBgmHandle = handle;
	currentBgmId = id;
}

void SoundManager::StopBgm()
{
	if (currentBgmHandle >= 0)
	{
		StopSoundMem(currentBgmHandle);
	}

	currentBgmHandle = -1;
	currentBgmId = BgmId::Count;
}

void SoundManager::PlaySeLoop(SeId id,float volumeScale)
{
	const int handle = ResourceManager::Instance().Sound(SeKey(id));
	if (handle < 0)
	{
		return;
	}

	//既に鳴っているなら何もしない
	if (CheckSoundMem(handle) == 1)
	{
		return;
	}

	ChangeVolumeSoundMem(ToDxVolume(seVolume * volumeScale), handle);
	PlaySoundMem(handle, DX_PLAYTYPE_LOOP, TRUE);
}

void SoundManager::StopSeLoop(SeId id)
{
	const int handle = ResourceManager::Instance().Sound(SeKey(id));
	if (handle < 0)
	{
		return;
	}
	StopSoundMem(handle);
}

void SoundManager::PlaySe3D(int handle, const VECTOR& pos, float radius)
{
	if (handle < 0)
	{
		return;
	}

	//音源の位置と聞こえる範囲を設定
	Set3DPositionSoundMem(pos, handle);
	Set3DRadiusSoundMem(radius, handle);

	//音量（SE音量を反映）
	ChangeVolumeSoundMem(ToDxVolume(seVolume), handle);

	//再生
	PlaySoundMem(handle, DX_PLAYTYPE_BACK, TRUE);
}

void SoundManager::Set3DListener(const VECTOR& listenerPos, const VECTOR& frontPos)
{
	//リスナーの位置と向きを設定（上方向はY軸）
	Set3DSoundListenerPosAndFrontPos_UpVecY(listenerPos, frontPos);
}

bool SoundManager::IsSeLoopPlaying(SeId id) const
{
	const int handle = ResourceManager::Instance().Sound(SeKey(id));
	if (handle < 0)
	{
		return false;
	}
	return (CheckSoundMem(handle) == 1);
}

void SoundManager::FadeOutBgm()
{
	if (currentBgmHandle < 0)
	{
		return;
	}

	if (prevBgmHandle >= 0)
	{
		StopSoundMem(prevBgmHandle);
	}

	//今のBGMをフェードアウト対象にする
	prevBgmHandle		= currentBgmHandle;
	currentBgmHandle	= -1;                //次に鳴らすBGMは無し
	currentBgmId		= BgmId::Count;

	//フェード開始
	fadeTimer	= 0.0f;
	fading		= true;
}

void SoundManager::ChangeBgm(BgmId id, float volumeScale)
{
	if (id == currentBgmId && currentBgmHandle >= 0 && CheckSoundMem(currentBgmHandle) == 1)
	{
		return;
	}

	const int nextHandle = ResourceManager::Instance().Sound(BgmKey(id));
	if (nextHandle < 0)
	{
		return;
	}

	bgmVolumeScale = volumeScale;

	if (currentBgmHandle >= 0)
	{
		if (prevBgmHandle>=0)
		{
			StopSoundMem(prevBgmHandle);
		}
		prevBgmHandle = currentBgmHandle;
	}

	//次のBGMを音量0から再生開始
	ChangeVolumeSoundMem(0, nextHandle);
	PlaySoundMem(nextHandle, DX_PLAYTYPE_LOOP, TRUE);

	currentBgmHandle	= nextHandle;
	currentBgmId		= id;

	//フェード開始
	fadeTimer	= 0.0f;
	fading		= true;
}

void SoundManager::Update(float dt)
{
	if (!fading)
	{
		return;
	}

	fadeTimer += dt;
	const float t = fadeTimer / Config::Sound::BGM_FADE_TIME;
	const float clamped = (t > 1.0f) ? 1.0f : t;

	if (currentBgmHandle >= 0)
	{
		ChangeVolumeSoundMem(ToDxVolume(bgmVolume * clamped * bgmDuckScale * bgmVolumeScale), currentBgmHandle);
	}

	if (prevBgmHandle >= 0)
	{
		ChangeVolumeSoundMem(ToDxVolume(bgmVolume * (1.0f - clamped) * bgmDuckScale * bgmVolumeScale), prevBgmHandle);
	}

	if (clamped >= 1.0f)
	{
		if (prevBgmHandle >= 0)
		{
			StopSoundMem(prevBgmHandle);
			prevBgmHandle = -1;
		}
		fading = false;
	}
}

void SoundManager::PlaySe(SeId id)
{
	const int handle = ResourceManager::Instance().Sound(SeKey(id));
	if (handle < 0)
	{
		return;
	}

	//SE は重ねて鳴らす
	PlaySoundMem(handle, DX_PLAYTYPE_BACK, TRUE);
}

void SoundManager::PlaySeRandom(SeId first, int count)
{
	if (count <= 0)
	{
		return;
	}

	const int offset	= GetRand(count - 1);
	const SeId id		= static_cast<SeId>(static_cast<int>(first) + offset);
	PlaySe(id);
}

void SoundManager::DuckBgm(float scale)
{
	bgmDuckScale = (scale < 0.0f) ? 0.0f : (scale > 1.0f ? 1.0f : scale);

	if (currentBgmHandle >= 0)
	{
		ChangeVolumeSoundMem(ToDxVolume(bgmVolume * bgmDuckScale * bgmVolumeScale), currentBgmHandle);
	}
}

void SoundManager::UnduckBgm()
{
	bgmDuckScale = 1.0f;

	if (currentBgmHandle >= 0)
	{
		ChangeVolumeSoundMem(ToDxVolume(bgmVolume * bgmVolumeScale), currentBgmHandle);
	}
}

void SoundManager::SetBgmVolume(float v)
{
	bgmVolume = (v < 0.0f) ? 0.0f : (v > 1.0f ? 1.0f : v);

	if (currentBgmHandle >= 0)
	{
		ChangeVolumeSoundMem(ToDxVolume(bgmVolume * bgmDuckScale * bgmVolumeScale), currentBgmHandle);
	}
}

void SoundManager::Reset()
{
	StopBgm();
	ApplySeVolume();
}