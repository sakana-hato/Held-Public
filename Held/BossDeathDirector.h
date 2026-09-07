#pragma once
#include "Director.h"

struct SharedContext;
class Boss;
class CameraSystem;
class Player;

class BossDeathDirector :public Director
{
public:
	explicit BossDeathDirector(SharedContext&ctx, CameraSystem& camera,Player& player,Boss&boss)
		:ctx_(ctx), camera(camera), player(player),boss(boss) {}

	void Start();

	void Update(float dt)override;
private:
	/// 演出の進行段階
	enum class Phase
	{
		Falling,       //倒れる（死亡アニメ）
		MagicCircle,   //足元に魔法陣が出る
		Sinking,       //地面の下へ沈む
	};

	/// <summary>
	/// 次のフェーズへ移行する
	/// </summary>
	void EnterPhase(Phase next);

	/// <summary>
	/// 死亡カメラを設定する（アリーナ中心側の斜め上から見下ろす）
	/// </summary>
	void SetupDeathCamera();

	SharedContext& ctx_;
	CameraSystem& camera;
	Player& player;
	Boss& boss;

	Phase  phase = Phase::Falling;
	float  timer = 0.0f;
	float  sinkStartY = 0.0f;              //沈み始めのボスのY座標
	int    magicCircleInstance = -1;       //足元の魔法陣エフェクト
};