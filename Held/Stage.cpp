#include "DxLib.h"
#include "Config.h"
#include "Precompiled.h"
#include "Stage.h"

Stage::~Stage()
{
	End();
}

void Stage::ApplyTransformTo(int model) const
{
	if (model < 0)
	{
		return;
	}

	const float sca = (model == collModel) ? collScale : scale; 
	MV1SetPosition		(model, pos);
	MV1SetScale			(model, VGet(sca, sca, sca));
	MV1SetRotationXYZ	(model, VGet(0.0f, (model == collModel) ? collYawRad : 0.0f, 0.0f));
}

void Stage::SetViewModel(int handle)
{
	//表示モデルを差し替え
	if (viewModel >= 0)
	{
		if (doorAttachA >= 0)
		{
			MV1DetachAnim(viewModel, doorAttachA);
		}

		if (doorAttachB >= 0)
		{
			MV1DetachAnim(viewModel, doorAttachB);
		}

		doorAttachA = doorAttachB = -1;
	}
	viewModel = handle;
	ApplyTransformTo(viewModel);
	AttachDoorAnims();

}

void Stage::SetCollisionModel(int handle)
{
	//コリジョンモデルを差し替え
	if (collReady && collModel >= 0)
	{
		MV1TerminateCollInfo(collModel, -1, -1);
		collReady = false;
	}
	collModel = handle;
	ApplyTransformTo(collModel); 
	SetupCollision();
}

void Stage::SetupCollision()
{
	if (collModel < 0)
	{
		return;
	}

	MV1SetupCollInfo(collModel, -1, 8, 8, 8);  //粗いモデルなので軽め
	collReady = true;
}

bool Stage::GetFloorY(const VECTOR& p, float& outY) const
{
	//安全措置
	if (collModel < 0 || !collReady)
	{
		return false;
	}

	//調べる点の少し上から、真下へ長い線を伸ばす
	const VECTOR start	= VGet(p.x, p.y + FLOOR_RAY_UP, p.z);
	const VECTOR end	= VGet(p.x, p.y - FLOOR_RAY_DOWN, p.z);

	MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(collModel, -1, start, end);//線とコリジョンモデルの交差を調べる

	if (hit.HitFlag == 0)
	{
		return false;
	}

	if (hit.Normal.y < 0.5f)
	{
		return false;
	}


	outY = hit.HitPosition.y;
	return true;
}

VECTOR Stage::ResolveWall(const VECTOR& footPos, float radius, float height)
{
	VECTOR result = footPos;

	//安全措置
	if (collModel < 0 || !collReady)
	{
		return result;
	}

	//カプセルの下端と上端の高さ
	const float bottomOffset	= radius + WALL_FOOT_LIFT;
	const float topOffset		= height - radius;

	for (int i = 0; i < WALL_RESOLVE_ITER; ++i)
	{
		//カプセルの上と下を計算する
		const VECTOR capBottom	= VGet(result.x, result.y + bottomOffset, result.z);
		const VECTOR capTop		= VGet(result.x, result.y + topOffset, result.z);

		//カプセルに触れているポリゴンを全て取得する
		MV1_COLL_RESULT_POLY_DIM dim =MV1CollCheck_Capsule(collModel, -1, capBottom, capTop, radius);

		//どこにも当たっていない時
		if (dim.HitNum == 0)
		{
			MV1CollResultPolyDimTerminate(dim);
			break;
		}

		VECTOR push		= VGet(0.0f, 0.0f, 0.0f);	//押し出しベクトルの合計
		int wallCount	= 0;						//壁として扱った面の数


		for (int k = 0; k < dim.HitNum; ++k)
		{
			const MV1_COLL_RESULT_POLY& poly = dim.Dim[k];

			VECTOR nor = poly.Normal;

			if (std::fabs(nor.y) > WALL_NORMAL_Y_MAX)
			{
				continue; //床/天井は無視
			}

			nor.y		= 0.0f;//水平成分だけの法線
			float nlen	= VSize(nor);

			//真上か真下を向いていた
			if (nlen <= 1e-4f)
			{
				continue;
			}

			nor = VScale(nor, 1.0f / nlen);
			nor = VScale(nor, -1.0f);//壁から離れる向きへ反転

			//カプセル軸の中心から、ポリゴン面までの距離を見てめり込んでいる分だけ押し戻す
			const VECTOR capCenter = VGet(result.x, result.y + (bottomOffset + topOffset) * 0.5f, result.z);

			//ポリゴンの代表点
			const VECTOR pc = VGet((poly.Position[0].x + poly.Position[1].x + poly.Position[2].x) / 3.0f,(poly.Position[0].y + poly.Position[1].y + poly.Position[2].y) / 3.0f,(poly.Position[0].z + poly.Position[1].z + poly.Position[2].z) / 3.0f);

			//中心から面までの符号付き距離
			const VECTOR toCenter	= VSub(capCenter, pc);
			const float dist		= toCenter.x * nor.x + toCenter.z * nor.z; //水平を見ています

			//半径より近ければめり込んでいる
			const float penetration = radius - dist;
			if (penetration > 0.0f)
			{
				push = VAdd(push, VScale(nor, penetration));
				wallCount++;
			}
		}

		MV1CollResultPolyDimTerminate(dim);

		//押し出す必要がない時
		if (wallCount == 0 || VSize(push) <= 1e-4f)
		{
			break;
		}

		//複数面の平均を少し余裕を持って押す
		push		= VScale(push, 1.0f / static_cast<float>(wallCount));

		//WALL_SKIN の分だけ余分に離し、壁に触れ続けて振動するのを防ぐ
		result.x	+= push.x + (push.x > 0 ? WALL_SKIN : (push.x < 0 ? -WALL_SKIN : 0.0f));
		result.z	+= push.z + (push.z > 0 ? WALL_SKIN : (push.z < 0 ? -WALL_SKIN : 0.0f));
	}

	return result;
}

VECTOR Stage::ClampCameraByWall(const VECTOR& from, const VECTOR& to, float margin)const
{
	//安全措置
	if (collModel < 0 || !collReady)
	{
		return to;
	}

	MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(collModel, -1, from, to);	// 注視点からカメラへ線を飛ばし、遮る壁を探す

	//遮るものがないので理想位置のまま
	if (hit.HitFlag == 0)
	{
		return to;
	}

	//床や天井で引き寄せるとカメラが暴れるので、壁だけを対象にする
	if (std::fabs(hit.Normal.y) > WALL_NORMAL_Y_MAX)
	{
		return to;
	}

	VECTOR dir		= VSub(to, from);
	const float len = VSize(dir);

	//始点と終点がほぼ同じ
	if (len <= 1e-4f)
	{
		return to;
	}

	dir					= VScale(dir, 1.0f / len);
	const float hitDist = VSize(VSub(hit.HitPosition, from));
	float dist			= hitDist - margin;

	//注視点より手前に行かせない
	if (dist < 0.0f)
	{
		dist = 0.0f;
	}

	return VAdd(from, VScale(dir, dist));
}

VECTOR Stage::ResolveCameraSphere(const VECTOR& camPos, float radius)const
{
	VECTOR result = camPos;

	//安全措置
	if (collModel < 0 || !collReady)
	{
		return result;
	}

	const float halfH = radius * 0.5f; // カメラに球をつける

	for (int i = 0; i < CAM_RESOLVE_ITER; ++i)
	{
		const VECTOR capBottom	= VGet(result.x, result.y - halfH, result.z);
		const VECTOR capTop		= VGet(result.x, result.y + halfH, result.z);

		MV1_COLL_RESULT_POLY_DIM dim = MV1CollCheck_Capsule(collModel, -1, capBottom, capTop, radius);

		if (dim.HitNum == 0)
		{
			MV1CollResultPolyDimTerminate(dim);
			break;
		}

		VECTOR push		= VGet(0.0f, 0.0f, 0.0f);	//押し出しベクトルの合計
		int count		= 0;						//押し出した面の数

		for (int k = 0; k < dim.HitNum; ++k)
		{
			const MV1_COLL_RESULT_POLY& poly = dim.Dim[k];

			//面
			const VECTOR pc = VGet(
				(poly.Position[0].x + poly.Position[1].x + poly.Position[2].x) / 3.0f,
				(poly.Position[0].y + poly.Position[1].y + poly.Position[2].y) / 3.0f,
				(poly.Position[0].z + poly.Position[1].z + poly.Position[2].z) / 3.0f);

			//法線ではなく重心から離れる向きへ押す
			VECTOR away			= VSub(result, pc);
			away.y				= 0.0f;
			const float dlen	= VSize(away);

			//重なって向きが変わらないとき用
			if (dlen <= 1e-4f)
			{
				continue;
			}

			away = VScale(away, 1.0f / dlen);

			const float penetration = radius - dlen;
			if (penetration > 0.0f)
			{
				push = VAdd(push, VScale(away, penetration));
				count++;
			}
		}
		MV1CollResultPolyDimTerminate(dim);

		if (count == 0 || VSize(push) <= 1e-4f)
		{
			break;
		}

		push	= VScale(push, 1.0f / static_cast<float>(count));
		result	= VAdd(result, push);
	}
	return result;
}

VECTOR Stage::ResolveDoorWall(const VECTOR& pos, float radius) const
{
	//ドアがアクティブでないとき
	if (!doorWallActive)
	{
		return pos;
	}

	VECTOR result = pos;

	//扉のX範囲内か
	const float dx = pos.x - doorWallCenterX;

	//X範囲外なので関係なし
	if (std::abs(dx) >= doorWallHalfX + radius)
	{
		return result;  
	}

	//扉の板の手前側・奥側の境界
	const float frontZ	= doorWallZ + radius;   //手前側の限界
	const float backZ	= doorWallZ - radius;   //奥側の限界

	//板の厚みの中にめり込んでいるときだけ押し出す
	if (pos.z < frontZ && pos.z > backZ)
	{
		//近い方の面へ押し出す
		const float toFront = frontZ - pos.z;   //手前へ出るのに必要な距離
		const float toBack	= pos.z - backZ;    //奥へ出るのに必要な距離

		if (toFront <= toBack)
		{
			result.z = frontZ;   //手前へ押し出す
		}
		else
		{
			result.z = backZ;    //奥へ押し出す
		}
	}
	return result;
}

void Stage::AttachDoorAnims()
{
	//安全措置
	if (viewModel < 0)
	{
		return;
	}

	//左右の扉をそれぞれ別のアニメとして付ける
	doorAttachA = MV1AttachAnim(viewModel, Config::Door::DOOR_ANIM_A, -1, FALSE);
	doorAttachB = MV1AttachAnim(viewModel, Config::Door::DOOR_ANIM_B, -1, FALSE);

	//総再生時間を控えておく
	doorTotalA = (doorAttachA >= 0) ? MV1GetAttachAnimTotalTime(viewModel, doorAttachA) : 0.0f;
	doorTotalB = (doorAttachB >= 0) ? MV1GetAttachAnimTotalTime(viewModel, doorAttachB) : 0.0f;

	//閉じた状態で止めておく
	if (doorAttachA >= 0)
	{
		MV1SetAttachAnimTime(viewModel, doorAttachA, 0.0f);
	}

	if (doorAttachB >= 0)
	{
		MV1SetAttachAnimTime(viewModel, doorAttachB, 0.0f);
	}

	doorTime	= 0.0f;
	doorPlaying = false;
}

void Stage::PlayDoor()
{
	
	doorTime	= 0.0f;
	doorPlaying = true;
	doorReverse = false;
}

void Stage::CloseDoor()
{
	
	doorTime	= (doorTotalA > doorTotalB) ? doorTotalA : doorTotalB;
	doorPlaying = true;
	doorReverse = true;  
}

void Stage::Update(float dt)
{
	//安全措置
	if (viewModel < 0 || !doorPlaying)
	{
		return;
	}

	//秒からフレームへ変換する
	if (doorReverse)
	{
		doorTime -= dt * Config::Anime::ANIM_FPS * Config::Door::DOOR_ANIM_SPEED;
		if (doorTime < 0.0f)
		{
			doorTime = 0.0f;
		}

	}
	else
	{
		doorTime += dt * Config::Anime::ANIM_FPS * Config::Door::DOOR_ANIM_SPEED;
	}

	//長さが違う場合は片方のアニメーション再生時間に合わせる
	if (doorAttachA >= 0)
	{
		const float time = (doorTime > doorTotalA) ? doorTotalA : doorTime;
		MV1SetAttachAnimTime(viewModel, doorAttachA, time);
	}
	
	if (doorAttachB >= 0)
	{
		const float time = (doorTime > doorTotalB) ? doorTotalB : doorTime;
		MV1SetAttachAnimTime(viewModel, doorAttachB, time);
	}

	//両方の扉が動き終わったら再生を止める
	if (doorReverse)
	{
		if (doorTime <= 0.0f)
		{
			doorPlaying = false;
		}
	}
	else
	{
		if (doorTime >= doorTotalA && doorTime >= doorTotalB)
		{
			doorPlaying = false;
		}
	}
}

void Stage::End()
{
	//モデルの当たり判定の解放
	if (collReady && collModel >= 0)
	{
		MV1TerminateCollInfo(collModel, -1, -1);
		collReady = false;
	}

	//アニメーションの解放
	if (viewModel >= 0)
	{
		if (doorAttachA >= 0)
		{
			MV1DetachAnim(viewModel, doorAttachA);
		}

		if (doorAttachB >= 0)
		{
			MV1DetachAnim(viewModel, doorAttachB);
		}

	}

	doorAttachA = doorAttachB = -1;
	doorPlaying = false;
	doorTime	= 0.0f;
	viewModel	= -1;
	collModel	= -1;
}

void Stage::Draw() const
{
	//安全措置
	if (viewModel < 0)
	{
		return;
	}

	MV1SetPosition	(viewModel, pos);
	MV1SetScale		(viewModel, VGet(scale, scale, scale));
	MV1DrawModel	(viewModel);
	
}

void Stage::DrawCollisionDebug() const
{
	if (collModel < 0)
	{
		return;
	}

	//スケール・位置を反映してからワイヤーフレームで描く
	MV1SetPosition(collModel, pos);
	MV1SetScale(collModel, VGet(collScale, collScale, collScale));  
	MV1SetRotationXYZ(collModel, VGet(0.0f, collYawRad, 0.0f));

	//半透明＋ワイヤーで見やすく（塗りつぶしだと中が見えないため）
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	MV1SetWireFrameDrawFlag(collModel, TRUE);   //ワイヤーフレーム表示ON
	MV1DrawModel(collModel);
	MV1SetWireFrameDrawFlag(collModel, FALSE);  //戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Stage::DrawDoorWallDebug()const
{
	if (!doorWallActive)
	{
		return;   //壁が無効なら描かない
	}

	//扉の壁を、Z=doorWallZ の線に沿った縦の面として描く
	const float x0 = doorWallCenterX - doorWallHalfX;
	const float x1 = doorWallCenterX + doorWallHalfX;
	const float y0 = 0.0f;
	const float y1 = 400.0f;   //壁の高さ（見える範囲）
	const float z = doorWallZ;

	const unsigned int col = GetColor(255, 60, 60);

	//4隅
	VECTOR p00 = VGet(x0, y0, z);
	VECTOR p10 = VGet(x1, y0, z);
	VECTOR p01 = VGet(x0, y1, z);
	VECTOR p11 = VGet(x1, y1, z);

	//枠を線で描く
	DrawLine3D(p00, p10, col);
	DrawLine3D(p10, p11, col);
	DrawLine3D(p11, p01, col);
	DrawLine3D(p01, p00, col);
	//斜めも
	DrawLine3D(p00, p11, col);
	DrawLine3D(p10, p01, col);
}