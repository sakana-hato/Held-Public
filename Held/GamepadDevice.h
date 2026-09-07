#pragma once
#include "InputDevice.h"
#include "Precompiled.h"

/// ボタンマッピング:
///   A             … ジャンプ / 決定
///   B             … 回避 兼　攻撃
///   X             … 攻撃
///   Y             … 魔法
///   RB            … 必殺技
///   LB            … ターゲットロック
///   RS（押し込み）… ターゲット切り替え
///   LS（押し込み）… ダッシュ
///   Start         … ポーズ
///   Back          … カメラ切り替え
///   左スティック  … 移動
///   右スティック  … カメラ
/// 予定

/// <summary>
/// コントロール入力クラス
/// </summary>
class GamepadDevice final : public InputDevice
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GamepadDevice();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GamepadDevice() = default;

	/// <summary>
	/// パッドの入力状態を更新する
	/// </summary>
	virtual void Update()override;

	/// <summary>
	/// 押されっぱなしかどうか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	virtual bool IsHeld(InputAction action)const override;

	/// <summary>
	/// 押した瞬間かどうか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	virtual bool IsPressed(InputAction action)const override;

	/// <summary>
	/// 離した瞬間かどうか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	virtual bool IsReleased(InputAction action)const override;

	/// <summary>
	/// 左スティックの水平入力
	/// </summary>
	virtual float GetAxisX()const override;

	/// <summary>
	/// 左スティックの垂直入力
	/// </summary>
	virtual float GetAxisY()const override;

	/// <summary>
	/// 右スティックの水平入力
	/// </summary>
	virtual float GetCamAxisX()const override;

	/// <summary>
	/// 右スティックの垂直入力
	/// </summary>
	virtual float GetCamAxisY()const override;

	/// <summary>
	/// 振動を開始する
	/// </summary>
	/// <param name="leftPower"> </param>左モーターの強さ
	/// <param name="rightPower"></param>右モーターの強さ
	void SetVibration(int leftPower, int rightPower);

	/// <summary>
	/// 振動を停止する
	/// </summary>
	void StopVibration();

private:
	XINPUT_STATE curr = {};		// 今フレームの入力状態
	XINPUT_STATE prev = {};		// 前フレームの入力状態

	static constexpr float STICK_DEADZONE = 0.15f; // スティックの不感帯（ドリフト防止）
	static constexpr float STICK_MAX_RAW = 32768.0f; // スティックの生値を -1.0 1.0 に正規化

	// XInput ボタンビット のマッピングテーブル
	std::array<int, static_cast<size_t>(InputAction::Count)> buttonMap = {};

	/// <summary>
	/// アクションとボタンの対応表を作る
	/// </summary>
	void BuildButtonMap();

	//前後のフレームを比べて押下状態を判定する
	bool BtnHeld(int btn) const		{return curr.Buttons[btn] != 0;}
	bool BtnPressed(int btn) const	{return curr.Buttons[btn] != 0 && prev.Buttons[btn] == 0;}
	bool BtnReleased(int btn) const {return curr.Buttons[btn] == 0 && prev.Buttons[btn] != 0;}

	/// <summary>
	/// デットゾーンを適用する
	/// </summary>
	/// <param name="raw"></param>正規化
	/// <returns></returns>デットゾーン処理後の値
	float ApplyDeadzone(float raw) const;
};