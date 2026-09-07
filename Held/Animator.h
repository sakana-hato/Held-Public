#pragma once
#include "Config.h"

/// <summary>
/// アニメションを再生クラス
/// </summary>
class Animator
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Animator() = default;
	explicit Animator(int modelHandle) { SetModel(modelHandle); }

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Animator();

	/// <summary>
	/// 対象モデルの設定関数
	/// </summary>
	/// <param name="modelHandle"></param>使用するモデル
	void SetModel(int modelHandle);

	/// <summary>
	/// アニメションを再生する
	/// </summary>
	/// <param name="animIndex"></param>使用するアニメーション番号
	/// <param name="loop">		</param>ループ再生
	/// <param name="blendSec">	</param>前のアニメからのブレンド時間
	void Play(int animModel,int animIndex, bool loop = true, float blendSec = Config::Anime::ANIM_BLEND_DEFAULT);

	/// <summary>
	/// ブレンドなしでアニメーション再生する
	/// </summary>
	/// <param name="animModel"></param>使用するアニメーションモデル
	/// <param name="animIndex"></param>使用するアニメーション番号
	/// <param name="loop">		</param>ループ再生
	void PlayImmediate(int animModel, int animIndex, bool loop = true);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// 再生終了したかどうか
	/// </summary>
	bool IsFinished()const;

	/// <summary>
	/// 現在の再生しているアニメション
	/// </summary>
	int CurrentAnim()const { return curAnimIndex; }

	/// <summary>
	/// アニメーションの再生スピードを変更する
	/// </summary>
	/// <param name="spe"></param>再生スピード
	void SetSpeed(float spe) { speed = spe; }

	/// <summary>
	/// アニメーションのスピードゲッター
	/// </summary>
	/// <returns></returns>
	float GetSpeed() const { return speed; }

	/// <summary>
	/// 現在のアニメーションモデル
	/// </summary>
	int   CurrentAnimModel() const { return curAnimModel; }

	/// <summary>
	/// 現在のアニメーション番号
	/// </summary>
	int   CurrentAnimIndex() const { return curAnimIndex; }

	/// <summary>
	/// 現在のアニメーション時間
	/// </summary>
	float CurrentAnimTime()  const { return curTime; }

private:
	/// <summary>
	/// アニメーションのモデルへのアタッチ
	/// </summary>
	/// <param name="modelHandle">	</param>モデルハンドル
	/// <param name="animIndex">	</param>アニメーション番号
	/// <param name="outTotal">		</param>総時間
	/// <returns></returns>アタッチ番号
	int Attach(int modelHandle, int animIndex, float& outTotal)const;

	/// <summary>
	/// 再生時間を進めてモデルを反映する
	/// </summary>
	/// <param name="attach">	</param>アニメーションアタッチ
	/// <param name="time">		</param>時間
	/// <param name="total">	</param>総時間
	/// <param name="loop">		</param>ループ再生
	/// <param name="dt">		</param>デルタタイム
	void AdvanceAnim(int attach, float& time, float total, bool loop, float dt)const;

	/// <summary>
	/// ブレンドを完了し次のアニメーション移行する
	/// </summary>
	void FinalizeBlend();

	/// <summary>
	/// アタッチ済みアニメーションをすべて外す
	/// </summary>
	void DetachAll();


	int model				= -1;	// モデル

	//現在使用しているアニメ
	int		curAnimModel	= -1;	// 現在のモデル
	int		curAttach		= -1;	// 現在のアタッチアニメ
	int		curAnimIndex	= -1;	// 現在のアニメ番号
	float	curTime			= 0.0f;	// 現在のアニメ再生時間
	float	curTotal		= 0.0f;	// 現在のアニメ総時間
	bool	curLoop			= true;	// 現在のアニメループ

	//ブレンド先のアニメ
	int		nextAnimModel	= -1;	// 次のモデル
	int		nextAttach		= -1;	// 次のアタッチモデル
	int		nextAnimIndex	= -1;	// 次のアニメ番号
	float	nextTime		= 0.0f;	// 次のアニメ再生時間
	float	nextTotal		= 0.0f;	// 次のアニメ総時間
	bool	nextLoop		= true;	// 次のアニメループ

	
	float	blendTime		= 0.0f;	// ブレンド経過
	float	blendDur		= 0.0f;	// ブレンド総時間
	float	speed			= 1.0f;	// 再生速度倍率
	bool	blending		= false;// ブレンドフラグ

};
