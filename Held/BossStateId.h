#pragma once

/// <summary>
/// ボスの行動の識別子　後で増やすよ後でね
/// </summary>
enum class BossStateId
{
	Intro,		//出現
	Idle,		//待機
	Chase,		//追跡
	Attack,		//攻撃
	DrawSword,	//大剣装備
	Damage,		//被弾
	Stagger,	//怯み
	Dead,		//死亡

	Count		//カウント
};
