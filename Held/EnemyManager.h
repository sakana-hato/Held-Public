#pragma once
#include "Enemy.h"

struct SharedContext;
class Player;
class Stage;


class EnemyManager
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>共通データ
	explicit EnemyManager(SharedContext& ctx,Stage& stage, Player& player)
		: ctx_(ctx), stage(stage), player(player) {}

	/// <summary>
	/// //敵を1体生成して指定位置に置く
	/// </summary>
	/// <param name="modelHandle">	</param>モデルハンドル
	/// <param name="scale">		</param>スケール
	/// <param name="pos">			</param>座標
	Enemy* Spawn(int modelHandle, float scale, const VECTOR& pos);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">		</param>デルタタイム
	/// <param name="deadDt">	</param>死亡
	void Update(float dt, float deadDt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 全敵の頭上HPバーを描く
	/// </summary>
	void DrawHpBars() const;   

	/// <summary>
	/// プレイヤーの刀カプセルと全敵の当たり判定を行う。
	/// </summary>
	/// <param name="blade">		</param>刀
	/// <param name="power">		</param>力
	/// <param name="attackId">		</param>攻撃ID
	/// <param name="attackerPos">	</param>攻撃座標
	bool CheckPlayerAttack(const Capsule& blade, float power, int attackId, const VECTOR& attackerPos);

	/// <summary>
	/// すべての敵が死んだか
	/// </summary>
	bool AllDead() const { return enemies.empty(); }

	/// <summary>
	/// 生きてる敵のカウント
	/// </summary>
	int  AliveCount() const { return static_cast<int>(enemies.size()); }

	/// <summary>
	/// プレイヤーを感知している敵が1体でもいるか
	/// </summary>
	bool AnyEnemyEngaged() const;

	void ResolvePlayerCollision(Player& player);

	int    AliveEnemyList(std::vector<Enemy*>& out) const;
	Enemy* NextAliveEnemy(Enemy* current, int dir) const;

	/// <summary>
	/// いずれかの敵の攻撃判定が、指定球に入っているか
	/// </summary>
	/// <param name="center"></param>中央
	/// <param name="radius"></param>半径
	bool AnyAttackHitsSphere(const VECTOR& center, float radius) const;

	Enemy* FindNearest(const VECTOR& from, float maxRange) const;

	void Clear() { enemies.clear(); }

	VECTOR LastHitPos() const { return lastHitPos; }

private:
	Stage& stage;
	Player& player;
	SharedContext& ctx_;//共通データ
	std::vector<std::unique_ptr<Enemy>> enemies;

	//二重ヒット防止
	int lastAttackId_ = -1;
	std::vector<Enemy*> hitThisAttack_;

	VECTOR lastHitPos = VGet(0.0f, 0.0f, 0.0f);
};