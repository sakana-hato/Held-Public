#include "DxLib.h"
#include "ResourceManager.h"
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

	case SeId::Dodge: 
		return "se_dodge";

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
	//“¯‚¶BGM‚ª—¬‚ê‚È‚¢‚æ‚¤‚É
	if (id == currentBgmId && currentBgmHandle >= 0&&CheckSoundMem(currentBgmHandle)==1)
	{
		return;
	}

	StopBgm();

	const int handle = ResourceManager::Instance().Sound(BgmKey(id));//“o˜^Ï‚İ‚ÌƒTƒEƒ“ƒhƒnƒ“ƒhƒ‹

	//–¢“o˜^‚ÍÄ¶‚µ‚È‚¢
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

void SoundManager::ChangeBgm(BgmId id)
{
	PlayBgm(id);
}

void SoundManager::PlaySe(SeId id)
{
	const int handle = ResourceManager::Instance().Sound(SeKey(id));
	if (handle < 0)
	{
		return;
	}

	//SE ‚Íd‚Ë‚Ä–Â‚ç‚·
	PlaySoundMem(handle, DX_PLAYTYPE_BACK, TRUE);
}

void SoundManager::SetBgmVolume(float v)
{
	bgmVolume = (v < 0.0f) ? 0.0f : (v > 1.0f ? 1.0f : v);

	//Ä¶’†‚Ì BGM ‚É‚à‘¦”½‰f
	if (currentBgmHandle >= 0)
	{
		ChangeVolumeSoundMem(ToDxVolume(bgmVolume), currentBgmHandle);
	}
}

void SoundManager::SetSeVolume(float v)
{
	seVolume = (v < 0.0f) ? 0.0f : (v > 1.0f ? 1.0f : v);
	ApplySeVolume();
}

void SoundManager::Reset()
{
	StopBgm();
	ApplySeVolume();
}