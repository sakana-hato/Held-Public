#pragma once
#include "Director.h"

class CameraSystem;
class Player;

class TitleDirector :public Director
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera"></param>カメラ
	/// <param name="player"></param>プレイヤー
	explicit TitleDirector(CameraSystem& camera,Player& player)
		:camera(camera),player(player){ }

	void Start();

	void SkipToEnd();

	void Update(float dt)override;

	bool IsLogoVisible()const { return logoVisible; }

private:
	enum class Phase
	{
		Stand,
		Walking,
		ArriveDoor,
		DrawSword,
	};

	void EnterPhase(Phase next);

	void UpdateWalkCamera(float progress);

	void SetDoorCamera();

	void StopFootstep();

	CameraSystem& camera;
	Player& player;

	Phase phase			= Phase::Walking;
	float timer			= 0.0f;
	bool  logoVisible	= false;   //ロゴを出してよいか
	bool  swordDrawn	= false;   //抜刀を実行したか
	bool footstepPlaying = false;   //足音をループ再生中か
	bool voicePlayed = false;   //セリフを再生したか
};
