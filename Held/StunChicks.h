#pragma once

/// <summary>
/// 気絶中にぐるぐる回るヒヨコ。ボスの頭上を旋回しながら上下にふわふわ動く
/// </summary>
class StunChicks
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>複製元のヒヨコモデル
	/// <param name="scale">	  </param>表示サイズの倍率
	void Init(int modelHandle, float scale);

	/// <summary>
	/// 複製したモデルを解放する
	/// </summary>
	void End();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="headPos"></param>ボスの頭のボーン位置
	void Draw(const VECTOR& headPos) const;

private:
	static constexpr int   CHICK_COUNT	= 3;		//ヒヨコの数
	static constexpr float RADIUS		= 150.0f;	//頭の周りを回る半径
	static constexpr float HEIGHT		= 120.0f;	//頭からの高さ
	static constexpr float ROTATE_SPEED = 2.0f;		//旋回速度
	static constexpr float BOB_SPEED	= 4.0f;		//上下の速さ
	static constexpr float BOB_AMP		= 15.0f;	//上下の振れ幅

	int   handles[CHICK_COUNT]			= { -1, -1, -1 };	//複製したモデルのハンドル
	float angle							= 0.0f;				//旋回の角度
	float bob							= 0.0f;				//上下の位相
};