#pragma once
#include "InputDevice.h"
#include "KeyboardDevice.h"
#include "GamepadDevice.h"

/// <summary>
/// 操作関係のシステムクラス
/// </summary>
class InputSystem
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	InputSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~InputSystem() = default;

	//コピー禁止
	InputSystem(const InputSystem&)				= delete;
	InputSystem& operator=(const InputSystem&)	= delete;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// どちらかのデバイスで押されっぱなしか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	bool IsHeld(InputAction action)const;

	/// <summary>
	/// どちらかのデバイスで押した瞬間か
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	bool IsPressed(InputAction action)const;

	/// <summary>
	/// どちらかのデバイスで離した瞬間か
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	bool IsReleased(InputAction action)const;

	/// <summary>
	/// 移動入力の水平成分
	/// </summary>
	float GetMoveX()const;

	/// <summary>
	/// 移動入力の垂直成分
	/// </summary>
	float GetMoveY()const;

	/// <summary>
	/// カメラ入力の水平成分
	/// </summary>
	float GetCamX()const;

	/// <summary>
	/// カメラ入力の垂直成分
	/// </summary>
	float GetCamY()const;

	/// <summary>
	/// パッドを直接取得する
	/// </summary>
	GamepadDevice& Pad() { return *pad; }
private:
	std::unique_ptr<KeyboardDevice> keyboard;	//キーボード入力
	std::unique_ptr<GamepadDevice>  pad;		//ゲームパッド入力

	/// <summary>
	/// パッドが接続されているか
	/// </summary>
	bool IsPadConnected() const;
};