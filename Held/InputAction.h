#pragma once
#include "Precompiled.h"

/// <summary>
/// ゲームの操作アクション一覧を作成する
/// </summary>
enum class InputAction : uint32_t
{
	//移動
	MoveForward,	//前進
	MoveBack,		//後進
	MoveLeft,		//右移動
	MoveRight,		//左移動

	//カメラ
	CamUp,			//カメラ上
	CamDown,		//カメラ下
	CamLeft,		//カメラ右
	CamRight,		//カメラ左

	//アクション
	Attack,			//通常攻撃
	Magic,			//魔法
	Ultimate,		//必殺技
	Jump,			//ジャンプ
	Dash,			//ダッシュ
	Dodge,			//回避

	//ターゲット
	TargetLock,		//最も近くの敵をターゲット
	TargetSwitch,	//ターゲット切り替え

	//カメラ視点切り替え
	ToggleCamera,	//一人称か三人称

	//Ui
	Pause,			//ポーズ
	Confirm,		//決定
	Cancel,			//キャンセル

	Interact,		//開始
	Skip,			//カットシーンスキップ

	Count			//カウント
};
