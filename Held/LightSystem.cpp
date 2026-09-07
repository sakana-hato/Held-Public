#include "DxLib.h"
#include "LightSystem.h"

void LightSystem::Apply()
{
	SetUseLighting(TRUE);
	Enable(true);

	//1本の平行光源を斜め下向きに
	_dir = VGet(Config::Light::LIGHT_DIR_X, Config::Light::LIGHT_DIR_Y, Config::Light::LIGHT_DIR_Z);
	ChangeLightTypeDir(_dir);

	SetDiffuse(Config::Light::LIGHT_DIF_R, Config::Light::LIGHT_DIF_G, Config::Light::LIGHT_DIF_B);
	SetAmbient(Config::Light::LIGHT_AMB_R, Config::Light::LIGHT_AMB_G, Config::Light::LIGHT_AMB_B);
	SetGlobalAmbient(Config::Light::GLOBAL_AMB_R, Config::Light::GLOBAL_AMB_G, Config::Light::GLOBAL_AMB_B);

	//ハイライトは控えめに
	SetLightSpcColor(GetColorF(0.25f, 0.25f, 0.30f, 0.0f));
}

void LightSystem::DrawDebug() const
{
	//仮のライト位置
	const VECTOR lightPos = VGet(0.0f, 3000.0f, -500.0f);   //シーン中央の上空

	//位置に球を描く（黄色）
	DrawSphere3D(lightPos, 80.0f, 12, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);

	//方向を矢印で描く
	const VECTOR dirEnd = VAdd(lightPos, VScale(_dir, 600.0f));   //方向へ600伸ばす
	DrawLine3D(lightPos, dirEnd, GetColor(255, 200, 0));

	//矢印の先端に小さい球
	DrawSphere3D(dirEnd, 30.0f, 8, GetColor(255, 120, 0), GetColor(255, 120, 0), TRUE);
}
