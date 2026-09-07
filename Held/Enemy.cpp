#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "Stage.h"
#include "SharedContext.h"
#include "Capsule.h"
#include "Player.h"
#include "EnemyStateId.h"
#include "EnemyState.h"
#include "Enemy.h"

namespace
{
	/// <summary>
	/// 方向ベクトルからY軸まわりの角度を求める
	/// </summary>
	/// <param name="dir"></param>水平の方向ベクトル
	float DirToYawDeg(const VECTOR& dir)
	{
		return std::atan2(dir.x, dir.z) * 180.0f / DX_PI_F;
	}
}

Enemy::Enemy(SharedContext& ctx,Stage& stage, Player& player)
	:ctx_(ctx)
	,stage(stage)
	,player(player)
{
	BuildStates();
	current = states[static_cast<size_t>(EnemyStateId::Idle)].get();
}

Enemy::~Enemy() = default;

void Enemy::SetModel(int handle, float scale)
{
	modelHandle		= handle;
	modelScale		= scale;
	animator.SetModel(handle);

	if (modelHandle>=0)
	{
		handFrame = MV1SearchFrame(modelHandle, "mixamorig:RightHand");
	}

	if (current)
	{
		current->OnEnter();
	}
}

void Enemy::BuildStates()
{
	using Id = EnemyStateId;

	states[static_cast<size_t>(Id::Idle)]	= std::make_unique<EnemyIdle>(*this);
	states[static_cast<size_t>(Id::Chase)]	= std::make_unique <EnemyChase>(*this);
	states[static_cast<size_t>(Id::Attack)] = std::make_unique<EnemyAttack>(*this);
	states[static_cast<size_t>(Id::Damage)] = std::make_unique<EnemyDamage>(*this);
	states[static_cast<size_t>(Id::Dead)]	= std::make_unique<EnemyDead>(*this);
}

void Enemy::Update(float dt)
{
	TickCooldown(dt);

	if (current)
	{
		current->Update(dt);
	}

	if (CurrentStateId() != EnemyStateId::Dead)
	{
		ApplyGravity(dt);
	}

	if (modelHandle >= 0)
	{
		MV1SetPosition(modelHandle, VGet(0.0f, 0.0f, 0.0f));
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, 0.0f, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));
	}

	animator.Update(dt);

	if (modelHandle >= 0)
	{
		const float yawRad = (comp.facingYawDeg + modelYawOffsetDeg) * DX_PI_F / 180.0f;
		MV1SetPosition(modelHandle, comp.pos);
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, yawRad, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));
	}
}

void Enemy::Draw()const
{
	if (modelHandle >= 0)
	{
		MV1SetPosition(modelHandle, comp.pos);
		const float yawRad = (comp.facingYawDeg + modelYawOffsetDeg) * DX_PI_F / 180.0f;
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, yawRad, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));

		SetUseLighting(FALSE);
		MV1DrawModel(modelHandle);
		SetUseLighting(TRUE);
	}

#if defined(_DEBUG)

	const Capsule body = GetBodyCapsule();
	DrawCapsule3D(body.p0, body.p1, body.radius, 12,
		GetColor(120, 200, 120), GetColor(255, 255, 255), FALSE);

	//攻撃判定が有効なら手カプセルを表示
	if (attackActive)
	{
		const Capsule atk = GetAttackCapsule();
		DrawCapsule3D(atk.p0, atk.p1, atk.radius, 8,
			GetColor(255, 80, 80), GetColor(255, 255, 255), FALSE);
	}

	//探知円・攻撃範囲・近づきすぎ距離を地面に描く（デバッグ）
	auto drawGroundCircle = [](const VECTOR& center, float radius, unsigned int color)
	{
			const int seg = 32;
			VECTOR prev = VGet(0, 0, 0);
			for (int i = 0; i <= seg; ++i)
			{
				const float a = (DX_PI_F * 2.0f) * i / seg;
				const VECTOR p = VGet(
					center.x + std::cos(a) * radius,
					center.y + 2.0f,   //地面から少し浮かせてZファイト防止
					center.z + std::sin(a) * radius);
				if (i > 0)
				{
					DrawLine3D(prev, p, color);
				}
				prev = p;
			}
	};

	const VECTOR foot = comp.pos;   //足元中心
	drawGroundCircle(foot, Config::Enemy::DETECT_RANGE, GetColor(80, 160, 255));	//探知円（青）
	drawGroundCircle(foot, Config::Enemy::ATTACK_RANGE, GetColor(255, 80, 80));		//攻撃範囲（赤）
	drawGroundCircle(foot, Config::Enemy::STOP_DISTANCE, GetColor(255, 220, 80));	//近づきすぎ距離（黄）

#endif
}

void Enemy::DrawHpBar() const
{
	//死んでいたら出さない
	if (comp.IsDead())
	{
		return;
	}

	const EnemyStateId state = CurrentStateId();
	if (state == EnemyStateId::Idle)
	{
		return;
	}

	//頭上のワールド座標（敵の頭より少し上）
	VECTOR headPos = comp.pos;
	headPos.y += comp.height + Config::Enemy::HpBar::HEIGHT_OFFSET;

	//ワールド座標→画面座標に変換
	VECTOR screen = ConvWorldPosToScreenPos(headPos);

	//画面より後ろ（カメラの裏）なら描かない
	if (screen.z <= 0.0f || screen.z >= 1.0f)
	{
		return;
	}

	//HPの割合
	const float rate		= comp.hp / Config::Enemy::HP_MAX;
	const float clampedRate = (rate < 0.0f) ? 0.0f : (rate > 1.0f ? 1.0f : rate);

	//バーのサイズ（画面ピクセル）
	const int barW			= Config::Enemy::HpBar::WIDTH;
	const int barH			= Config::Enemy::HpBar::HEIGHT;

	//バーの左上（画面座標。中央揃え）
	const int x				= static_cast<int>(screen.x) - barW / 2;
	const int y				= static_cast<int>(screen.y) - barH / 2;

	//背景（暗い下地）
	DrawBox(x - 1, y - 1, x + barW + 1, y + barH + 1, GetColor(20, 20, 20), TRUE);

	//減った部分の下地
	DrawBox(x, y, x + barW, y + barH, GetColor(60, 20, 20), TRUE);

	//現在HP（赤）
	const int hpW = static_cast<int>(barW * clampedRate);
	if (hpW > 0)
	{
		DrawBox(x, y, x + hpW, y + barH, GetColor(252, 90, 86), TRUE);
	}

	//枠線
	DrawBox(x, y, x + barW, y + barH, GetColor(200, 200, 200), FALSE);
}

Capsule Enemy::GetBodyCapsule() const
{
	return Capsule{ comp.CapsuleBottom(), comp.CapsuleTop(), comp.radius };
}

Capsule Enemy::GetAttackCapsule() const
{
	//小さなカプセルを作る
	if (handFrame < 0 || modelHandle < 0)
	{
		const VECTOR z = VGet(0.0f, 0.0f, 0.0f);
		return Capsule{ z, z, 0.0f };
	}

	const MATRIX hand		= MV1GetFrameLocalWorldMatrix(modelHandle, handFrame);
	const VECTOR handPos	= VTransform(VGet(0.0f, 0.0f, 0.0f), hand);

	//手の位置に球状のカプセルを置く。半径で殴りの範囲を表す。
	return Capsule{ handPos, handPos, Config::Enemy::HAND_CAPSULE_RADIUS };
}

void Enemy::TakeDamage(float amount, const VECTOR& attackerPos)
{
	if (comp.IsDead())
	{
		return;
	}

	comp.hp -= amount;

	if (comp.hp <= 0.0f)
	{
		comp.hp = 0.0f;
		
		VECTOR launchDir = player.GetForward();

		launchDir.y = 0.0f;
		const float len = VSize(launchDir);
		if (len > 1e-4f)
		{
			launchDir = VScale(launchDir, 1.0f / len);
		}


		//Dead状態に方向を渡して遷移
		auto* dead = static_cast<EnemyDead*>(states[static_cast<size_t>(EnemyStateId::Dead)].get());
		if (dead)
		{
			dead->SetKnockbackDir(launchDir);
		}
		ChangeState(EnemyStateId::Dead);
	}
	else
	{
		//のけぞり方向＝攻撃元→敵（押される向き）
		VECTOR knock = VSub(comp.pos, attackerPos);
		knock.y = 0.0f;
		const float len = VSize(knock);
		if (len > 1e-4f)
		{
			knock = VScale(knock, 1.0f / len);
		}
		else
		{
			knock = VScale(comp.Forward(), -1.0f);
		}

		//Damaged状態に方向を渡して遷移
		auto* dmg = static_cast<EnemyDamage*>(states[static_cast<size_t>(EnemyStateId::Damage)].get());

		if (dmg)
		{
			dmg->SetKnockbackDir(knock);
		}
		ChangeState(EnemyStateId::Damage);
	}
}

void Enemy::ChangeState(EnemyStateId id)
{
	if (id == EnemyStateId::Count)
	{
		return;
	}

	EnemyState* next = states[static_cast<size_t>(id)].get();

	if (!next || next == current)
	{
		return;
	}

	current->OnExit();
	current = next;
	current->OnEnter();
}

EnemyStateId Enemy::CurrentStateId() const
{
	return current ? static_cast<EnemyStateId>(current->Id()) : EnemyStateId::Count;
}

float Enemy::FloorYAt(const VECTOR& pos) const
{
	float y = Config::Graund::GROUND_Y;

	if (stage.GetFloorY(pos, y))
	{
		return y;
	}

	return Config::Graund::GROUND_Y;
}

void Enemy::ApplyGravity(float dt)
{
	comp.vy -= Config::Player::Status::GRAVITY * dt;
	comp.pos.y += comp.vy * dt;

	const float floorY = FloorYAt(comp.pos);
	if (comp.pos.y <= floorY)
	{
		comp.pos.y = floorY;
		comp.vy = 0.0f;
	}
}

void Enemy::FaceTowardDeg(float targetYawDeg, float dt)
{
	float diff = targetYawDeg - comp.facingYawDeg;
	while (diff > 180.0f)
	{
		diff -= 360.0f;
	}

	while (diff < -180.0f)
	{
		diff += 360.0f;
	}


	const float maxStep = Config::Enemy::TURN_SPEED * dt;
	if (std::fabs(diff) <= maxStep)
	{
		comp.facingYawDeg = targetYawDeg;
	}
	else
	{
		comp.facingYawDeg += (diff > 0.0f) ? maxStep : -maxStep;
	}


	while (comp.facingYawDeg > 180.0f)
	{
		comp.facingYawDeg -= 360.0f;
	}

	while (comp.facingYawDeg < -180.0f)
	{
		comp.facingYawDeg += 360.0f;
	}

}

float Enemy::DistanceToPlayer() const
{
	

	const VECTOR plpo = player.GetPosition();
	const VECTOR dir = VSub(plpo, comp.pos);
	return std::sqrt(dir.x * dir.x + dir.z * dir.z);   //水平距離
}

VECTOR Enemy::DirToPlayer() const
{
	VECTOR dir = VSub(player.GetPosition(), comp.pos);
	dir.y = 0.0f;
	const float len = VSize(dir);

	if (len > 1e-4f)
	{
		dir = VScale(dir, 1.0f / len);
	}

	return dir;
}

float Enemy::YawToPlayerDeg() const
{
	return DirToYawDeg(DirToPlayer());
}

VECTOR Enemy::AvoidObstacles(const VECTOR& desiredDir) const
{
	
	return desiredDir;
}

void Enemy::MoveTowardPlayer(float speed, float dt)
{
	const float dist = DistanceToPlayer();

	//近づきすぎ防止
	if (dist <= Config::Enemy::STOP_DISTANCE)
	{
		FaceTowardDeg(YawToPlayerDeg(), dt);
		comp.velocity = VGet(0.0f, 0.0f, 0.0f);
		return;
	}

	VECTOR dir = AvoidObstacles(DirToPlayer());

	
	FaceTowardDeg(DirToYawDeg(dir), dt);

	//前進
	comp.pos.x += dir.x * speed * dt;
	comp.pos.z += dir.z * speed * dt;
	comp.velocity = VScale(dir, speed);

	//柱・壁にめり込んだら押し戻し
	comp.pos = stage.ResolveWall(comp.pos, comp.radius, comp.height);
}

void Enemy::PlayAnim(int animModel, int animIndex, bool loop)
{
	animator.Play(animModel, animIndex, loop);
}