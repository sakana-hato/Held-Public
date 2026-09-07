#pragma once
#include "Config.h"

/// <summary>
/// 世界の光クラス　個別で色を変えたい場合はsetterとかを使ってほしい
/// 将来はシェーダーを使いたい
/// </summary>
class LightSystem
{
public:
	/// <summary>
	/// Configの設定値でライトを一括適用する
	/// </summary>
	void Apply();

	/// <summary>
	/// ライティングの有効・無効を切り替える
	/// </summary>
	/// <param name="ena"></param>trueで有効
	void Enable(bool ena) { SetLightEnable(ena ? TRUE : FALSE); }

	/// <summary>
	/// メインライトの拡散色（0.0から1.0）
	/// </summary>
	/// <param name="r"></param>赤
	/// <param name="g"></param>緑
	/// <param name="b"></param>青
	void SetDiffuse(float r, float g, float b) { SetLightDifColor(GetColorF(r, g, b, 0.0f)); }

	/// <summary>
	/// メインライトの環境光
	/// </summary>
	/// <param name="r"></param>赤
	/// <param name="g"></param>緑
	/// <param name="b"></param>青
	void SetAmbient(float r, float g, float b) { SetLightAmbColor(GetColorF(r, g, b, 0.0f)); }

	/// <summary>
	/// 全体の環境光
	/// </summary>
	/// <param name="r"></param>赤
	/// <param name="g"></param>緑
	/// <param name="b"></param>青
	void SetGlobalAmbient(float r, float g, float b) { SetGlobalAmbientLight(GetColorF(r, g, b, 0.0f)); }

	/// <summary>
	/// ライトの向きを設定する
	/// </summary>
	/// <param name="dir"></param>光の進む方向
	void SetDirection(const VECTOR& dir) { _dir = dir; SetLightDirection(dir); }

	/// <summary>
	/// ライトの向きを取得する
	/// </summary>
	VECTOR GetDirection() const { return _dir; }

	/// <summary>
	/// ライトの位置と方向を可視化する（デバッグ用）
	/// </summary>
	void DrawDebug() const;
private:
	VECTOR _dir = VGet(0.0f, -1.0f, 0.0f);	//光の進む方向
};