#include "DxLib.h"
#include "Precompiled.h"
#include "BoneAttachment.h"

void BoneAttachment::Update(int ownerModel, int followFrame)
{
	if (handle < 0 || ownerModel < 0 || followFrame < 0)
	{
		return;
	}

	const MATRIX boneWorld= MV1GetFrameLocalWorldMatrix(ownerModel, followFrame);

	MATRIX mat	= MGetScale(VGet(scale, scale, scale));
	mat			= MMult(mat, offset);
	mat			= MMult(mat, boneWorld);

	world = mat;
	MV1SetMatrix(handle, world);
}

void BoneAttachment::Draw()const
{
	if (handle < 0)
	{
		return;
	}

	SetUseLighting(FALSE);
	MV1DrawModel(handle);
	SetUseLighting(TRUE);
}
