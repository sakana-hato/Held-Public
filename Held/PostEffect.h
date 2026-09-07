#pragma once

/// <summary>
/// ポストエフェクト
/// </summary>
class PostEffect
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PostEffect();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="w"></param>画面幅
	/// <param name="h"></param>画面高さ
	void Init(int w,int h);

	/// <summary>
	/// 確保したリソースを解放する
	/// </summary>
	void End();

	/// <summary>
	///  //3D描画先をオフスクリーンへ切り替え
	/// </summary>
	void BeginScene()const;    

	/// <summary>
	/// 暗化＋色調＋ビネットを掛けてバックバッファへ合成する
	/// </summary>
	void Composite()  const;	

	/// <summary>
	/// モノクロの強さを設定する
	/// </summary>
	/// <param name="rate"></param>0=通常、1=完全モノクロ
	void SetMonochromeRate(float rate) { monochromeRate = rate; }

	/// <summary>
	/// 被弾時の赤フラッシュの強さを設定する
	/// </summary>
	/// <param name="rate"></param>0=なし、1=最大
	void SetDamageRate(float rate) { damageRate = rate; }

	/// <summary>
	/// グリッチ（横方向のずれと色収差）の強さを設定する
	/// </summary>
	/// <param name="rate"></param>0=なし、1=最大
	void SetGlitchRate(float rate) { glitchRate = rate; }

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 波紋演出を開始する
	/// </summary>
	void StartRipple() { rippleState = RippleState::Expanding; rippleProgress = 0.0f; }

	/// <summary>
	/// 波紋演出を終了させる
	/// </summary>
	void StopRipple() { if (rippleState != RippleState::None) rippleState = RippleState::Shrinking; }
	
private:
	//波紋の状態
	enum class RippleState
	{ 
		None,		//何もない
		Expanding,	//広がり中
		Shrinking	//縮み中
	};  

	RippleState rippleState = RippleState::None;// 現在の波紋の状態

	int		_w				= 0;		// 画面幅
	int		_h				= 0;		// 画面高さ
	int		sceneScreen		= -1;		// 3Dを一旦描くオフスクリーン
	int		psHandle		= -1;		// ピクセルシェーダーのハンドル
	int		cbHandle		= -1;		// 定数バッファのハンドル

	//シェーダーへ渡すパラメータ
	float	monochromeRate	= 0.0f;		// モノクロの強さ
	float	glitchRate		= 0.0f;		// グリッチの強さ
	float	time			= 0.0f;		// 起動からの経過時間
	float	damageRate		= 0.0f;		// 被弾赤フラッシュの強さ
	float	rippleProgress	= 0.0f;		// 波紋の進行
	bool	rippleActive	= false;    // 波紋演出中フラグ
};
