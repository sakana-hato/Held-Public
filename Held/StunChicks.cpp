#include "DxLib.h"
#include "Precompiled.h"
#include "StunChicks.h"

void StunChicks::Init(int modelHandle, float scale)
{
	//同じモデルを同時に別々の位置へ複製する
	for (int i = 0; i < CHICK_COUNT; ++i)
	{
		handles[i] = MV1DuplicateModel(modelHandle);
		MV1SetScale(handles[i], VGet(scale, scale, scale));
	}
}

void StunChicks::End()
{
	for (int i = 0; i < CHICK_COUNT; ++i)
	{
		if (handles[i] >= 0)
		{
			MV1DeleteModel(handles[i]);
			handles[i] = -1;
		}
	}
}

void StunChicks::Update(float dt)
{
	angle	+= ROTATE_SPEED * dt;	//回転角度を進める
	bob		+= dt * BOB_SPEED;		//上下のふわふわ
}

void StunChicks::Draw(const VECTOR& headPos) const
{
	SetUseLighting(FALSE);	//ライティングを切る

	for (int i = 0; i < CHICK_COUNT; ++i)
	{
		if (handles[i] < 0)
		{
			continue;
		}

		//等間隔（3体なら120度ずつ）ずらして円形に配置
		const float a = angle + DX_TWO_PI_F * i / CHICK_COUNT;
		const float x = headPos.x + std::cos(a) * RADIUS;
		const float z = headPos.z + std::sin(a) * RADIUS;
		const float y = headPos.y + HEIGHT + std::sin(bob + i * 2.0f) * BOB_AMP;//上下にふわふわ

		MV1SetPosition(handles[i], VGet(x, y, z));

		//ヒヨコが進行方向を向くよう回す
		MV1SetRotationXYZ(handles[i], VGet(0.0f, -a + DX_PI_F * 0.5f, 0.0f));

		MV1DrawModel(handles[i]);
	}

	SetUseLighting(TRUE);	//他の描画に影響しないよう戻す
}