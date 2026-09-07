#include "DxLib.h"
#include "CutsceneDirector.h"
#include "SharedContext.h"

#include "CameraSystem.h"
#include "InputSystem.h"
#include "Stage.h"

#include "CameraMode.h"
#include "Player.h"
#include "Fader.h"
#include "Config.h"
#include "EffectManager.h"
#include "ResourceManager.h"
#include "Boss.h"

void CutSceneDirector::Start()
{
	Begin();
	phase				= Phase::FadeOut;
	timer				= 0.0f;
	walkAnimStarted		= false;
	doorCloseStarted	= false;
	Fader::GetInstance().FadeOut(Config::Cutscene::FADE_SPEED);
}

void CutSceneDirector::EnterPhase(Phase next)
{
	phase = next;
	timer = 0.0f;

	switch (next)
	{
	case Phase::DoorOpening:
		player.PlayCutsceneIdle();   //扉が開く間はidle
		break;
	case Phase::PlayerWalking:
		player.PlayCutsceneWalk();   //歩いて入る
		break;
	case Phase::DoorClosing:
		player.PlayCutsceneIdle();   //扉が閉じる間はidle
		break;
	default:
		break;
	}
}

void CutSceneDirector::Update(float dt)
{
	if (!IsPlaying())
	{
		return;
	}

	//スキップ入力（最初の暗転中と、既にスキップ中は除く）
	if (phase != Phase::FadeOut &&phase != Phase::SkipFadeOut &&phase != Phase::SkipSwitch &&input.IsPressed(InputAction::Skip))
	{
		Fader::GetInstance().FadeOut(Config::Cutscene::FADE_SPEED);
		EnterPhase(Phase::SkipFadeOut);
		return;
	}

	timer += dt;

	switch (phase)
	{
	case Phase::FadeOut:
	{
		//カメラを見上げ位置に切り替えて扉を開き始める
		if (Fader::GetInstance().IsFinishFadeOut())
		{
			camera.SetMode(CameraMode::Cutscene);

			//プレイヤーを扉の前へ移動させる
			VECTOR fixedPos = VGet(Config::Cutscene::PLAYER_START_X,player.GetPosition().y,Config::Cutscene::PLAYER_START_Z);
			player.SetPosition(fixedPos);

			//地面近くから扉とプレイヤーを見上げる構図
			const VECTOR camPos = VGet(Config::Cutscene::CAM_POS_X,Config::Cutscene::CAM_POS_Y,Config::Cutscene::CAM_POS_Z);// 演出用のカメラ位置
			const VECTOR camTgt = VGet(Config::Cutscene::CAM_TGT_X,Config::Cutscene::CAM_TGT_Y,Config::Cutscene::CAM_TGT_Z);// 演出用の注視点
			camera.SetCutsceneCamera(camPos, camTgt);

			//扉を開く
			stage.PlayDoor();

			//明転
			Fader::GetInstance().FadeIn(Config::Cutscene::FADE_SPEED);

			EnterPhase(Phase::DoorOpening);
		}
		break;
	}
	case Phase::DoorOpening:
	{
		//扉が開ききったらプレイヤーを歩かせる
		if (stage.IsDoorFinished())
		{
			EnterPhase(Phase::PlayerWalking);
		}
		break;
	}
	case Phase::PlayerWalking:
	{
		//移動だけ
		VECTOR pos	 = player.GetPosition();		//現在のプレイヤー座標
		pos.z		-= Config::Cutscene::WALK_SPEED * dt;
		player.SetPosition(pos);

		//プレイヤーが特定の座標まで移動したら完了とみなす
		if (pos.z < Config::Cutscene::BOSS_ROOM_ENTER_Z)
		{	
			EnterPhase(Phase::DoorClosing);
		}
		break;
	}
	case Phase::DoorClosing:
	{
		//閉じる処理は一回だけ行う
		if (!doorCloseStarted)
		{
			stage.CloseDoor();
			doorCloseStarted = true;
			break;  
		}

		//扉が閉じきったら当たり判定を復活させて完了
		if (stage.IsDoorFinished())
		{
			stage.CloseDoorWall();

			camera.SetMode(CameraMode::Cutscene);

			const VECTOR camPos = VGet(Config::Boss::BOSS_CAM_POS_X,Config::Boss::BOSS_CAM_POS_Y,Config::Boss::BOSS_CAM_POS_Z);// ボス戦用のカメラ位置
			const VECTOR camTgt = VGet(Config::Boss::BOSS_CAM_TGT_X,Config::Boss::BOSS_CAM_TGT_Y,Config::Boss::BOSS_CAM_TGT_Z);// ボス戦用の注視点

			camera.SetCutsceneCamera(camPos, camTgt);

			{
				boss.Activate();
				boss.ChangeState(BossStateId::Intro);

				
				VECTOR bossPos = boss.Comp().pos;//ボスの足元に魔法陣を出す

				
				bossPos.y = boss.FloorYAt(bossPos);//地面の高さ

				bossPos.y = bossPos.y + 10;	//地面にめり込まないよう少し浮かせる 

				EffectManager::Instance().Play(ResourceManager::Instance().Effect("boss_appear"),bossPos,Config::Effect::BOSS_APPEAR_SCALE);
			}

			EnterPhase(Phase::BossRising);
		}
		break;
	}
	case Phase::BossRising:
	{
		if (boss.IsIntroFinished())
		{
			boss.PlayRoar();
			EnterPhase(Phase::BossRoar);
		}
		break;
	}
	case Phase::BossRoar:
	{
		//咆哮の指定区間だけカメラを揺らす
		if (timer >= Config::Cutscene::ROAR_SHAKE_START &&
			timer <= Config::Cutscene::ROAR_SHAKE_END)
		{
			camera.AddShake(Config::Cutscene::ROAR_SHAKE);
		}

		//咆哮が終わったら通常のカメラと操作に戻す
		if (timer >= Config::Boss::BOSS_ROAR_TIME)
		{
			camera.SetMode(CameraMode::ThirdPerson);

			boss.ChangeState(BossStateId::Idle);
		
			Finish();
		}
		break;
	}
	case Phase::SkipFadeOut:
	{
		//暗転しきったら、状態を切り替える
		if (Fader::GetInstance().IsFinishFadeOut())
		{
			SkipToEnd();  
			Fader::GetInstance().FadeIn(Config::Cutscene::FADE_SPEED);
			EnterPhase(Phase::SkipSwitch);
		}
		break;
	}
	case Phase::SkipSwitch:
	{
		//明転しきったら完了
		if (Fader::GetInstance().IsFinishFadeIn())
		{
			Finish();
		}
		break;
	}
	default:
		break;
	}
}

void CutSceneDirector::Draw() const
{
	//使用しているかどうか
	if (!IsPlaying())
	{
		return;
	}

	const int w			= Config::Window::WINDOW_W;				//画面幅
	const int h			= Config::Window::WINDOW_H;				//画面高さ
	const int barHeight = Config::Cutscene::LETTERBOX_HEIGHT;   //帯の高さ

	//上の帯
	DrawBox(0, 0, w, barHeight, GetColor(0, 0, 0), TRUE);
	//下の帯
	DrawBox(0, h - barHeight, w, h, GetColor(0, 0, 0), TRUE);
}

void CutSceneDirector::SkipToEnd()
{
	//扉を閉じて当たり判定を復活
	stage.CloseDoor();
	stage.CloseDoorWall();

	//プレイヤーをボス部屋の中に配置
	VECTOR pos = player.GetPosition();
	pos.x = Config::Cutscene::PLAYER_START_X;
	pos.z = Config::Cutscene::BOSS_ROOM_ENTER_Z;
	player.SetPosition(pos);
	player.PlayCutsceneIdle();

	//ボスを起こして、通常戦闘状態に
	{
		boss.Activate();
		boss.ChangeState(BossStateId::Idle);
	}

	//カメラを通常の三人称に戻す
	camera.SetMode(CameraMode::ThirdPerson);
}