#include "DxLib.h"
#include "Boss.h"
#include "Player.h"
#include "SharedContext.h"
#include "ResourceManager.h"
#include "BossBehaviors.h"
#include "BossAttackMelee.h"

void  BossIntroState::OnEnter()
{
	timer = 0.0f;
	finished = false;

	auto& comp = boss.Comp();

	goalY = boss.FloorYAt(comp.pos);
	
	startY = goalY - Config::Boss::INTRO_RISE_DEPTH;

	comp.pos.y = startY;
	comp.vy = 0.0f;

	//出現アニメ
	const int appear = ResourceManager::Instance().Model("boss_appear");
	if (appear >= 0)
	{
		boss.PlayAnim(appear, 0, false);
	}
	else
	{
		const int idle = ResourceManager::Instance().Model("boss_idle");
		if (idle >= 0)
		{
			boss.PlayAnim(idle, 0, true);
		}

	}
}

void BossIntroState::Update(float dt)
{
	timer += dt;

	auto& comp = boss.Comp();

	//魔法陣が広がるのを待つ
	if (timer < Config::Boss::INTRO_DELAY)
	{
		return;  
	}

	//せり上がり
	float t = (timer - Config::Boss::INTRO_DELAY) / Config::Boss::INTRO_RISE_TIME;
	if (t > 1.0f)
	{
		t = 1.0f;
	}

	//イーズアウト
	const float ease = 1.0f - (1.0f - t) * (1.0f - t);
	comp.pos.y = startY + (goalY - startY) * ease;

	//プレイヤーの方を向いておく
	boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

	if (timer >= Config::Boss::INTRO_RISE_TIME)
	{
		comp.pos.y = goalY;
		finished = true;
		//boss.ChangeState(BossStateId::Idle);
	}
}

void BossIdleState::OnEnter()
{
	timer = 0.0f;
	const int idle = ResourceManager::Instance().Model("boss_idle");
	if (idle >= 0)
	{
		boss.PlayAnim(idle, 0, true);
	}
}

void BossIdleState::Update(float dt)
{
	timer += dt;

	//プレイヤーが行動範囲外なら反応しない
	if (!boss.IsPlayerInArena())
	{
		return;
	}

	//プレイヤーの方を向く
	boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

	const float dist = boss.DistanceToPlayer();

	//攻撃距離内でクールダウンが明けていれば攻撃
	if (dist <= Config::Boss::ATTACK_RANGE && boss.CanAttack())
	{
		boss.ChangeState(BossStateId::Attack);
		return;
	}

	//「攻撃距離＋余裕(CHASE_RANGE)」より遠ければ追跡
	if (dist > Config::Boss::CHASE_RANGE)
	{
		boss.ChangeState(BossStateId::Chase);
		return;
	}

}

void BossChaseState::OnEnter()
{
	const int move = ResourceManager::Instance().Model("boss_walk");
	if (move >= 0)
	{
		boss.PlayAnim(move, 0, true);
	}
}

void BossChaseState::Update(float dt)
{
	if (!boss.IsPlayerInArena())
	{
		boss.ChangeState(BossStateId::Idle);
		return;
	}

	boss.FaceTowardDeg(boss.YawToPlayerDeg(), dt);

	const float dist = boss.DistanceToPlayer();

	//攻撃距離に入ったら攻撃
	if (dist <= Config::Boss::ATTACK_RANGE)
	{
		if (boss.CanAttack())
		{
			boss.ChangeState(BossStateId::Attack);
			return;
		}
		//クールダウン中は、攻撃距離まで来たらIdleで待つ
		boss.ChangeState(BossStateId::Idle);
		return;
	}

	//まだ遠いので近づく（STOP_DISTANCEまで）
	if (dist > Config::Boss::STOP_DISTANCE)
	{
		boss.MoveTowardPlayer(Config::Boss::MOVE_SPEED, dt);
	}
}

void BossAttackState::OnEnter()
{
	//使える攻撃パターンを選ぶ
	BossAttack* atk = boss.PickAttack();
	boss.SetCurrentAttack(atk);

	if (atk)
	{
		atk->OnStart(boss);
	}
}

void BossAttackState::Update(float dt)
{
	BossAttack* atk = boss.CurrentAttack();
	if (!atk)
	{
		//攻撃が無ければ待機へ
		boss.ChangeState(BossStateId::Idle);
		return;
	}

	//攻撃パターンを進める。falseで終了。
	const bool continuing = atk->Update(boss, dt);
	if (!continuing)
	{
		boss.ChangeState(BossStateId::Idle);
	}
}

void BossAttackState::OnExit()
{
	BossAttack* atk = boss.CurrentAttack();
	if (atk)
	{
		atk->OnEnd(boss);
	}
	//攻撃判定を確実に切る
	boss.SetAttackActive(false);
	//次の攻撃までのクールダウンを開始
	boss.StartCooldown();
	boss.SetCurrentAttack(nullptr);
}

void BossDrawSwordState::OnEnter()
{
	timer = 0.0f;
	roared = false;
	drawn = false;

	//咆哮アニメ
	boss.PlayRoar();
	roared = true;

	//咆哮の間、カメラを揺らす（GameSceneでシェイクしてもよい）
}

void BossDrawSwordState::Update(float dt)
{
	timer += dt;

	//咆哮中、プレイヤーを遠ざける（近いほど強く後退）
	if (timer < Config::Boss::DrawSword::PUSH_TIME)
	{
		Player& pl = boss.GetPlayer();
		VECTOR toPlayer = VSub(pl.GetPosition(), boss.Comp().pos);
		toPlayer.y = 0.0f;
		const float dist = VSize(toPlayer);

		if (dist > 1e-4f && dist < Config::Boss::DrawSword::PUSH_RANGE)
		{
			const float strength = 1.0f - (dist / Config::Boss::DrawSword::PUSH_RANGE);
			const VECTOR pushDir = VScale(toPlayer, 1.0f / dist);
			const float pushSpeed = Config::Boss::DrawSword::PUSH_SPEED * strength;

			VECTOR pos = pl.GetPosition();
			pos.x += pushDir.x * pushSpeed * dt;
			pos.z += pushDir.z * pushSpeed * dt;
			pl.SetPosition(pos);
		}
		
	}

	//咆哮が終わったら、抜刀アニメに切り替え
	if (!drawAnimStarted && timer >= Config::Boss::DrawSword::ROAR_TIME)
	{
		const int drawAnim = ResourceManager::Instance().Model("boss_draw_sword");
		if (drawAnim >= 0)
		{
			boss.PlayAnim(drawAnim, 0, false);
			boss.Anim().SetSpeed(Config::Boss::DrawSword::DRAW_ANIM_SPEED);
		}

		drawAnimStarted = true;
	}

	//抜刀アニメの途中で、実際に大剣を手に移す
	if (!drawn && timer >= Config::Boss::DrawSword::DRAW_TIME)
	{
		boss.DrawSword();   
		drawn = true;
	}

	//演出終了→戦闘再開
	if (timer >= Config::Boss::DrawSword::TOTAL_TIME)
	{
		boss.ChangeState(BossStateId::Idle);
	}
}

void BossDamageState::OnEnter()
{
	timer = 0.0f;
	boss.SetAttackActive(false);

	const int dmg = ResourceManager::Instance().Model("boss_damaged");
	if (dmg >= 0)
	{
		boss.PlayAnim(dmg, 0, false);
	}

}

void BossDamageState::Update(float dt)
{
	timer += dt;
	auto& comp = boss.Comp();

	//のけぞり移動（軽く後退）
	comp.pos.x += knockDir.x * 200.0f * dt;
	comp.pos.z += knockDir.z * 200.0f * dt;

	if (timer >= Config::Boss::DAMAGED_DURATION)
	{
		boss.ChangeState(BossStateId::Idle);
	}
}

void BossStaggerState::OnEnter()
{
	timer = 0.0f;
	boss.SetAttackActive(false);   //攻撃判定を切る

	//怯みアニメ（無ければダメージアニメ）
	const int stagger = ResourceManager::Instance().Model("boss_stagger");
	if (stagger >= 0) boss.PlayAnim(stagger, 0, false);
	else
	{
		const int dmg = ResourceManager::Instance().Model("boss_damaged");
		if (dmg >= 0) boss.PlayAnim(dmg, 0, true);
	}
}

void BossStaggerState::Update(float dt)
{
	timer += dt;

	//怯み中は動かない（プレイヤーの攻撃チャンス）

	if (timer >= Config::Boss::STAGGER_DURATION)
	{
		boss.ChangeState(BossStateId::Idle);
	}
}

void BossDeadState::OnEnter()
{
	timer = 0.0f;
	animFinished = false;
	boss.SetAttackActive(false);

	const int dead = ResourceManager::Instance().Model("boss_dead");
	if (dead >= 0)
	{
		boss.PlayAnim(dead, 0, false);
	}

}

void BossDeadState::Update(float dt)
{
	timer += dt;
	//重力で地面に留まる（Boss::Updateのgravityが効く）

	if (timer >= Config::Boss::DEAD_DURATION)
	{
		animFinished = true;
	}
}