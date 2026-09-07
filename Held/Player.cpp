#define NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "CameraSystem.h"
#include "InputSystem.h"
#include "TargetSystem.h"

#include "SharedContext.h"
#include "ResourceManager.h"
#include "Stage.h"
#include "Player.h"
#include "PlayerState.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "EffectManager.h"


Player::Player(SharedContext& ctx, CameraSystem& camera, InputSystem& input, Stage& stage, TargetSystem& target)
	: ctx_(ctx)
	, camera(camera)
	, input(input)
	, stage(stage)
	, target(target)
{
	BuildStates();

	current = states[static_cast<size_t>(PlayerStateId::BaseMovement)].get();
	if (current)
	{
		current->OnEnter();
	}
}

Player::~Player() = default;

void Player::SetModel(int handle, float scale)
{
	modelHandle = handle;
	modelScale = scale;
	animator.SetModel(handle);

	rimVS_ = LoadVertexShader("Shader/PlayerrimVS.vso");
	rimPS_ = LoadPixelShader("Shader/PlayerrimPS.pso");
	rimCB_ = CreateShaderConstantBuffer(sizeof(float) * 8);
}

void Player::SetKatana(int handle)
{
	katana.Init(handle);
	katanaTrail.Init(15);
	katanaTrail.LoadShader();
	if (modelHandle >= 0)
	{
		katana.SetWaistFrame(MV1SearchFrame(modelHandle, "mixamorig:Hips"));
		katana.SetHandFrame(MV1SearchFrame(modelHandle, "mixamorig:RightHand"));
	}
}

void Player::SetSheath(int handle)
{
	sheathHandle = handle;
	sheath.SetModel(handle);
	sheath.SetScale(Config::Katana::MODEL_SCALE);   

	//鞘の腰オフセット（刀と同じ位置に置くなら刀の腰オフセットと同じ値）
	MATRIX rot = MMult(MMult(MGetRotX(Config::Sheath::WAIST_PITCH_RAD),MGetRotY(Config::Sheath::WAIST_YAW_RAD)),MGetRotZ(Config::Sheath::WAIST_ROLL_RAD));
	MATRIX tr = MGetTranslate(VGet(Config::Sheath::WAIST_OFFSET_X,Config::Sheath::WAIST_OFFSET_Y,Config::Sheath::WAIST_OFFSET_Z));
	sheath.SetOffset(MMult(rot, tr));
}

void Player::PlayAnim(int animModel, int animIndex, bool loop, bool useRootMotion)
{

	animator.Play(animModel, animIndex, loop);
	animator.SetSpeed(1.0f);
}

void Player::BuildStates()
{
	using Id = PlayerStateId;

	//各 state に自分(*this)を渡して生成
	states[static_cast<size_t>(Id::BaseMovement)]			= std::make_unique<BaseMovement>	(*this);
	states[static_cast<size_t>(Id::Dodge)]					= std::make_unique<DodgeState>		(*this);
	states[static_cast<size_t>(Id::Jump)]					= std::make_unique<JumpState>		(*this);
	states[static_cast<size_t>(Id::JumpAttack)]				= std::make_unique<JumpAttackState>	(*this);
	states[static_cast<size_t>(Id::Attack)]					= std::make_unique<AttackState>		(*this);
	states[static_cast<size_t>(Id::CounterDash)]			= std::make_unique<CounterDashState>(*this);
	states[static_cast<size_t>(Id::Ultimate)]				= std::make_unique<UltimateState>	(*this);
	states[static_cast<size_t>(Id::Damaged)]				= std::make_unique<DamagedState>	(*this);
	states[static_cast<size_t>(Id::Dead)]					= std::make_unique<DeadState>		(*this);
}

void Player::Update(float dt,float rawDt)
{
	UpdatePassive(dt);

#if defined(_DEBUG)
	DebugAdjustKatanaInput();
#endif

	if (current)
	{
		current->Update(dt);
	}

	if (slowMoTimer > 0.0f)
	{
		slowMoTimer -= Config::FrameRate::FIXED_DT;;
		if (slowMoTimer < 0.0f)
		{
			slowMoTimer = 0.0f;
		}
	}

	if (modelHandle >= 0)
	{
		MV1SetPosition(modelHandle, VGet(0.0f, 0.0f, 0.0f));
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, 0.0f, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));
	}
	
	animator.Update(dt);

	

	if (drawingKatana)
	{
		drawTimer += dt;
		if (drawTimer >= Config::Katana::DRAW_SWITCH_TIME)
		{
			katana.Unsheathe();   //このタイミングで腰→手へ
			drawingKatana = false;
		}
	}

	if (sheathingKatana)
	{
		sheatheTimer += dt;
		if (sheatheTimer >= Config::Katana::SHEATHE_SWITCH_TIME)
		{
			katana.Sheathe();  
			sheathingKatana= false;
		}
	}

	if (IsBladeActive())
	{
		if (!wasBladeActive)
		{
			katanaTrail.TriggerFlash();
		}

		const Capsule blade = katana.GetBladeCapsule();
		katanaTrail.Push(blade.p0, blade.p1);
		katanaTrail.Update(dt);
		wasBladeActive = true;
	}
	else
	{
		katanaTrail.Clear();  
		wasBladeActive = false;
	}

	if (modelHandle >= 0)
	{
		const float yawRad = (comp.facingYawDeg + modelYawOffsetDeg) * DX_PI_F / 180.0f;
		MV1SetPosition(modelHandle, comp.pos);
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, yawRad, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));
	}

	//ターゲット中は対象を向く
	if (target.IsLocked())
	{
		Enemy* t = target.GetTarget();
		if (t)
		{
			VECTOR toTarget = VSub(t->Comp().pos, comp.pos);
			toTarget.y = 0.0f;
			if (VSize(toTarget) > 1e-4f)
			{
				const float targetYaw = std::atan2(toTarget.x, toTarget.z) * 180.0f / DX_PI_F;
				//徐々に向く（急に向くと不自然なら補間）
				comp.facingYawDeg = targetYaw;   //または FaceTowardDeg で補間
			}
		}
	}

	katana.Update(modelHandle);

	const int waist = MV1SearchFrame(modelHandle, "mixamorig:Hips");
	sheath.Update(modelHandle, waist);
	//ApplyRootMotion();   
}

void Player::UpdatePassive(float dt)
{
	

	//HP回復(簡)
	if (DifficultyParam::HasHpRegen(ctx_.difficulty) && comp.hp > 0.0f)
	{
		comp.hp = std::min(comp.hp + Config::Player::Status::PLAYER_HP_REGEN, Config::Player::Status::PLAYER_HP_MAX);
	}

	const float regen = comp.enhanced ? Config::Player::Ult::ULT_GAUGE_REGEN_ENHANCED : Config::Player::Ult::ULT_GAUGE_REGEN;
	AddUltGauge(regen * dt);

	//スロータイム
	if (slowMoTimer > 0.0f)
	{
		slowMoTimer -= dt;
		if (slowMoTimer <= 0.0f)
		{
			slowMoTimer = 0.0f;
			comp.invincible = false;
		}
	}
}

void Player::ChangeState(PlayerStateId id)
{
	if (id == PlayerStateId::Count)
	{
		return;
	}

	PlayerState* next = states[static_cast<size_t>(id)].get();


	if (!next || next == current)
	{
		return;
	}
	current->OnExit();
	current = next;
	current->OnEnter();
}

void Player::PlayCutsceneWalk()
{
	comp.facingYawDeg = Config::Cutscene::PLAYER_WALK_YAW; 
	const int walk = ResourceManager::Instance().Model("anim_walk");
	if (walk >= 0)
	{
		PlayAnim(walk, 0, true, false);
	}
}

PlayerStateId  Player::CurrentStateId()const
{
	return current ? static_cast<PlayerStateId>(current->Id()) : PlayerStateId::Count;
}

Capsule Player::GetBodyCapsule()const
{
	return Capsule{ comp.CapsuleBottom(), comp.CapsuleTop(), comp.radius };
}

void  Player::TakeDamage(float amount, const VECTOR& attackerPos)
{
	if (comp.invincible)
	{
		return;   //無敵中（回避など）は無効
	}

	comp.hp -= amount;

	if (comp.hp < 0.0f)
	{
		comp.hp = 0.0f;
	}

	if (comp.hp <= 0.0f)
	{
		ChangeState(PlayerStateId::Dead);
		return;
	}


	//のけぞり方向＝攻撃元→プレイヤー（後ろに押される）
	VECTOR knock = VSub(comp.pos, attackerPos);
	knock.y = 0.0f;
	const float len = VSize(knock);
	if (len > 1e-4f)
	{
		knock = VScale(knock, 1.0f / len);
	}
	else
	{
		knock = VScale(comp.Forward(), -1.0f);   //真上から等は後ろへ
	}

	//UIの観測者に渡す
	health.NotifyHealthChanged(comp.hp, Config::Player::Status::PLAYER_HP_MAX, -amount);

	//Damaged状態に方向を渡して遷移
	auto* dmg = static_cast<DamagedState*>(GetState(PlayerStateId::Damaged));
	if (dmg) dmg->SetKnockbackDir(knock);
	ChangeState(PlayerStateId::Damaged);
}

void Player::AddUltGauge(float v)
{
	comp.ultGauge = std::clamp(comp.ultGauge + v, 0.0f, Config::Player::Ult::ULT_GAUGE_MAX);
}

void Player::TriggerJustDodgeSuccess()
{
	slowMoTimer = Config::Player::Evasion::SLOWMO_DURATION;
	comp.invincible = true;
	AddUltGauge(Config::Player::Ult::ULT_GAUGE_HIT);
	counterDashUsed_ = false;

	const int effect = ResourceManager::Instance().Effect("just_dodge");
	if (effect >= 0)
	{
		const VECTOR center = GetJustDodgeSphereCenter();   //体の中心
		EffectManager::Instance().Play(effect, center, Config::Effect::JUST_DODGE_SCALE);
	}
    //後でseとか入れといて
}

float Player::GetWorldTimeScale() const
{
	return (slowMoTimer > 0.0f) ? Config::Player::Evasion::SLOWMO_SCALE : 1.0f;
}

VECTOR Player::CalcMoveDirFromInput() const
{
	const float mx = input.GetMoveX(); //左スティックX（右が正）
	const float my = input.GetMoveY(); //左スティックY（上が正＝前）

	if (std::fabs(mx) < 1e-4f && std::fabs(my) < 1e-4f)
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}

	//カメラの yaw を基準にワールド方向へ変換
	const float yawRad		= camera.GetComponent().yaw * DX_PI_F / 180.0f;
	const VECTOR camFwd		= VGet(std::sin(yawRad), 0.0f, std::cos(yawRad));
	const VECTOR camRight	= VGet(std::cos(yawRad), 0.0f, -std::sin(yawRad));

	VECTOR dir = VAdd(VScale(camFwd, my), VScale(camRight, mx));

	const float len = VSize(dir);
	if (len > 1e-4f)
	{
		dir = VScale(dir, 1.0f / len);
	}
	return dir;
}

float Player::FloorYAt(const VECTOR& p) const
{
	float y = Config::Graund::GROUND_Y;
	if (stage.GetFloorY(p, y))
	{
		return y;
	}
	return Config::Graund::GROUND_Y;
}

void Player::FaceTowardDeg(float targetYawDeg, float dt)
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

	const float maxStep = Config::Player::Move::PLAYER_ROTATE_SPEED * dt;
	if (std::fabs(diff) <= maxStep)
	{
		comp.facingYawDeg = targetYawDeg;
	}
	else
	{
		comp.facingYawDeg += (diff > 0.0f) ? maxStep : -maxStep;
	}

	//-180 180 に正規化
	while (comp.facingYawDeg > 180.0f)
	{
		comp.facingYawDeg -= 360.0f;
	}

	while (comp.facingYawDeg < -180.0f)
	{
		comp.facingYawDeg += 360.0f;
	}
	
}

void Player::ApplyGravity(float dt)
{
	comp.vy		-= Config::Player::Status::GRAVITY * dt;
	comp.pos.y	+= comp.vy * dt;

	const float floorY = FloorYAt(comp.pos);

	if (comp.pos.y <= floorY)
	{
		comp.pos.y = floorY;
		comp.vy = 0.0f;
	}
}

void Player::ApplyHorizontalMove(const VECTOR& dir, float speed, float dt)
{
	comp.pos.x += dir.x * speed * dt;
	comp.pos.z += dir.z * speed * dt;
	comp.velocity = VScale(dir, speed);

	//移動後、壁にめり込んでいたら押し戻す
	comp.pos = stage.ResolveWall(comp.pos, comp.radius, comp.height);
	comp.pos = stage.ResolveDoorWall(comp.pos, comp.radius);
}

void Player::Draw() const
{
	if (hidden_)
	{
		return;
	}

	//モデルがあれば描く
	if (modelHandle >= 0)
	{
		MV1SetPosition(modelHandle, comp.pos);
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));

		//カメラとの距離で不透明度を決める（近いほど薄く）
		const VECTOR camPos = camera.GetEyePosition();
		const float d = VSize(VSub(comp.pos, camPos));

		//FADE_FAR より遠ければ不透明、FADE_NEAR より近ければ完全透明
		float alpha = 1.0f;
		if (d < Config::Camera::CAM_FADE_NEAR)
		{
			alpha = 0.0f;
		}
		else if (d < Config::Camera::CAM_FADE_FAR)
		{
			alpha = (d - Config::Camera::CAM_FADE_NEAR) / (Config::Camera::CAM_FADE_FAR - Config::Camera::CAM_FADE_NEAR);
		}

		if (alpha <= 0.01f)
		{
			return;   //完全に透明なら描かない
		}

		const int a = static_cast<int>(alpha * 255.0f);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);

		const float yawRad = (comp.facingYawDeg + modelYawOffsetDeg) * DX_PI_F / 180.0f;
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, yawRad, 0.0f));

		SetUseLighting(FALSE);
		if (IsSlowMoActive() && rimVS_ >= 0 && rimPS_ >= 0)
		{
			//ジャスト回避中：リムライトシェーダーで描く

			//リムライトのパラメータを定数バッファに書き込む
			if (rimCB_ >= 0)
			{
				float* p = static_cast<float*>(GetBufferShaderConstantBuffer(rimCB_));
				p[0] = 0.7f;//R 
				p[1] = 0.7f;//G
				p[2] = 1.2f;//B
				p[3] = 0.5f;//輪郭の光の強さ
				//g_RimParam（絞り, 有効, ベース発光, 予備）
				p[4] = 0.2f;   //リムの絞り（小さいほど広く光る）
				p[5] = 1.0f;   //有効フラグ
				p[6] = 0.0f;   //ベース発光（体全体を少し明るく）
				p[7] = 0.0f;	//プレイヤーが真っ黒になる
				UpdateShaderConstantBuffer(rimCB_);
				SetShaderConstantBuffer(rimCB_, DX_SHADERTYPE_PIXEL, 4);   //b4
			}

			MV1SetUseOrigShader(TRUE);
			SetUseVertexShader(rimVS_);
			SetUsePixelShader(rimPS_);

			SetUseLighting(FALSE);
			MV1DrawModel(modelHandle);
			SetUseLighting(TRUE);

			MV1SetUseOrigShader(FALSE);   //元に戻す
		}
		else
		{
			//通常時：普通に描く
			MV1DrawModel(modelHandle);
		}
		SetUseLighting(TRUE);

		
		katana.Draw();
		katanaTrail.Draw();
		sheath.Draw();
	}

#if defined(_DEBUG)

	{
		const VECTOR c = GetJustDodgeSphereCenter();
		const float  r = GetJustDodgeSphereRadius();

		//受付中（敵攻撃が球に入っている）は赤、通常は水色
		const unsigned int col = IsIncomingAttack()
			? GetColor(255, 80, 80)     //攻撃が来ている＝今回避すればジャスト
			: GetColor(80, 180, 255);   //通常

		DrawSphere3D(c, r, 16, col, GetColor(255, 255, 255), FALSE);
	}

	if (IsBladeActive())
	{
		const Capsule blade = GetBladeCapsule();
		DrawCapsule3D(blade.p0, blade.p1, blade.radius, 8, GetColor(255, 60, 60), GetColor(255, 255, 255), FALSE);
	}

	if (CheckHitKey(KEY_INPUT_B))
	{
		//色分け（既存）
		unsigned int col = GetColor(80, 200, 255);
		if (comp.enhanced)
		{
			col = GetColor(255, 90, 60);
		}
		else if (comp.invincible)
		{
			col = GetColor(255, 240, 120);
		}


		

		const unsigned int spec = GetColor(255, 255, 255);
		DrawCapsule3D(comp.CapsuleBottom(), comp.CapsuleTop(), comp.radius, 16, col, spec, TRUE);

		//向きの線
		const VECTOR center = VGet(comp.pos.x, comp.pos.y + comp.height * 0.5f, comp.pos.z);
		const VECTOR tip = VAdd(center, VScale(comp.Forward(), comp.radius * 2.5f));
		DrawLine3D(center, tip, GetColor(255, 255, 0));

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		
	}
	

	//デバッグ情報
	DrawFormatString(10, 60, GetColor(255, 255, 255),
		"Player State:%d  HP:%.0f/%.0f  Ult:%.0f  %s",
		static_cast<int>(CurrentStateId()),
		comp.hp, Config::Player::Status::PLAYER_HP_MAX, comp.ultGauge,
		comp.enhanced ? "[ENHANCED]" : "");
#endif
}

void Player::ToggleKatanaDraw()
{
	if (katana.IsDrawn())
	{
		//納刀開始：納刀アニメ再生。刀はまだ手のまま。
		const int sheathe = ResourceManager::Instance().Model("anim_sheathe");
		PlayAnim(sheathe, 0, false, false);
		sheathingKatana = true;
		sheatheTimer = 0.0f;
	}
	else
	{
		//抜刀開始：抜刀アニメ再生。刀はまだ腰のまま。
		const int draw = ResourceManager::Instance().Model("anim_draw");
		PlayAnim(draw, 0, false, false);
		drawingKatana = true;
		drawTimer = 0.0f;
	}
}

void Player::UpdateAnimOnly(float dt)
{
	if (modelHandle >= 0)
	{
		MV1SetPosition(modelHandle, VGet(0.0f, 0.0f, 0.0f));
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, 0.0f, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));
	}

	animator.Update(dt);

	if (modelHandle >= 0)
	{
		const float yawRad = comp.facingYawDeg * DX_PI_F / 180.0f;
		MV1SetPosition(modelHandle, comp.pos);
		MV1SetRotationXYZ(modelHandle, VGet(0.0f, yawRad, 0.0f));
		MV1SetScale(modelHandle, VGet(modelScale, modelScale, modelScale));
	}

	//刀と鞘の追従（これが無いとカットシーン中に置いて行かれる）
	katana.Update(modelHandle);

	const int waist = MV1SearchFrame(modelHandle, "mixamorig:Hips");
	sheath.Update(modelHandle, waist);
}

void Player::PlayCutsceneIdle()
{
	const int idle = ResourceManager::Instance().Model("anim_idle");
	if (idle >= 0)
	{
		PlayAnim(idle, 0, true, false);
	}
}

bool Player::ShouldStartAwaken() const
{
	//HPが閾値以下で、まだ覚醒演出を発動していない、生きている
	return !awakenTriggered
		&& comp.hp > 0.0f
		&& comp.HpRate() <= Config::Player::Status::PLAYER_ENHANCE_THRESHOLD;
}

void Player::EnterEnhanced()
{
	awakenTriggered = true;
	comp.enhanced = true;   //ここで強化状態に
}

float Player::GetCurrentAttackPower() const
{
	float power;
	if (CurrentStateId() == PlayerStateId::JumpAttack)
	{
		power = Config::Player::Attack::JUMP_ATTACK_POWER;
	}
	else
	{
		switch (comp.comboIndex)
		{
		case 0:  power = Config::Player::Attack::ATTACK_POWER_1; break;
		case 1:  power = Config::Player::Attack::ATTACK_POWER_2; break;
		case 2:  power = Config::Player::Attack::ATTACK_POWER_3; break;
		default: power = Config::Player::Attack::ATTACK_POWER_1; break;
		}
	}

	//強化状態ならダメージアップ
	if (comp.enhanced)
	{
		power *= Config::Player::Attack::ENHANCED_DAMAGE_MULT;   
	}

	return power;
}

float Player::GetCurrentHitEffectScale() const
{
	//落下攻撃なら専用のスケール
	if (CurrentStateId() == PlayerStateId::JumpAttack)
	{
		return Config::Effect::JUMP_ATTACK_HIT_SCALE;  
	}

	
	switch (comp.comboIndex)
	{
	case 0:  return Config::Effect::HIT_SCALE_1;
	case 1:  return Config::Effect::HIT_SCALE_2;
	case 2:  return Config::Effect::HIT_SCALE_3;
	default: return Config::Effect::HIT_SCALE_1;
	}
}


#if defined(_DEBUG)
void Player::DebugAdjustKatanaInput()
{
	//Nキーで調整モードのトグル
	static bool prevN = false;
	const bool nowN = (CheckHitKey(KEY_INPUT_N) != 0);
	if (nowN && !prevN)
	{
		katanaAdjustMode = !katanaAdjustMode;
		if (katanaAdjustMode && !IsKatanaDrawn()) ToggleKatanaDraw();   //抜刀して見えるように
	}
	prevN = nowN;

	if (!katanaAdjustMode) return;

	//矢印キー：位置
	const float moveStep = 1.0f;
	if (CheckHitKey(KEY_INPUT_LEFT))  dbgKatanaOffset.x -= moveStep;
	if (CheckHitKey(KEY_INPUT_RIGHT)) dbgKatanaOffset.x += moveStep;
	if (CheckHitKey(KEY_INPUT_UP))    dbgKatanaOffset.y += moveStep;
	if (CheckHitKey(KEY_INPUT_DOWN))  dbgKatanaOffset.y -= moveStep;
	if (CheckHitKey(KEY_INPUT_PGUP))  dbgKatanaOffset.z += moveStep;
	if (CheckHitKey(KEY_INPUT_PGDN))  dbgKatanaOffset.z -= moveStep;

	//テンキー：回転
	const float rotStep = 0.02f;
	if (CheckHitKey(KEY_INPUT_NUMPAD8)) dbgKatanaPitch += rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD2)) dbgKatanaPitch -= rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD4)) dbgKatanaYaw += rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD6)) dbgKatanaYaw -= rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD7)) dbgKatanaRoll += rotStep;
	if (CheckHitKey(KEY_INPUT_NUMPAD9)) dbgKatanaRoll -= rotStep;

	//調整値でMATRIXを作って、刀の手オフセットに適用
	MATRIX rot = MMult(MMult(
		MGetRotX(dbgKatanaPitch),
		MGetRotY(dbgKatanaYaw)),
		MGetRotZ(dbgKatanaRoll));
	MATRIX tr = MGetTranslate(dbgKatanaOffset);
	katana.SetHandOffset(MMult(rot, tr));
}

void Player::DebugDrawKatanaAdjust() const
{
	if (!katanaAdjustMode) return;

	DrawFormatString(10, 500, GetColor(0, 255, 255),
		"KATANA ADJUST (N:toggle) arrows:XY PgUp/Dn:Z NUM:rot");
	DrawFormatString(10, 520, GetColor(0, 255, 255),
		"OFFSET X:%.1f Y:%.1f Z:%.1f", dbgKatanaOffset.x, dbgKatanaOffset.y, dbgKatanaOffset.z);
	DrawFormatString(10, 540, GetColor(0, 255, 255),
		"PITCH:%.3f YAW:%.3f ROLL:%.3f", dbgKatanaPitch, dbgKatanaYaw, dbgKatanaRoll);
}
#endif