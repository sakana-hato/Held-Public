#pragma once
#include "InputAction.h"
#include "Precompiled.h"

/// <summary>
/// 入力デバイス抽象基底クラス（Strategy Pattern）キーボードとゲームパッドで実装を差し替える。
/// </summary>
class InputDevice
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~InputDevice() = default;

	/// <summary>
	/// 入力状態を更新する
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 押されっぱなしかどうか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	virtual bool IsHeld(InputAction action)const = 0;

	/// <summary>
	/// 押した瞬間かどうか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	virtual bool IsPressed(InputAction action)const = 0;

	/// <summary>
	/// 離した瞬間かどうか
	/// </summary>
	/// <param name="action"></param>調べたいアクション
	virtual bool IsReleased(InputAction action)const = 0;

	/// <summary>
	/// 移動入力の水平成分
	/// </summary>
	virtual float GetAxisX()const = 0;

	/// <summary>
	/// 移動入力の垂直成分
	/// </summary>
	virtual float GetAxisY()const = 0;

	/// <summary>
	/// カメラ入力の水平成分
	/// </summary>
	virtual float GetCamAxisX()const = 0;

	/// <summary>
	/// カメラ入力の垂直成分
	/// </summary>
	virtual float GetCamAxisY()const = 0;
};
