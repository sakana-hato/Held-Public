#pragma once
#include "Config.h"

/// <summary>
/// シャドウマップ用クラス
/// </summary>
class ShadowMap
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ShadowMap() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ShadowMap();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="size"></param>シャドウマップの解像度
	void Init(int size = Config::Shadow::SHADOW_MAP_SIZE);

	/// <summary>
	/// シャドウマップを解放す
	/// </summary>
	void End();

	/// <summary>
	/// 影を落とす光の向きを設定する
	/// </summary>
	/// <param name="dir"></param>光の進む方向
	void SetLightDirection(const VECTOR& dir)const;

	/// <summary>
	/// 影を描画する範囲を設定する
	/// </summary>
	/// <param name="minPos"></param>範囲の最小座標
	/// <param name="maxPos"></param>範囲の最大座標
	void SetArea(const VECTOR& minPos, const VECTOR& maxPos)const;

	/// <summary>
	/// 影を落とす側の描画を始める
	/// </summary>
	void BeginCast() const;

	/// <summary>
	/// 影を落とす側の描画を終える
	/// </summary>
	void EndCast() const;

	/// <summary>
	/// 影を受ける側の描画を始める
	/// </summary>
	void BeginReceive() const;

	/// <summary>
	/// 影を受ける側の描画を終える
	/// </summary>
	void EndReceive() const;

	/// <summary>
	/// シャドウマップのハンドルを取得する
	/// </summary>
	/// <returns></returns>ハンドル
	int  GetHandle() const { return handle; }

private:
	int handle = -1;	//　シャドウマップのハンドル
};