#pragma once

/// <summary>
/// プレイヤーの行動状態の識別子
/// </summary>
enum class PlayerStateId
{
	BaseMovement,		//待機＋移動＋ダッシュ
	Dodge,				//回避
	Jump,				//ジャンプ
	JumpAttack,			//落下攻撃
	Attack,				//地上コンボ攻撃
	CounterDash,		//ジャスト回避攻撃
	Magic,				//魔法
	Ultimate,			//必殺技
	Damaged,			//被弾のけぞり
	Dead,				//死亡

	Count				//総数
};
