#pragma once
#include "DxLib.h"

/// <summary>
/// 地面攻撃の着地点に岩石リングをせり上がらせるクラス
/// </summary>
class RockRing
{
public:
	/// <summary>
	/// モデルをセットする
	/// </summary>
	/// <param name="modelHandle"></param>岩石リングのモデルハンドル
	void Init(int modelHandle);

	/// <summary>
	/// モデルを解放する
	/// </summary>
	void End();

	/// <summary>
	///　発動のトリガー処理
	/// </summary>
	/// <param name="center"> </param>中心のワールド座標
	/// <param name="radius"> </param>攻撃範囲の半径
	/// <param name="groundY"></param>地面の高さ
	void Trigger(const VECTOR& center, float radius, float groundY);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 演出中かどうか。
	/// </summary>
	bool IsActive() const { return active; }

private:
	//演出の段階
	enum class Phase
	{
		Rising,    //地面の下からせり上がる
		Hold,      //地面の高さで少し維持
		Sinking,   //地面に沈んでいく
	};

	int    handle	= -1;				//複製したモデル
	VECTOR _center	= {};				//中心
	float  _groundY = 0.0f;				//地面の高さ
	float  _radius	= 0.0f;				//攻撃範囲の半径
	float  timer	= 0.0f;				//現在フェーズの経過時間
	bool   active	= false;			//演出中か
	Phase  phase	= Phase::Rising;	//最初のフェーズはせり上がり
};