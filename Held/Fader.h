#pragma once

/// <summary>
/// フェード演出用のシングルトンクラス
/// </summary>
class Fader final
{
public:
	/// <summary>
	///シングルトンインスタンスを取得
	/// </summary>
	static Fader& GetInstance()
	{
		static Fader instance;
		return instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// だんだん明るくなる(明転)
	/// </summary>
	/// <param name="speed"></param>明るくなるスピード
	void FadeIn(float speed = 10.0f);

	/// <summary>
	/// だんだん暗くなる
	/// </summary>
	/// <param name="speed"></param>暗くなるスピード
	void FadeOut(float speed = 10.0f);

	// <summary>
	/// フェードイン（明転）が終わったか
	/// </summary>
	bool IsFinishFadeIn() const;

	/// <summary>
	/// フェードアウト（暗転）が終わったか
	/// </summary>
	bool IsFinishFadeOut() const;

	/// <summary>
	/// フェード中かどうか
	/// </summary>
	bool IsFadeNow() const { return isFadeNow; }

	/// <summary>
	/// フェードイン（明転）中かどうか
	/// </summary>
	bool IsFadeInNow() const { return isFadeIn; }

	/// <summary>
	/// フェードアウト（暗転）中かどうか
	/// </summary>
	bool IsFadeOutNow() const { return isFadeOut; }

	/// <summary>
	/// 覆う色を変更する
	/// </summary>
	void SetColor(unsigned int color) { _color = color; }
private:
	/// <summary>
	/// //シングルトンのためコンストラクタ
	/// </summary>
	Fader()				= default;
	~Fader()			= default;

	Fader(const Fader&) = delete;
	Fader& operator		= (const Fader&) = delete;

	Fader(Fader&&)		= delete;
	Fader& operator		=(Fader&&)	= delete;

	float countFrame	= 0.0f;			//黒幕の濃さ（0=透明, 255=真っ黒）
	float fadeSpeed		= 0.0f;			//1フレームあたりの変化量
	bool  isFadeNow		= false;		//フェード中か
	bool  isFadeIn		= false;		//明転中
	bool  isFadeOut		= false;		//暗転中
	unsigned int _color = 0x000000;		//覆う色
};
