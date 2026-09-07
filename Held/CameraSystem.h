#pragma once
#include "CameraComponent.h"
#include "CameraMode.h"
#include "Config.h"

class Stage;//前方宣言

/// <summary>
/// カメラシステム
/// </summary>
class CameraSystem
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CameraSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CameraSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="nearClip"></param>カメラの最低描画距離
	/// <param name="farClip"> </param>カメラの最大描画距離
	void Init(float nearClip = Config::Camera::CAM_NEAR, float farClip = Config::Camera::CAM_FAR);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">       </param>デルタタイム
	/// <param name="playerPos"></param>プレイヤー中心座標
	/// <param name="playerDir"></param>プレイヤー正面方向
	void Update(float dt, const VECTOR& playerPos, const VECTOR& playerDir);

	/// <summary>
	/// カメラシェイク
	/// </summary>
	/// <param name="strength"></param>強さ
	void AddShake(float strength);

	/// <summary>
	/// カメラシェイクを更新する
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void UpdateShake(float dt);

	/// <summary>
	/// カメラに適応させる
	/// </summary>
	void Apply()const;

	/// <summary>
	/// 使用するカメラをセットする
	/// </summary>
	/// <param name="mode"></param>使用するカメラモード
	void SetMode(CameraMode mode) { _mode = mode; }

	/// <summary>
	/// 現在のカメラモードを取得する
	/// </summary>
	/// <returns></returns>現在のカメラモード
	CameraMode GetMode()const { return _mode; }

	/// <summary>
	/// カットシーン用のカメラをセットする
	/// </summary>
	/// <param name="pos">		</param>カメラの座標
	/// <param name="target">	</param>ターゲット
	void SetCutsceneCamera(const VECTOR& pos, const VECTOR& target);

	/// <summary>
	/// ロックオンする用にカメラをセットする
	/// </summary>
	/// <param name="active">	</param>使用するか
	/// <param name="targetPos"></param>ターゲットの座標
	void SetLockOnTarget(bool active, const VECTOR& targetPos){lockOn = active;lockOnTargetPos = targetPos;}

	/// <summary>
	/// カメラのコンポーネントを取得する（参照用）
	/// </summary>
	/// <returns></returns>読み取り専用のカメラコンポーネント
	const CameraComponent& GetComponent()const { return cam; }

	/// <summary>
	/// カメラのコンポーネントを取得する（書き換え用）
	/// </summary>
	/// <returns></returns>値を直接変更できるカメラコンポーネント
	CameraComponent& GetComponent() { return cam; }

	/// <summary>
	/// コントローラのカメラ感度をセットする
	/// </summary>
	/// <param name="s"></param>感度
	void SetStickSensitivity(float sen) { stickSens = sen; }

	/// <summary>
	/// マウスのカメラ感度をセットする
	/// </summary>
	/// <param name="sen"></param>感度
	void SetMouseSensitivity(float sen) { mouseSens = sen; }

	/// <summary>
	/// 壁と床の判定に使用するステージをセットする
	/// </summary>
	/// <param name="sta"></param>判定に使用するステージ
	void SetStage(const Stage* sta)	  { stage = sta; }

	/// <summary>
	/// 最後に適用されたカメラ位置を取得する
	/// </summary>
	VECTOR GetEyePosition() const { return eyePos; }

private:
	/// <summary>
	/// 三人称カメラの更新
	/// </summary>
	/// <param name="dt">		</param>デルタタイム
	/// <param name="playerPos"></param>プレイヤー中心座標
	/// <param name="playerDir"></param>プレイヤー正面方向
	void UpdateThirdPerson(float dt, const VECTOR& playerPos, const VECTOR& playerDir);

	/// <summary>
	/// 一人称カメラの更新
	/// </summary>
	/// <param name="dt">		</param>デルタタイム
	/// <param name="playerPos"></param>プレイヤー中心座標
	/// <param name="playerDir"></param>プレイヤー正面方向
	void UpdateFirstPerson(float dt, const VECTOR& playerPos, const VECTOR& playerDir);

	/// <summary>
	/// デバッグカメラの更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void UpdateDebug(float dt);
	
	/// <summary>
	/// 視点入力を回転角に反映する
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void ApplyLook(float dt);

	/// <summary>
	/// 三人称カメラを適用する
	/// </summary>
	/// <param name="targetPos"></param>カメラが回り込む中心となる注視点
	void ApplyOrbit(const VECTOR& targetPos) const;

	// <summary>
	/// 一人称カメラを適用する
	/// </summary>
	/// <param name="playerPos"></param>プレイヤー中心座標
	/// <param name="playerDir"></param>プレイヤー正面方向
	void ApplyFirstPerson(const VECTOR& playerPos, const VECTOR& playerDir) const;

	/// <summary>
	/// デバッグカメラを適用する
	/// </summary>
	void ApplyDebug() const;

	/// <summary>
	/// カットシーンカメラを適用する
	/// </summary>
	void ApplyCutscene() const;

	/// <summary>
	/// 右スティックの水平入力を取得する
	/// </summary>
	/// <returns></returns>-1.0 1.0の入力値
	float GetCamAxisX()const;

	/// <summary>
	/// 右スティックの垂直入力を取得する
	/// </summary>
	/// <returns></returns>-1.0 1.0の入力値
	float GetCamAxisY()const;

	/// <summary>
	/// 度をラジアンに変換する
	/// </summary>
	/// <param name="deg"></param>角度（度）
	/// <returns></returns>角度（ラジアン）
	static float Deg2Rad(float deg) { return deg * DX_PI_F / 180.0f; }

	const Stage* stage = nullptr;  // 壁と床の判定に使うステージ


	CameraComponent cam;						// カメラの姿勢データ
	CameraMode _mode = CameraMode::ThirdPerson;	// 現在のカメラモード

	float _nearClip = Config::Camera::CAM_NEAR;	// カメラのnear
	float _farClip	= Config::Camera::CAM_FAR;	// カメラのfar

	float stickSens = 90.0f;					// コントローラ時のカメラ感度
	float mouseSens = 0.08f;					// マウス時のカメラ感度

	float distLerp			= 1.0f;								// 距離補間の速さ
	mutable VECTOR eyePos	= {};								// 最後に適用したカメラ座標
	mutable float  curDist	= Config::Camera::CAM_DISTANCE;		// 壁補正込みの現在距離
	mutable float _dt		= 1.0f / 60.0f;						// ApplyへUpdateのdtを渡すための退避先

	//デバック用カメラ設定
	VECTOR debugPos			= { 0.0f, 200.0f, -500.0f };	// デバッグカメラの座標
	float  debugYaw			= 0.0f;							// デバッグカメラの水平回転角
	float  debugPitch		= 10.0f;						// デバッグカメラの垂直回転角
	float  debugMoveSpeed	= 300.0f;						// 移動速度
	float  debugFastMult	= 4.0f;							// Shift押下時の速度倍率

	//三人称の注視点追従
	VECTOR followTarget = {};		// 実際にカメラが見る注視点
	float  followLerp	= 6.0f;		// 追従の速さ
	bool   followInit	= false;	// 初回フラグ

	//カットシーン
	VECTOR cutscenePos		= {};	// カットシーン時のカメラ座標
	VECTOR cutsceneTarget	= {};	// カットシーン時の注視点

	//Updateの結果をApplyへ引き渡すための保存領域
	VECTOR lastTargetPos = {};					// 最後に計算した注視点
	VECTOR lastPlayerDir = { 0.0f, 0.0f, 1.0f };// 最後に受け取ったプレイヤー正面方向

	//ロックオン
	VECTOR lockOnTargetPos	= {};			// ロックオン中かどうか
	bool   lockOn			= false;		// ロックオン対象の座標

	//マウス差分の算出用
	int prevMouseX			= 0;	// 前フレームのカーソルX
	int prevMouseY			= 0;	// 前フレームのカーソルY
	bool mouseInitialized	= false;// 初回の巨大な差分を無視するためのフラグ

	float shakeStrength		= 0.0f;	// 現在のシェイク強度

#if defined(_DEBUG)
	/// <summary>
	/// キー入力でデバッグカメラを移動させる
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void MoveDebugCamera(float dt);
#endif
};