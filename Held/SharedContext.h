#pragma once
#include "Difficulty.h"
//#include "InputSystem.h"
//#include "CameraSystem.h"
//#include "TargetSystem.h"
//#include "Stage.h"
//#include "Player.h"
//#include "Boss.h"
//#include "LightSystem.h"
//#include "ProjectileManager.h"

//class EnemyManager;

// 各Managerが実装されたら追加していく


/// <summary>
/// 各シーン間で共有されるデータ
/// </summary>
struct SharedContext
{
	Difficulty difficulty = Difficulty::Normal;

	//InputSystem  input;

	//CameraSystem camera;
	
	//Stage* stage = nullptr;

	//Player* player = nullptr;

	//EnemyManager* enemies = nullptr;

	//Boss* boss = nullptr;

	//TargetSystem target;

	//LightSystem light;

	//ProjectileManager projectiles;

	float clearTimeSec	= 0.0f;		//ボス撃破までの秒数;
	bool isGameClear	= false;	//ゲームクリアしたかどうか
};