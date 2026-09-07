#pragma once
#include "InputDevice.h"
#include "Precompiled.h"

/// キーマッピング:
///   WASD          … 移動
///   方向キー      … カメラ
///   Z             … 攻撃
///   X             … 魔法
///   C             … 必殺技
///   Space         … ジャンプ
///   Shift         … ダッシュ
///   Alt           … 回避
///   F             … ターゲットロック
///   R             … ターゲット切り替え
///   V             … カメラ切り替え
///   Escape        … ポーズ
///   Enter         … 決定
///   Backspace     … キャンセル

//キーボード入力クラス
class KeyboardDevice final :public InputDevice
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	KeyboardDevice();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~KeyboardDevice() = default;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	bool IsHeld(InputAction action)const override;
	bool IsPressed(InputAction action)const override;
	bool IsReleased(InputAction action)const override;
    
	float GetAxisX()const override;
	float GetAxisY()const override;
	float GetCamAxisX()const override;
	float GetCamAxisY()const override;

private:
	static constexpr int KEY_NUM = 256;

	//キー入力バッファ
	std::array<char, KEY_NUM>curr = {};//0離し
	std::array<char, KEY_NUM>prev = {};//1押し

	int mouseCurr = 0;  
	int mousePrev = 0; 

	//DxLibキーマップ
	std::array<int, static_cast<size_t>(InputAction::Count)>keyMap = {};

	void BuildKeyMap();

	bool KeyHeld(int keyCode)const { return curr[keyCode] != 0; }
	bool KeyPressed(int keyCode)const { return curr[keyCode] != 0 && prev[keyCode] == 0; }
	bool KeyReleased(int keyCode)const { return curr[keyCode] == 0 && prev[keyCode] != 0; }

};