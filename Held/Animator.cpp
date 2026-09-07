#include "DxLib.h"
#include "Precompiled.h"
#include "Animator.h"

Animator::~Animator()
{
	DetachAll();
}

void Animator::SetModel(int modelHandle)
{
	DetachAll();

	model			= modelHandle;
	curAnimModel	= -1;
	curAttach		= -1;
	curAnimIndex	= -1;
	curTime			= 0.0f;
	curTotal		= 0.0f;
	curLoop			= true;

	nextAnimModel	= -1;
	nextAttach		= -1;
	nextAnimIndex	= -1;
	nextTime		= 0.0f;
	nextTotal		= 0.0f;
	nextLoop		= true;

	blending		= false;
	blendTime		= 0.0f;
	blendDur		= 0.0f;
}

int Animator::Attach(int modelHandle, int animIndex, float& outTotal)const
{
	//アニメーションの供給元を決める
	const int input = (modelHandle == model) ? -1 : modelHandle;

	//アニメーションのアタッチする
	const int att = MV1AttachAnim(model, animIndex, input, true);

	//アニメーション相殺性時間を受け取る
	outTotal = (att >= 0) ? MV1GetAttachAnimTotalTime(model, att) : 0.0f;
	return att;
}

void Animator::Play(int animModel, int animIndex, bool loop , float blendSec )
{
	//安全措置
	if (model < 0 || animModel < 0||animIndex<0|| animIndex >= MV1GetAnimNum(animModel))
	{
		return;
	}

    //使用するアニメーションモデル
	const int playModel = blending ? nextAnimModel : curAnimModel;
	//使用するアニメーション番号
	const int playIndex = blending ? nextAnimIndex : curAnimIndex;

	if (animModel == playModel && animIndex == playIndex)
	{
		return;
	}

	//ブレンド中なら片付ける
	if (blending)
	{
		FinalizeBlend();
	}

	if (curAttach<0)
	{
		curAttach		= Attach(animModel, animIndex, curTotal);
		curAnimModel	= animModel;
		curAnimIndex	= animIndex;
		curTime			= 0.0f;
		curLoop			= loop;

		if (curAttach >= 0)
		{
			MV1SetAttachAnimBlendRate(model, curAttach, 1.0f);
		}

		blending		= false;
		return;
	}

	nextAttach		= Attach(animModel, animIndex, nextTotal);
	nextAnimModel	= animModel;
	nextAnimIndex	= animIndex;
	nextTime		= 0.0f;
	nextLoop		= loop;
	blending		= true;
	blendTime		= 0.0f;
	blendDur		= (blendSec > 0.0f) ? blendSec : 0.0001f;

	MV1SetAttachAnimBlendRate(model, curAttach, 1.0f);

	if (nextAttach >= 0)
	{
		MV1SetAttachAnimBlendRate(model, nextAttach, 0.0f);
	}
}

void Animator::PlayImmediate(int animModel, int animIndex, bool loop)
{
	//安全措置
	if (model < 0 || animModel < 0 || animIndex < 0 || animIndex >= MV1GetAnimNum(animModel))
	{
		return;
	}

	//ブレンド中なら片付ける
	if (blending)
	{
		FinalizeBlend();
	}

	//既存のアタッチを外す
	if (curAttach >= 0)
	{
		MV1DetachAnim(model, curAttach);
		curAttach = -1;
	}
	if (nextAttach >= 0)
	{
		MV1DetachAnim(model, nextAttach);
		nextAttach = -1;
	}

	//新しく再生する
	curAttach		= Attach(animModel, animIndex, curTotal);
	curAnimModel	= animModel;
	curAnimIndex	= animIndex;
	curTime			= 0.0f;
	curLoop			= loop;

	if (curAttach >= 0)
	{
		MV1SetAttachAnimBlendRate(model, curAttach, 1.0f);
	}

	blending = false;
}

void Animator::AdvanceAnim(int attach, float& time, float total, bool loop, float dt)const
{
	//安全措置
	if (attach < 0)
	{
		return;
	}

	time += dt * speed * Config::Anime::ANIM_FPS;

	if (loop)
	{
		if (total > 0.0f)
		{
			time = std::fmod(time, total);
			if (time < 0.0f) time += total;
		}
	}
	else
	{
		if (time > total)
		{
			time = total;
		}
	}

	MV1SetAttachAnimTime(model, attach, time);
}

void Animator::Update(float dt)
{
	//安全措置
	if (model < 0 || curAttach < 0)
	{
		return;
	}

	AdvanceAnim(curAttach, curTime, curTotal, curLoop, dt);

	if (blending)
	{
		AdvanceAnim(nextAttach, nextTime, nextTotal, nextLoop, dt);
		blendTime += dt;
		float rate =(blendDur>0.0f) ? (blendTime / blendDur) : 1.0f;

		if (rate > 1.0f)
		{
			rate = 1.0f;
		}

		MV1SetAttachAnimBlendRate(model, curAttach, 1.0f - rate);
		MV1SetAttachAnimBlendRate(model, nextAttach, rate);

		if (rate >= 1.0f)
		{
			FinalizeBlend();
		}
	}
}

void Animator::FinalizeBlend()
{
	if (!blending)
	{
		return;
	}

	if (curAttach >= 0)
	{
		MV1DetachAnim(model, curAttach);
	}

	curAnimModel	= nextAnimModel;
	curAttach		= nextAttach;
	curAnimIndex	= nextAnimIndex;
	curTime			= nextTime;
	curTotal		= nextTotal;
	curLoop			= nextLoop;

	nextAnimModel	= -1;
	nextAttach		= -1;
	nextAnimIndex	= -1;
	nextTime		= 0.0f;
	nextTotal		= 0.0f;
	nextLoop		= true;

	blending		= false;

	if (curAttach >= 0)
	{
		MV1SetAttachAnimBlendRate(model, curAttach, 1.0f);
	}
}

bool Animator::IsFinished()const
{
	if (curAttach < 0)
	{
		return true;
	}

	if (curLoop)
	{
		return false;
	}

	if (blending)
	{
		return false;
	}

	return curTime >= curTotal;
}

void Animator::DetachAll()
{
	if (model < 0)
	{
		return;
	}

	if (curAttach >= 0)
	{
		MV1DetachAnim(model, curAttach);
	}

	if (nextAttach >= 0)
	{
		MV1DetachAnim(model, nextAttach);
	}

	curAttach	= -1;
	nextAttach	= -1;
}