#define  NOMINMAX
#include "DxLib.h"
#include "Precompiled.h"
#include "Config.h"
#include "Stage.h"
#include "CameraSystem.h"

CameraSystem::CameraSystem()
{
	cam.yaw			= Config::Camera::CAM_YAW;
	cam.pitch		= Config::Camera::CAM_PITCH;
	cam.distance	= Config::Camera::CAM_DISTANCE;
	cam.fov			= Config::Camera::CAM_FOV;
}

void CameraSystem::Init(float nearClip, float farClip)
{
	_nearClip	= nearClip;
	_farClip	= farClip;

	//カメラのnear farをセットする
	SetCameraNearFar(_nearClip, _farClip);

	//遠近法カメラをセットアップする
	SetupCamera_Perspective(Deg2Rad(cam.fov));

	//マウスカーソルの位置を取得する
	GetMousePoint(&prevMouseX, &prevMouseY);

	mouseInitialized = true;
}

void CameraSystem::Update(float dt, const VECTOR& playerPos, const VECTOR& playerDir)
{
	//F1でデバッグカメラを切り替える
	static bool debugPrev = false;
	const bool debugNow = (CheckHitKey(KEY_INPUT_F1) != 0);
	if (debugNow && debugPrev)
	{
		_mode = (_mode == CameraMode::Debug) ? CameraMode::ThirdPerson : CameraMode::Debug;
	}
	debugPrev = debugNow;

	UpdateShake(dt);

	//各モードのupdateを使用する
	switch (_mode)
	{
	case CameraMode::ThirdPerson:
		UpdateThirdPerson(dt, playerPos, playerDir);
		break;
	case CameraMode::FirstPerson:
		UpdateFirstPerson(dt, playerPos, playerDir);
		break;
	case CameraMode::Debug:
		UpdateDebug(dt);
		break;
	case CameraMode::Cutscene:
		break;
	default:
		break;
	}
}

void CameraSystem::Apply()const
{
	SetCameraNearFar(_nearClip, _farClip);
	SetupCamera_Perspective(Deg2Rad(cam.fov));

	//各モードのapplyを使用する
	switch (_mode)
	{
	case CameraMode::ThirdPerson:
		ApplyOrbit(lastTargetPos);
		break;
	case CameraMode::FirstPerson:
		ApplyFirstPerson(lastTargetPos, lastPlayerDir);
		break;
	case CameraMode::Debug:
		ApplyDebug();
		break;
	case CameraMode::Cutscene:
		ApplyCutscene();
		break;
	}
}

void CameraSystem::SetCutsceneCamera(const VECTOR& pos, const VECTOR& target)
{
	cutscenePos		= pos;
	cutsceneTarget	= target;
	_mode			= CameraMode::Cutscene;
}

void CameraSystem::UpdateThirdPerson(float dt, const VECTOR& playerPos, const VECTOR& playerDir)
{
	_dt = dt;

	if (!followInit)
	{
		followTarget	= playerPos;   
		followInit		= true;
	}
	else
	{
		// 指数補間で追従する
		const float  followRate = 1.0f - std::exp(-followLerp * dt);
		followTarget			= VAdd(followTarget, VScale(VSub(playerPos, followTarget), followRate));
	}

	lastTargetPos = followTarget;   
	lastPlayerDir = playerDir;

	if (lockOn)
	{
		VECTOR toTarget = VSub(lockOnTargetPos, followTarget);	// 注視点からロックオン対象へのベクトル
		toTarget.y		= 0.0f;									// 水平角だけ扱うので高さは捨てる

		if (VSize(toTarget) > 1e-4f)
		{
			
			const float targetYaw	= std::atan2(toTarget.x, toTarget.z) * 180.0f / DX_PI_F;	// 対象方向のyaw（度）
			float diff				= targetYaw - cam.yaw;										// 角度差

			while (diff > 180.0f)
			{
				diff -= 360.0f;
			}

			while (diff < -180.0f)
			{
				diff += 360.0f;
			}

			const float lockOnRate	= 1.0f - std::exp(-Config::Camera::LOCKON_LERP * dt);// ロックオンしたときのカメラの向く割合
			cam.yaw					+= diff * lockOnRate;
		}
	}

	ApplyLook(dt);
}

void CameraSystem::UpdateFirstPerson(float dt, const VECTOR& playerPos, const VECTOR& playerDir)
{
	lastTargetPos = playerPos;
	lastPlayerDir = playerDir;

	ApplyLook(dt);
}

void CameraSystem::ApplyLook(float dt)
{
	// 右スティック（レート入力なので dt を掛ける
	cam.yaw		+= GetCamAxisX() * stickSens * dt;
	cam.pitch	-= GetCamAxisY() * stickSens * dt;

	//マウス
	int mx, my;	// 現在のカーソル座標
	GetMousePoint(&mx, &my);

	if (mouseInitialized)
	{
		const float dx = static_cast<float>(mx - prevMouseX);	// 前フレームからの横移動量
		const float dy = static_cast<float>(my - prevMouseY);	// 前フレームからの縦移動量

		cam.yaw		  += dx * mouseSens;	// 左右反転したいときは符号を反転
		cam.pitch	  += dy * mouseSens;	// 上下反転したいときは符号を反転
	}

	// 次フレーム用にカーソルを画面中央へ戻す
	const int cx = Config::Window::WINDOW_W / 2;// 画面中央X
	const int cy = Config::Window::WINDOW_H / 2;// 画面中央Y
	SetMousePoint(cx, cy);

	prevMouseX			= cx;
	prevMouseY			= cy;
	mouseInitialized	= true;
	cam.ClampPitch();
}

void CameraSystem::UpdateDebug(float dt)
{
	int mx, my;
	GetMousePoint(&mx, &my);

	if (mouseInitialized)
	{
		const float dx = static_cast<float>(mx - prevMouseX);
		const float dy = static_cast<float>(my - prevMouseY);

			debugYaw	+= dx * mouseSens;
			debugPitch	-= dy * mouseSens;
			debugPitch	= std::clamp(debugPitch, -89.0f, 89.0f);
	}
	prevMouseX = mx;
	prevMouseY = my;

#if defined(_DEBUG)
	MoveDebugCamera(dt);
#endif
}

void CameraSystem::ApplyOrbit(const VECTOR& targetPos)const
{

	const float yaw		= Deg2Rad(cam.yaw);
	const float pitch	= Deg2Rad(cam.pitch);

	const float wantDist = std::max(cam.distance, 0.01f);   //壁補正前の距離

	//注視点から見たカメラ方向の単位ベクトル
	const MATRIX rotX		= MGetRotX(pitch);
	const MATRIX rotY		= MGetRotY(yaw);
	const VECTOR dirLocal	= VTransform(VTransform(VGet(0.0f, 0.0f, -1.0f), rotX), rotY);
	const VECTOR pivot		= VGet(targetPos.x, targetPos.y + Config::Camera::CAM_PIVOT_UP, targetPos.z);//壁判定の起点
	float targetDist		= wantDist;//補正後に目指す距離

	if (stage)
	{
		const VECTOR idealEye = VAdd(targetPos, VScale(dirLocal, wantDist));								//障害物が無い場合のカメラ位置

		const VECTOR clamped = stage->ClampCameraByWall(pivot, idealEye, Config::Camera::CAM_WALL_MARGIN);	//壁に押し戻された後のカメラ位置

		targetDist = VSize(VSub(clamped, targetPos));

		if (targetDist > wantDist)
		{
			targetDist = wantDist;
		}

		constexpr float CAM_DIST_MIN = 100.0f;  //寄りすぎてプレイヤーにめり込むのを防ぐ下限
		if (targetDist < CAM_DIST_MIN)
		{
			targetDist = CAM_DIST_MIN;
		}
	}

	const float distRate = 1.0f - std::exp(-distLerp * _dt);// 壁際でカクつかないよう距離を補間する
	curDist				+= (targetDist - curDist) * distRate;

	 VECTOR lookAt = VGet(targetPos.x, targetPos.y + Config::Camera::CAM_LOOK_UP, targetPos.z);// 実際に見る点

	//補間後の距離でカメラ位置を決める
	VECTOR eye = VAdd(lookAt, VScale(dirLocal, curDist));// カメラ位置

	if (stage)
	{
		float camFloorY = 0.0f;	// カメラ真下の床の高さ
		if (stage->GetFloorY(eye, camFloorY))
		{
			const float minY = camFloorY + Config::Camera::CAM_GROUND_CLEARANCE;// 床の裏側が見えないようにする最低の高さ
			if (eye.y < minY)
			{
				eye.y = minY;   //床の少し上に持ち上げる
			}
		}
	}
		// 視線の水平方向
		VECTOR fwd = VGet(dirLocal.x, 0.0f, dirLocal.z);				// カメラ方向の水平成分
		if (VSize(fwd) > 0.0001f)
		{
			fwd					= VNorm(fwd);
			VECTOR right		= VCross(VGet(0.0f, 1.0f, 0.0f), fwd);	// カメラの右方向 = 上 × 前
			right				= VNorm(right);
			const VECTOR shift	= VScale(right, Config::Camera::SHOULDER_OFFSET);// 横へのずらし量
			eye					= VAdd(eye, shift);
		}
	
	eyePos = eye;
	
	if (shakeStrength > 0.0f)
	{
		//カメラシェイク時に加える力
		const float rx	= (GetRand(2000) / 1000.0f - 1.0f) * shakeStrength;
		const float ry	= (GetRand(2000) / 1000.0f - 1.0f) * shakeStrength;
		const float rz	= (GetRand(2000) / 1000.0f - 1.0f) * shakeStrength;
		eye				= VAdd(eye, VGet(rx, ry, rz));
	}

	SetCameraPositionAndTarget_UpVecY(eye, lookAt);
}

void CameraSystem::ApplyFirstPerson(const VECTOR& playerPos, const VECTOR& playerDir)const
{
	constexpr float HEAD_OFFSET_Y	= 160.0f;// 足元から目線までの高さ
	const VECTOR headPos			= VGet(playerPos.x,playerPos.y + HEAD_OFFSET_Y,playerPos.z);// カメラを置く頭の位置

	// yaw / pitch から前方ベクトルを算出
	const float yaw			= Deg2Rad(cam.yaw);
	const float pitch		= Deg2Rad(cam.pitch);

	const VECTOR forward	= VGet(std::cos(pitch) * std::sin(yaw),std::sin(pitch),std::cos(pitch) * std::cos(yaw));//yaw/pitchから求めた前方向
	const VECTOR target		= VAdd(headPos, forward);//注視点
	SetCameraPositionAndTarget_UpVecY(headPos, target);
}

void CameraSystem::ApplyDebug()const
{
	const float yaw			= Deg2Rad(debugYaw);
	const float pitch		= Deg2Rad(debugPitch);

	const VECTOR forward	= VGet(std::cos(pitch) * std::sin(yaw), std::sin(pitch), std::cos(pitch) * std::cos(yaw));
	const VECTOR target		= VAdd(debugPos, forward);

	SetCameraPositionAndTarget_UpVecY(debugPos, target);
}

void CameraSystem::ApplyCutscene() const
{
	VECTOR eye = cutscenePos;

	if (shakeStrength > 0.0f)
	{
		const float rx = (GetRand(1000) / 1000.0f - 1.0f) * shakeStrength;
		const float ry = (GetRand(1000) / 1000.0f - 1.0f) * shakeStrength;
		const float rz = (GetRand(1000) / 1000.0f - 1.0f) * shakeStrength;
		eye = VAdd(eye, VGet(rx, ry, rz));
	}

	SetCameraPositionAndTarget_UpVecY(eye, cutsceneTarget);
}

void CameraSystem::AddShake(float strength)
{
	shakeStrength += strength;
	if (shakeStrength > Config::Camera::SHAKE_MAX)
	{
		shakeStrength = Config::Camera::SHAKE_MAX;
	}
}

void CameraSystem::UpdateShake(float dt)
{
	if (shakeStrength > 0.0f)
	{
		shakeStrength -= Config::Camera::SHAKE_DECAY * dt;
		if (shakeStrength < 0.0f)
		{
			shakeStrength = 0.0f;
		}
	}
}

float CameraSystem::GetCamAxisX()const
{
	XINPUT_STATE pad = {};
	GetJoypadXInputState(DX_INPUT_PAD1, &pad);
	const float axisX		= static_cast<float>(pad.ThumbRX) / 32768.0f;	// -1.0から1.0に正規化
	constexpr float DEAD	= 0.15f;										// スティックのあそび
	return (std::fabs(axisX) > DEAD) ? axisX : 0.0f;
}

float CameraSystem::GetCamAxisY() const
{
	XINPUT_STATE pad = {};
	GetJoypadXInputState(DX_INPUT_PAD1, &pad);
	const float axisY		= static_cast<float>(pad.ThumbRY) / 32768.0f;	// -1.0から1.0に正規化
	constexpr float DEAD	= 0.15f;										// スティックのあそび
	return (std::fabs(axisY) > DEAD) ? axisY : 0.0f;
}

#if defined(_DEBUG)
void CameraSystem::MoveDebugCamera(float dt)
{
	// Shiftで高速移動
	const float speed = (CheckHitKey(KEY_INPUT_LSHIFT))
		? debugMoveSpeed * debugFastMult
		: debugMoveSpeed;

	// yawからカメラの前後左右ベクトルを算出
	const float yaw		= Deg2Rad(debugYaw);
	const VECTOR fwd	= VGet(std::sin(yaw), 0.0f, std::cos(yaw));
	const VECTOR right	= VGet(std::cos(yaw), 0.0f, -std::sin(yaw));
	const VECTOR up		= VGet(0.0f, 1.0f, 0.0f);

	if (CheckHitKey(KEY_INPUT_UP)) debugPos = VAdd(debugPos, VScale(fwd, speed * dt));
	if (CheckHitKey(KEY_INPUT_DOWN)) debugPos = VAdd(debugPos, VScale(fwd, -speed * dt));
	if (CheckHitKey(KEY_INPUT_RIGHT)) debugPos = VAdd(debugPos, VScale(right, speed * dt));
	if (CheckHitKey(KEY_INPUT_LEFT)) debugPos = VAdd(debugPos, VScale(right, -speed * dt));
	if (CheckHitKey(KEY_INPUT_E)) debugPos = VAdd(debugPos, VScale(up, speed * dt));
	if (CheckHitKey(KEY_INPUT_Q)) debugPos = VAdd(debugPos, VScale(up, -speed * dt));
}
#endif