#include "DxLib.h"
#include "BossDeathDirector.h"
#include "SharedContext.h"
#include "Boss.h"
#include "CameraSystem.h"
#include "CameraMode.h"
#include "EffectManager.h"
#include "ResourceManager.h"
#include "Config.h"
#include "Player.h"

void BossDeathDirector::Start()
{
	
	Begin();          //基底：開始状態にする
	timer = 0.0f;
	phase = Phase::Falling;

	player.SetHidden(true);
	

	//死亡カメラを設定
	SetupDeathCamera();
}

void BossDeathDirector::SetupDeathCamera()
{
	
	const VECTOR bossPos = boss.Comp().pos;

	//アリーナ中心（壁対処B案：ここへ向かう方向にカメラを引く）
	const VECTOR arenaCenter = VGet(Config::Boss::ARENA_CENTER_X, 0.0f, Config::Boss::ARENA_CENTER_Z);

	//ボスから見てアリーナ中心へ向かう方向（＝部屋の内側＝壁のない側）
	VECTOR toCenter = VSub(arenaCenter, bossPos);
	toCenter.y = 0.0f;
	const float len = VSize(toCenter);

	VECTOR dir;
	if (len > 1e-3f)
	{
		dir = VScale(toCenter, 1.0f / len);
	}
	else
	{
		//ボスがほぼ中心にいる場合は、Z+方向から見る
		dir = VGet(0.0f, 0.0f, 1.0f);
	}

	//カメラ位置：ボスから中心方向へ水平に引き、高い位置に置く（斜め上から見下ろす）
	VECTOR camPos = bossPos;
	camPos.x += dir.x * Config::Boss::Death::CAM_BACK;
	camPos.z += dir.z * Config::Boss::Death::CAM_BACK;
	camPos.y += Config::Boss::Death::CAM_HEIGHT;

	//注視点：ボスの胴体あたり
	VECTOR camTgt = bossPos;
	camTgt.y += Config::Boss::Death::CAM_LOOK_HEIGHT;

	camera.SetMode(CameraMode::Cutscene);
	camera.SetCutsceneCamera(camPos, camTgt);
}

void BossDeathDirector::EnterPhase(Phase next)
{
	phase = next;
	timer = 0.0f;

	switch (next)
	{
	case Phase::MagicCircle:
	{
		//ボスの足元に魔法陣エフェクトを出す
		{
			VECTOR pos = boss.Comp().pos;
			pos.y = boss.FloorYAt(pos) + 10.0f;
			magicCircleInstance = EffectManager::Instance().Play(ResourceManager::Instance().Effect("boss_appear1"),pos, Config::Effect::BOSS_APPEAR_SCALE);
		}
		break;
	}
	case Phase::Sinking:
	{
		//沈み始めのY座標を記録するだけ（実際に沈めるのは Update）
		{
			sinkStartY = boss.Comp().pos.y;
		}
		break;
	}
	default:
		break;
	}
}

void BossDeathDirector::Update(float dt)
{
	if (!IsPlaying())
	{
		return;
	}

	timer += dt;

	//演出中もボス（死亡アニメ）とエフェクトは更新する
	if (phase != Phase::Sinking)
	{
		boss.Update(dt);
	}
	EffectManager::Instance().Update();

	switch (phase)
	{
	case Phase::Falling:
	{
		//倒れるアニメを見せる時間
		if (timer >= Config::Boss::Death::FALL_TIME)
		{
			EnterPhase(Phase::MagicCircle);
		}
		break;
	}
	case Phase::MagicCircle:
	{
		//魔法陣が出て広がるのを待つ
		if (timer >= Config::Boss::Death::CIRCLE_TIME)
		{
			EnterPhase(Phase::Sinking);
		}
		break;
	}
	case Phase::Sinking:
	{
		//ボスを地面の下へ沈めていく
		{
			auto& comp = boss.Comp();

			//イーズインで、だんだん速く沈む
			const float t = timer / Config::Boss::Death::SINK_TIME;
			const float clamped = (t > 1.0f) ? 1.0f : t;
			const float eased = clamped * clamped;   //イーズイン

			comp.pos.y = sinkStartY - Config::Boss::Death::SINK_DEPTH * eased;

			//モデルの位置を、沈めた座標に反映する（これが抜けていた）
			MV1SetPosition(boss.ModelHandle(), comp.pos);
		}

		if (timer >= Config::Boss::Death::SINK_TIME)
		{
			if (magicCircleInstance >= 0)
			{
				EffectManager::Instance().Stop(magicCircleInstance);
				magicCircleInstance = -1;
			}
			Finish();
		}
		break;
	}
	default:
		break;
	}
}