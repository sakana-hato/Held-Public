#include "DxLib.h"
#include "SharedContext.h"
#include "Config.h"
#include "Boss.h"
#include "Difficulty.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Stage.h"
#include "BossState.h"
#include "BossAttackMelee.h"
#include "BossBehaviors.h"
#include "BossAttackJump.h"
#include "BossAttackShockwave.h"
#include "BossAttackBeam.h"
#include "BossAttackMagic.h"
#include "BossAttackCharge.h"

Boss::Boss(SharedContext& ctx, CameraSystem& camera, Stage& stage,Player& player, ProjectileManager& projectiles)
	:ctx_(ctx)
	,camera(camera)
	,stage(stage)
	,player(player)
	,projectiles(projectiles)
{
	BuildStates();
	BuildAttacks();

	//難易度別HP
	comp.hp = DifficultyParam::BossHP(ctx_.difficulty);
	comp.hpMax = comp.hp;

	current = states[static_cast<size_t>(BossStateId::Intro)].get();
}

Boss::~Boss()
{
	stunChicks.End();
}

void Boss::BuildStates()
{
	states.resize(static_cast<size_t>(BossStateId::Count));
	states[static_cast<size_t>(BossStateId::Intro)]		= std::make_unique<BossIntroState>	(*this);
	states[static_cast<size_t>(BossStateId::Idle)]		= std::make_unique<BossIdleState>	(*this);
	states[static_cast<size_t>(BossStateId::Chase)]		= std::make_unique<BossChaseState>	(*this);
	states[static_cast<size_t>(BossStateId::Attack)]	= std::make_unique<BossAttackState>	(*this);
	states[static_cast<size_t>(BossStateId::DrawSword)] = std::make_unique<BossDrawSwordState>(*this);
	states[static_cast<size_t>(BossStateId::Damage)]	= std::make_unique<BossDamageState>	(*this);
	states[static_cast<size_t>(BossStateId::Stagger)] = std::make_unique<BossStaggerState>(*this);
	states[static_cast<size_t>(BossStateId::Dead)]		= std::make_unique<BossDeadState>	(*this);
}

void Boss::BuildAttacks()
{
	//ここに技を足していくよん
	//とりま近接攻撃1個
	attacks.push_back(std::make_unique<BossAttackMelee>());
	attacks.push_back(std::make_unique<BossAttackJump>());
	attacks.push_back(std::make_unique<BossAttackShockwave>());
	attacks.push_back(std::make_unique<BossAttackBeam>());
	attacks.push_back(std::make_unique<BossAttackMagic>());
	attacks.push_back(std::make_unique<BossAttackCharge>());
	//今後：武器攻撃、ミサイル、魔法を push_back で追加していくもん
}

void Boss::SetModel(int handle, float scale)
{
	modelHandle = handle;
	modelScale = scale;
	animator.SetModel(handle);

	afterImageModels.clear();
	afterImageModels.reserve(AFTERIMAGE_MAX);
	for (size_t i = 0; i < AFTERIMAGE_MAX; ++i)
	{
		const int m = MV1DuplicateModel(modelHandle);
		if (m >= 0)
		{
			MV1SetScale(m, VGet(modelScale, modelScale, modelScale));
			afterImageModels.push_back(m);
		}
	}

	const int chickModel = ResourceManager::Instance().Model("chick");
	if (chickModel >= 0)
	{
		stunChicks.Init(chickModel, 0.5f);   //スケールは実機で調整
	}

	const int rockModel = ResourceManager::Instance().Model("rock_ring");
	if (rockModel >= 0)
	{
		rockRing.Init(rockModel);
	}
	/*
	* if (current)
	{
		current->OnEnter();
	}
	*/
	
}

void Boss::SetSword(int handle)
{
	sword.SetModel(handle);
	sword.SetScale(Config::BossSword::MODEL_SCALE);

	if (modelHandle >= 0)
	{
		swordBackFrame = MV1SearchFrame(modelHandle, "mixamorig:Spine2");      
		swordHandFrame = MV1SearchFrame(modelHandle, "mixamorig:LeftHand");
	}

	SheatheSword();
}

void Boss::ChangeState(BossStateId id)
{
	if (current)
	{
		current->OnExit();
	}

	current = states[static_cast<size_t>(id)].get();

	if (current)
	{
		current->OnEnter();
	}
}

BossStateId Boss::CurrentStateId() const
{
	if (!current)
	{
		return BossStateId::Idle;
	}

	return static_cast<BossStateId>(current->Id());
}

void Boss::Update(float dt)
{

	if (!active)
	{
		return;
	}

	TickCooldown(dt);

	UpdatePhase();

	rockRing.Update(dt);

	warning.Update(dt);

#if defined(_DEBUG)
	DebugAdjustSword();   //大剣オフセットの実機調整
#endif

	if (current)
	{
		current->Update(dt);
	}

	
	if (CurrentStateId() != BossStateId::Intro)
	{
		ApplyGravity(dt);
	}

	if (CurrentStateId() == BossStateId::Stagger)
	{
		stunChicks.Update(dt);
	}

	

	//モデル反映
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

	const int followFrame = swordDrawn ? swordHandFrame : swordBackFrame;
	sword.Update(modelHandle, followFrame);
}

void Boss::DrawSword()
{
	swordDrawn = true;

	//手に持つときのオフセット
	MATRIX rot = MMult(MMult(
		MGetRotX(Config::BossSword::HAND_PITCH_RAD),
		MGetRotY(Config::BossSword::HAND_YAW_RAD)),
		MGetRotZ(Config::BossSword::HAND_ROLL_RAD));
	MATRIX tr = MGetTranslate(VGet(
		Config::BossSword::HAND_OFFSET_X,
		Config::BossSword::HAND_OFFSET_Y,
		Config::BossSword::HAND_OFFSET_Z));
	sword.SetOffset(MMult(rot, tr));
}

void Boss::SheatheSword()
{
	swordDrawn = false;

	//背中に背負うときのオフセット
	MATRIX rot = MMult(MMult(
		MGetRotX(Config::BossSword::BACK_PITCH_RAD),
		MGetRotY(Config::BossSword::BACK_YAW_RAD)),
		MGetRotZ(Config::BossSword::BACK_ROLL_RAD));
	MATRIX tr = MGetTranslate(VGet(
		Config::BossSword::BACK_OFFSET_X,
		Config::BossSword::BACK_OFFSET_Y,
		Config::BossSword::BACK_OFFSET_Z));
	sword.SetOffset(MMult(rot, tr));
}

void Boss::Draw() const
{
	if (!active)
	{
		return;
	}

	if (modelHandle >= 0)
	{
		
		MV1DrawModel(modelHandle);
		sword.Draw();
		
	}

	if (CurrentStateId() == BossStateId::Stagger)
	{
		//頭のボーンの位置
		const int headFrame = MV1SearchFrame(modelHandle, "mixamorig:Head");
		if (headFrame >= 0)
		{
			const VECTOR headPos = MV1GetFramePosition(modelHandle, headFrame);
			stunChicks.Draw(headPos);
		}
	}

	auto* charge = dynamic_cast<BossAttackCharge*>(currentAttack);
	if (charge)
	{
		const auto& images = charge->AfterImages();
		size_t idx = 0;

		for (const auto& ai : images)
		{
			if (idx >= afterImageModels.size()) break;
			const int m = afterImageModels[idx];
			if (m < 0) { ++idx; continue; }

			if (ai.animModel >= 0)
			{
				const int attach = MV1AttachAnim(m, ai.animIndex, ai.animModel, TRUE);
				if (attach >= 0) MV1SetAttachAnimTime(m, attach, ai.animTime);

				MV1SetPosition(m, ai.pos);
				MV1SetRotationXYZ(m, VGet(0.0f,
					(ai.yawDeg + modelYawOffsetDeg) * DX_PI_F / 180.0f, 0.0f));

				//寿命の進行（0＝生まれたて、1＝消える直前）
				const float t = ai.life / Config::Boss::Charge::AFTERIMAGE_LIFE;

				//色を補間：水色   黄緑
				COLOR_F col;
				col.r = Config::Boss::Charge::AI_COLOR_START_R + (Config::Boss::Charge::AI_COLOR_END_R - Config::Boss::Charge::AI_COLOR_START_R) * t;
				col.g = Config::Boss::Charge::AI_COLOR_START_G + (Config::Boss::Charge::AI_COLOR_END_G - Config::Boss::Charge::AI_COLOR_START_G) * t;
				col.b = Config::Boss::Charge::AI_COLOR_START_B + (Config::Boss::Charge::AI_COLOR_END_B - Config::Boss::Charge::AI_COLOR_START_B) * t;
				col.a = 1.0f;

				MV1SetDifColorScale(m, col);

				//透明度（寿命で薄く）
				const float alpha = (1.0f - t) * Config::Boss::Charge::AFTERIMAGE_ALPHA;
				MV1SetOpacityRate(m, alpha);

				MV1DrawModel(m);

				if (attach >= 0) MV1DetachAnim(m, attach);
			}
			++idx;
		}
	}

	rockRing.Draw();
	//warning.Draw();

#if defined(_DEBUG)
	
	const Capsule body = GetBodyCapsule();
	DrawCapsule3D(body.p0, body.p1, body.radius, 8, GetColor(255, 120, 0), GetColor(255, 120, 0), FALSE);

	//攻撃判定
	if (attackActive)
	{
		const Capsule atk = GetAttackCapsule();
		DrawCapsule3D(atk.p0, atk.p1, atk.radius, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
	}

	if (swordDrawn)
	{
		const Capsule blade = GetSwordBladeCapsule();
		DrawCapsule3D(blade.p0, blade.p1, blade.radius, 8, GetColor(0, 200, 255), GetColor(0, 200, 255), FALSE);
	}

	if (IsBeamFiring())
	{
		const Capsule beam = GetBeamCapsule();
		DrawCapsule3D(beam.p0, beam.p1, beam.radius, 8, GetColor(255, 0, 255), GetColor(255, 0, 255), FALSE);
	}

	//HP・状態
	DrawFormatString(10, 200, GetColor(255, 200, 0),
		"BOSS HP:%.0f/%.0f  phase:%d  state:%d",
		comp.hp, comp.hpMax, comp.phase, static_cast<int>(CurrentStateId()));

	//行動範囲（ボス部屋）を円で描く
	{
		const VECTOR center = VGet(Config::Boss::ARENA_CENTER_X, 10.0f, Config::Boss::ARENA_CENTER_Z);
		const float r = Config::Boss::ARENA_RADIUS;
		const int seg = 48;   //円の分割数
		const unsigned int col = GetColor(255, 160, 0);

		for (int i = 0; i < seg; ++i)
		{
			const float a0 = (DX_TWO_PI_F * i) / seg;
			const float a1 = (DX_TWO_PI_F * (i + 1)) / seg;
			const VECTOR p0 = VGet(center.x + std::cos(a0) * r, center.y, center.z + std::sin(a0) * r);
			const VECTOR p1 = VGet(center.x + std::cos(a1) * r, center.y, center.z + std::sin(a1) * r);
			DrawLine3D(p0, p1, col);
		}
	}

	{
		const VECTOR center = VGet(Config::Boss::ARENA_CENTER_X, 10.0f, Config::Boss::ARENA_CENTER_Z);
		const float r = Config::Boss::ARENA_RADIUS;
		const int seg = 48;   //円の分割数
		const unsigned int col = GetColor(255, 160, 0);

		for (int i = 0; i < seg; ++i)
		{
			const float a0 = (DX_TWO_PI_F * i) / seg;
			const float a1 = (DX_TWO_PI_F * (i + 1)) / seg;
			const VECTOR p0 = VGet(center.x + std::cos(a0) * r, center.y, center.z + std::sin(a0) * r);
			const VECTOR p1 = VGet(center.x + std::cos(a1) * r, center.y, center.z + std::sin(a1) * r);
			DrawLine3D(p0, p1, col);
		}
	}

	DebugDrawSwordAxis();

#endif
}

void Boss::TakeDamage(float amount, const VECTOR& attackerPos)
{
	if (comp.IsDead())
	{
		return;
	}

	comp.hp -= amount;
	if (comp.hp <= 0.0f)
	{
		comp.hp = 0.0f;
		ChangeState(BossStateId::Dead);
		return;
	}

	health.NotifyHealthChanged(comp.hp, Config::Boss::HP_MAX, -amount);
	
}

void Boss::EnterStagger()
{
	if (!IsDead() && CurrentStateId() != BossStateId::Stagger)
	{
		ChangeState(BossStateId::Stagger);
	}
}

void Boss::UpdatePhase()
{
	const float rate = comp.HpRate();
	int newPhase = 0;
	if (rate <= Config::Boss::PHASE3_HP_RATE)
	{
		newPhase = 2;
	}
	else if (rate <= Config::Boss::PHASE2_HP_RATE)
	{
		newPhase = 1;
	}	
	else
	{
		newPhase = 0;
	}

	if (newPhase >= 1 && comp.phase == 0 && !swordDrawn)
	{
		comp.phase = newPhase;
		ChangeState(BossStateId::DrawSword);
		return;
	}

	comp.phase = newPhase;
}

void Boss::StartCooldown()
{
	//フェーズが進むほど攻撃間隔が短くなる
	float cd = Config::Boss::ATTACK_COOLDOWN_P1;
	if (comp.phase == 1)
	{
		cd = Config::Boss::ATTACK_COOLDOWN_P2;
	}
	else if (comp.phase == 2)
	{
		cd = Config::Boss::ATTACK_COOLDOWN_P3;
	}
	attackCooldown = cd;
}

BossAttack* Boss::PickAttack()
{
	//使える攻撃を集めて、その中からランダムに選ぶ
	std::vector<BossAttack*> usable;
	for (auto& att : attacks)
	{
		if (att->IsUsable(*this))
		{
			usable.push_back(att.get());
		}
	}

	if (usable.empty())
	{
		return attacks.empty() ? nullptr : attacks.front().get();
	}

	const int idx = GetRand(static_cast<int>(usable.size()) - 1);
	return usable[idx];
}

Capsule Boss::GetBodyCapsule() const
{
	Capsule cap;
	cap.p0 = comp.CapsuleBottom();
	cap.p1 = comp.CapsuleTop();
	cap.radius = comp.radius;
	return cap;
}

bool Boss::IsBeamFiring() const
{
	if (!currentAttack)
	{
		return false;
	}
	//現在の攻撃がビームで、発射中か
	auto* beam = dynamic_cast<BossAttackBeam*>(currentAttack);
	return beam && beam->IsFiring();
}

Capsule Boss::GetBeamCapsule() const
{
	Capsule c{ VGet(0,0,0), VGet(0,0,0), 0.0f };
	if (!currentAttack)
	{
		return c;
	}

	auto* beam = dynamic_cast<BossAttackBeam*>(currentAttack);
	if (beam && beam->IsFiring())
	{
		c.p0 = beam->BeamStart();
		c.p1 = beam->BeamEnd();
		c.radius = Config::Boss::Beam::RADIUS;
	}
	return c;
}

Capsule Boss::GetAttackCapsule() const
{
	//攻撃判定を出すボーン

	if (currentAttack)
	{
		auto* charge = dynamic_cast<BossAttackCharge*>(currentAttack);
		if (charge && charge->IsDashing())
		{
			return GetBodyCapsule();   //体全体が攻撃判定
		}
	}

	if (swordDrawn)
	{
		return GetSwordBladeCapsule();
	}

	Capsule cap;
	if (attackFrame >= 0 && modelHandle >= 0)
	{
		const VECTOR handPos = MV1GetFramePosition(modelHandle, attackFrame);
		cap.p0 = handPos;
		cap.p1 = handPos;
		cap.radius = Config::Boss::Melee::HAND_RADIUS;
	}
	else
	{
		cap.p0 = comp.pos;
		cap.p1 = comp.pos;
		cap.radius = 0.0f;
	}
	return cap;
}

Capsule Boss::GetSwordBladeCapsule() const
{
	//大剣を抜いていなければ判定なし
	if (!swordDrawn)
	{
		const VECTOR zero = VGet(0.0f, 0.0f, 0.0f);
		return Capsule{ zero, zero, 0.0f };
	}

	//刃の補正行列
	MATRIX rot = MMult(MMult(
		MGetRotX(Config::BossSword::BLADE_PITCH_RAD),
		MGetRotY(Config::BossSword::BLADE_YAW_RAD)),
		MGetRotZ(Config::BossSword::BLADE_ROLL_RAD));
	MATRIX ofs = MMult(rot, MGetTranslate(VGet(
		Config::BossSword::BLADE_OFFSET_X,
		Config::BossSword::BLADE_OFFSET_Y,
		Config::BossSword::BLADE_OFFSET_Z)));

	//大剣のワールド行列に補正を掛ける
	const MATRIX mat = MMult(ofs, sword.GetWorldMatrix());

	//刃の根本・先端をローカルで作り、変換
	const VECTOR root = VTransform(VGet(0.0f, 0.0f, Config::BossSword::BLADE_ROOT_Z), mat);
	const VECTOR tip = VTransform(VGet(0.0f, 0.0f, Config::BossSword::BLADE_TIP_Z), mat);

	return Capsule{ root, tip, Config::BossSword::BLADE_RADIUS };
}

float Boss::DistanceToPlayer() const
{
	VECTOR dis = VSub(player.GetPosition(), comp.pos);
	dis.y = 0.0f;
	return VSize(dis);
}

VECTOR Boss::DirToPlayer() const
{
	
	VECTOR dir = VSub(player.GetPosition(), comp.pos);
	dir.y = 0.0f;
	const float len = VSize(dir);

	if (len > 1e-4f)
	{
		return VScale(dir, 1.0f / len);
	}
	return comp.Forward();
}

float Boss::YawToPlayerDeg() const
{
	const VECTOR dir = DirToPlayer();
	return std::atan2(dir.x, dir.z) * 180.0f / DX_PI_F;
}

void Boss::FaceTowardDeg(float targetYawDeg, float dt)
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

	const float maxTurn = Config::Boss::TURN_SPEED * dt;
	if (diff > maxTurn)
	{
		diff = maxTurn;
	}

	if (diff < -maxTurn)
	{
		diff = -maxTurn;
	}

	comp.facingYawDeg += diff;
}

void Boss::MoveTowardPlayer(float speed, float dt)
{
	const VECTOR dir = DirToPlayer();
	comp.pos.x += dir.x * speed * dt;
	comp.pos.z += dir.z * speed * dt;

	//行動範囲（ボス部屋）内に制限
	const VECTOR arenaCenter = VGet(Config::Boss::ARENA_CENTER_X, comp.pos.y, Config::Boss::ARENA_CENTER_Z);
	VECTOR fromCenter = VSub(comp.pos, arenaCenter);
	fromCenter.y = 0.0f;
	const float distFromCenter = VSize(fromCenter);
	if (distFromCenter > Config::Boss::ARENA_RADIUS)
	{
		//範囲の縁に押し戻す
		const VECTOR clamped = VScale(VNorm(fromCenter), Config::Boss::ARENA_RADIUS);
		comp.pos.x = arenaCenter.x + clamped.x;
		comp.pos.z = arenaCenter.z + clamped.z;
	}

	//壁で止める
	comp.pos = stage.ResolveWall(comp.pos, comp.radius, comp.height);
}

float Boss::FloorYAt(const VECTOR& p) const
{
	float y = 0.0f;
	if (stage.GetFloorY(p, y))
	{
		return y;
	}
	return 0.0f;
}

void Boss::ApplyGravity(float dt)
{
	const float floorY = FloorYAt(comp.pos);

	comp.vy -= Config::Player::Status::GRAVITY * dt;
	comp.pos.y += comp.vy * dt;

	if (comp.pos.y <= floorY)
	{
		comp.pos.y = floorY;
		comp.vy = 0.0f;
	}
}

void Boss::PlayAnim(int animModel, int animIndex, bool loop)
{
	animator.Play(animModel, animIndex, loop);
}

void Boss::PlayRoar()
{
	const int roar = ResourceManager::Instance().Model("boss_roar");
	if (roar >= 0)
	{
		animator.Play(roar, 0, false);
	}
}

bool Boss::IsIntroFinished()const
{
	if (!current)
	{
		return true;
	}

	if (CurrentStateId() != BossStateId::Intro)
	{
		return false;
	}

	const auto* intro = static_cast<const BossIntroState*>(current);

	return intro->IsFinished();
}

bool Boss::IsPlayerInArena() const
{
	const VECTOR center = VGet(Config::Boss::ARENA_CENTER_X, 0.0f, Config::Boss::ARENA_CENTER_Z);
	VECTOR dir			= VSub(player.GetPosition(), center);
	dir.y				= 0.0f;
	return VSize(dir)	<= Config::Boss::ARENA_RADIUS;
}

bool Boss::IsAttackJustDodgeWindow() const
{
	if (!currentAttack)
	{
		return false;
	}
	return currentAttack->IsJustDodgeWindow(*this);
}

void Boss::DrawBeamWarning() const
{
	if (!currentAttack)
	{
		return;
	}

	auto* beam = dynamic_cast<BossAttackBeam*>(currentAttack);
	if (!beam || !beam->IsCharging()) return;

	//予告の始点・終点（地面の高さに）
	VECTOR s = beam->PredictStart();
	VECTOR e = beam->PredictEnd();
	const float y = FloorYAt(comp.pos) + 5.0f;   //地面に少し浮かせて
	s.y = y;
	e.y = y;

	//帯の方向（s→e）と、それに垂直な横方向
	VECTOR dir = VSub(e, s);
	dir.y = 0.0f;
	const float len = VSize(dir);
	if (len < 1e-4f)
	{
		return;
	}

	dir = VScale(dir, 1.0f / len);

	//横方向（dirを90度回転）
	const VECTOR side = VGet(-dir.z, 0.0f, dir.x);
	const float halfWidth = Config::Boss::Beam::RADIUS;   //帯の幅の半分

	//長方形の4隅
	const VECTOR p0 = VAdd(s, VScale(side, halfWidth));
	const VECTOR p1 = VAdd(s, VScale(side, -halfWidth));
	const VECTOR p2 = VAdd(e, VScale(side, halfWidth));
	const VECTOR p3 = VAdd(e, VScale(side, -halfWidth));

	//点滅（timerで明滅させると警告らしい。ここでは簡易に固定alpha）
	const int alpha = 120;
	SetUseLighting(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetUseBackCulling(FALSE);   //両面描画（警告円で必要だった）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	const unsigned int col = GetColor(255, 30, 30);

	//長方形を2つの三角形で塗る
	DrawTriangle3D(p0, p1, p2, col, TRUE);
	DrawTriangle3D(p1, p3, p2, col, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseBackCulling(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
}

void Boss::DrawChargeWarning() const
{
	if (!currentAttack)
	{
		return;
	}

	auto* charge = dynamic_cast<BossAttackCharge*>(currentAttack);
	if (!charge || !charge->IsAiming())
	{
		return;
	}

	VECTOR s = charge->AimStart();
	VECTOR e = charge->AimEnd();
	const float y = FloorYAt(comp.pos) + 5.0f;
	s.y = y;
	e.y = y;

	VECTOR dir = VSub(e, s);
	dir.y = 0.0f;
	const float len = VSize(dir);
	if (len < 1e-4f)
	{
		return;
	}

	dir = VScale(dir, 1.0f / len);

	const VECTOR side = VGet(-dir.z, 0.0f, dir.x);
	const float halfWidth = Config::Boss::Charge::DASH_WIDTH;

	const VECTOR p0 = VAdd(s, VScale(side, halfWidth));
	const VECTOR p1 = VAdd(s, VScale(side, -halfWidth));
	const VECTOR p2 = VAdd(e, VScale(side, halfWidth));
	const VECTOR p3 = VAdd(e, VScale(side, -halfWidth));

	SetUseLighting(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetUseBackCulling(FALSE);   //両面描画（必須）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);

	const unsigned int col = GetColor(255, 30, 30);
	DrawTriangle3D(p0, p1, p2, col, TRUE);
	DrawTriangle3D(p1, p3, p2, col, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetUseBackCulling(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
}

bool Boss::IsCharging() const
{
	if (!currentAttack)
	{
		return false;
	}

	auto* charge = dynamic_cast<BossAttackCharge*>(currentAttack);
	return charge && charge->IsDashing();
}

#if defined(_DEBUG)
void Boss::DebugAdjustSword()
{
	//Mキーで調整モードのトグル
	static bool prevM = false;
	const bool nowM = (CheckHitKey(KEY_INPUT_M) != 0);
	if (nowM && !prevM)
	{
		swordAdjustMode = !swordAdjustMode;
		if (swordAdjustMode) DrawSword();
	}
	prevM = nowM;

	if (!swordAdjustMode) return;

	//矢印キーでオフセット（位置）
	const float moveStep = 1.0f;
	if (CheckHitKey(KEY_INPUT_LEFT))  dbgSwordOffset.x -= moveStep;
	if (CheckHitKey(KEY_INPUT_RIGHT)) dbgSwordOffset.x += moveStep;
	if (CheckHitKey(KEY_INPUT_UP))    dbgSwordOffset.y += moveStep;
	if (CheckHitKey(KEY_INPUT_DOWN))  dbgSwordOffset.y -= moveStep;
	if (CheckHitKey(KEY_INPUT_PGUP))  dbgSwordOffset.z += moveStep;
	if (CheckHitKey(KEY_INPUT_PGDN))  dbgSwordOffset.z -= moveStep;

	//テンキーで回転（RAD）
	const float rotStep = 0.02f;
	if (CheckHitKey(KEY_INPUT_NUMPAD8)) dbgSwordPitch += rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD2)) dbgSwordPitch -= rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD4)) dbgSwordYaw += rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD6)) dbgSwordYaw -= rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD7)) dbgSwordRoll += rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD9)) dbgSwordRoll -= rotStep;

	//調整したオフセットを大剣に適用
	MATRIX rot = MMult(MMult(
		MGetRotX(dbgSwordPitch),
		MGetRotY(dbgSwordYaw)),
		MGetRotZ(dbgSwordRoll));
	MATRIX tr = MGetTranslate(dbgSwordOffset);
	sword.SetOffset(MMult(rot, tr));

}

void Boss::DebugDrawSwordAdjust() const
{
	if (!swordAdjustMode) return;

	DrawFormatString(10, 400, GetColor(255, 255, 0),
		"SWORD ADJUST (M:toggle) arrows:XY PgUp/Dn:Z");
	DrawFormatString(10, 420, GetColor(255, 255, 0),
		"rot: NUM8/2:pitch NUM4/6:yaw NUM7/9:roll");
	DrawFormatString(10, 440, GetColor(255, 255, 0),
		"OFFSET X:%.1f Y:%.1f Z:%.1f", dbgSwordOffset.x, dbgSwordOffset.y, dbgSwordOffset.z);
	DrawFormatString(10, 460, GetColor(255, 255, 0),
		"PITCH:%.3f YAW:%.3f ROLL:%.3f", dbgSwordPitch, dbgSwordYaw, dbgSwordRoll);
}

void Boss::DebugDrawSwordAxis() const
{
	if (!swordAdjustMode) return;

	//手のボーンの位置
	const int handFrame = MV1SearchFrame(modelHandle, "mixamorig:RightHand");
	if (handFrame < 0) return;

	const VECTOR handPos = MV1GetFramePosition(modelHandle, handFrame);
	const float axisLen = 100.0f;   //軸の長さ

	//手のボーンのワールド行列（向きを含む）
	MATRIX handMat = MV1GetFrameLocalWorldMatrix(modelHandle, handFrame);

	//各軸方向（行列の各列が軸方向）
	VECTOR axisX = VNorm(VGet(handMat.m[0][0], handMat.m[0][1], handMat.m[0][2]));
	VECTOR axisY = VNorm(VGet(handMat.m[1][0], handMat.m[1][1], handMat.m[1][2]));
	VECTOR axisZ = VNorm(VGet(handMat.m[2][0], handMat.m[2][1], handMat.m[2][2]));

	//XYZ軸を色分けで描画（X赤, Y緑, Z青）
	DrawLine3D(handPos, VAdd(handPos, VScale(axisX, axisLen)), GetColor(255, 0, 0));
	DrawLine3D(handPos, VAdd(handPos, VScale(axisY, axisLen)), GetColor(0, 255, 0));
	DrawLine3D(handPos, VAdd(handPos, VScale(axisZ, axisLen)), GetColor(0, 0, 255));

	//大剣自体の向き（調整中のオフセット回転を反映した軸）も描くと分かりやすい
	//大剣のオフセット回転を手の行列に掛けた向き
	MATRIX rot = MMult(MMult(MGetRotX(dbgSwordPitch), MGetRotY(dbgSwordYaw)), MGetRotZ(dbgSwordRoll));
	MATRIX swordMat = MMult(rot, handMat);
	VECTOR sX = VNorm(VGet(swordMat.m[0][0], swordMat.m[0][1], swordMat.m[0][2]));
	VECTOR sY = VNorm(VGet(swordMat.m[1][0], swordMat.m[1][1], swordMat.m[1][2]));
	VECTOR sZ = VNorm(VGet(swordMat.m[2][0], swordMat.m[2][1], swordMat.m[2][2]));

	//大剣の軸は少し暗めの色で（区別のため）
	DrawLine3D(handPos, VAdd(handPos, VScale(sX, axisLen * 1.2f)), GetColor(150, 0, 0));
	DrawLine3D(handPos, VAdd(handPos, VScale(sY, axisLen * 1.2f)), GetColor(0, 150, 0));
	DrawLine3D(handPos, VAdd(handPos, VScale(sZ, axisLen * 1.2f)), GetColor(0, 0, 150));
}
#endif