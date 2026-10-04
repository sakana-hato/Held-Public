#include "DxLib.h"
#include "Precompiled.h"
#include "TitleDirector.h"
#include "CameraSystem.h"
#include "SoundManager.h"
#include "Player.h"
#include "Config.h"

void TitleDirector::Start()
{
	Begin();          //基底：開始状態にする

	timer = 0.0f;
	logoVisible = false;
	swordDrawn = false;
	voicePlayed = false;

	//プレイヤーを廊下のスタート地点に置く
	VECTOR startPos = VGet(Config::Title::PLAYER_START_X,Config::Graund::GROUND_Y,Config::Title::PLAYER_START_Z);
	player.SetPosition(startPos);
	//player.PlayCutsceneWalk();   //歩きアニメ

	camera.SetMode(CameraMode::Cutscene);

	SoundManager::Instance().ChangeBgm(BgmId::Cave, Config::Sound::BGM_CAVE_SCALE);

	EnterPhase(Phase::Stand);
}

void TitleDirector::SkipToEnd()
{
	Begin();


	StopFootstep();

	//扉の前に立たせて、抜刀済みにする
	VECTOR doorPos = VGet(Config::Title::PLAYER_START_X,Config::Graund::GROUND_Y,Config::Title::DOOR_STAND_Z);
	player.SetPosition(doorPos);
	player.PlayCutsceneIdle();

	SoundManager::Instance().ChangeBgm(BgmId::Title);

	if (!player.IsKatanaDrawn())
	{
		player.ToggleKatanaDraw();
	}

	camera.SetMode(CameraMode::Cutscene);
	SetDoorCamera();

	logoVisible = true;
	swordDrawn = true;

	Finish();   //演出は完了扱い
}

void TitleDirector::EnterPhase(Phase next)
{
	phase = next;
	timer = 0.0f;

	switch (next)
	{
	case TitleDirector::Phase::Stand:
	{
		//立ち止まって待機し、セリフを言う
		player.PlayCutsceneIdle();
		//SoundManager::Instance().PlaySe(SeId::TitleVoice);
		break;
	}
	case TitleDirector::Phase::Walking:
	{
		//歩き始める
		player.PlayCutsceneWalk();
		break;
	}
	case TitleDirector::Phase::ArriveDoor:
	{
		//扉の前で立ち止まる
		player.PlayCutsceneIdle();
		SetDoorCamera();
		break;
	}
	case TitleDirector::Phase::DrawSword:
	{
		//抜刀する
		if (!swordDrawn)
		{
			player.ToggleKatanaDraw();
			swordDrawn = true;
		}
		break;
	}
	default:
		break;
	}
}

void TitleDirector::UpdateWalkCamera(float progress)
{
	using namespace Config::Title;

	//progress: 0.0(歩き始め)   1.0(扉前)
	//カメラ位置を、右斜め下から右へ、補間で移す
	const float t = (progress > 1.0f) ? 1.0f : progress;

	//イーズアウトで、最初の動きを大きく、終わりを緩やかに
	const float eased = 1.0f - (1.0f - t) * (1.0f - t);

	//プレイヤーの現在位置を基準にする
	const VECTOR playerPos = player.GetPosition();

	//カメラのオフセット（右斜め下 → 右）
	const float offX = CAM_START_OFFSET_X + (CAM_END_OFFSET_X - CAM_START_OFFSET_X) * eased;
	const float offY = CAM_START_OFFSET_Y + (CAM_END_OFFSET_Y - CAM_START_OFFSET_Y) * eased;
	const float offZ = CAM_START_OFFSET_Z + (CAM_END_OFFSET_Z - CAM_START_OFFSET_Z) * eased;

	const VECTOR camPos = VGet(playerPos.x + offX,playerPos.y + offY,playerPos.z + offZ);

	//注視点はプレイヤーの少し上
	const VECTOR camTgt = VGet(playerPos.x,playerPos.y + CAM_LOOK_HEIGHT,playerPos.z);

	camera.SetCutsceneCamera(camPos, camTgt);
}

void TitleDirector::SetDoorCamera()
{
	using namespace Config::Title;

	//扉前のカメラ（地面近くから見上げる構図）
	const VECTOR camPos = VGet(DOOR_CAM_POS_X, DOOR_CAM_POS_Y, DOOR_CAM_POS_Z);
	const VECTOR camTgt = VGet(DOOR_CAM_TGT_X, DOOR_CAM_TGT_Y, DOOR_CAM_TGT_Z);

	camera.SetCutsceneCamera(camPos, camTgt);
}

void TitleDirector::Update(float dt)
{
	if (!IsPlaying())
	{
		return;
	}

	timer += dt;

	switch (phase)
	{
	case Phase::Stand:
	{
		//カメラは開始位置のまま
		UpdateWalkCamera(0.0f);

		if (!voicePlayed && timer >= Config::Title::VOICE_DELAY)
		{
			SoundManager::Instance().PlaySe(SeId::TitleVoice);
			voicePlayed = true;
		}

		//一定時間たったら歩き始める
		if (timer >= Config::Title::STANDBY_TIME)
		{
			EnterPhase(Phase::Walking);
		}
		break;
	}
	case Phase::Walking:
	{
		//扉に向かって歩く
		VECTOR pos = player.GetPosition();
		pos.z -= Config::Title::WALK_SPEED * dt;
		player.SetPosition(pos);

		if (!footstepPlaying)
		{
			SoundManager::Instance().PlaySeLoop(SeId::FootstepWalk, Config::Sound::FOOTSTEP_VOLUME);
			footstepPlaying = true;
		}

		//歩いた割合でカメラを補間
		const float total = Config::Title::PLAYER_START_Z - Config::Title::DOOR_STAND_Z;
		const float walked = Config::Title::PLAYER_START_Z - pos.z;
		const float progress = (total > 0.0f) ? (walked / total) : 1.0f;
		
		UpdateWalkCamera(progress);

		//扉前に着いたら次へ
		if (pos.z <= Config::Title::DOOR_STAND_Z)
		{
			pos.z = Config::Title::DOOR_STAND_Z;
			player.SetPosition(pos);
			StopFootstep();
			EnterPhase(Phase::ArriveDoor);
		}
		break;
	}
	case Phase::ArriveDoor:
	{
		//少し間を置いてから抜刀へ
		if (timer >= Config::Title::ARRIVE_WAIT_TIME)
		{
			EnterPhase(Phase::DrawSword);
		}
		break;
	}
	case Phase::DrawSword:
	{
		//抜刀アニメが終わるのを待って、ロゴを出す
		if (timer >= Config::Title::DRAW_WAIT_TIME)
		{
			logoVisible = true;

			SoundManager::Instance().ChangeBgm(BgmId::Title);

			Finish();   //基底：完了
		}
		break;
	}
	default:
		break;
	}
}

void TitleDirector::StopFootstep()
{
	if (footstepPlaying)
	{
		SoundManager::Instance().StopSeLoop(SeId::FootstepWalk);
		footstepPlaying = false;
	}
}