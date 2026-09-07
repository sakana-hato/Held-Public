#pragma once
#include "Scene.h"
#include "SharedContext.h"
#include "CameraSystem.h"
#include "InputSystem.h"
#include "TargetSystem.h"
#include "ProjectileManager.h"

#include "Player.h"
#include "Stage.h"
#include "LightSystem.h"
#include "ShadowMap.h"
#include "ShadowMapReceiver.h"
#include "PostEffect.h"
#include "TargetMarker.h"
#include "EnemyManager.h"
#include "CutsceneDirector.h"
#include "Boss.h"
#include "PlayerAwakenDirector.h"
#include "PlayerAura.h"
#include "PlayerHpUi.h"
#include "BossHpUI.h"
#include "BossDeathDirector.h"

/// <summary>
/// ゲームオーバ処理
/// </summary>
enum class GameOverPhase
{
    None,
    DeathAnim,   //死亡アニメ再生中
    FadeOut,     //暗転中
    Done,        //暗転完了　遷移する
};

/// <summary>
/// ゲームシーン　
/// </summary>
class GameScene final : public Scene
{
public:

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="ctx"></param>共通データ
    explicit GameScene(SharedContext& ctx, InputSystem& input) 
        : Scene(input), ctx_(ctx) {}

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GameScene() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void OnEnter() override;

    /// <summary>
    /// 終了
    /// </summary>
    void OnExit()  override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="dt"></param>デルタタイム
    void Update(float dt) override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw()    override;
private:
  
    /// <summary>
    /// シャドウマップを使用するモデル描画
    /// </summary>
    void DrawShadowCasters() const;


    void UpdateGameOverSequence(float dt);

    
    SharedContext& ctx_;//共通データ

    std::unique_ptr<Player> player;

    std::unique_ptr<EnemyManager> enemies;

    std::unique_ptr<Boss> boss;

    std::unique_ptr<CutSceneDirector> cutscene;

    std::unique_ptr<BossDeathDirector> bossDeath;

    CameraSystem camera;

    Stage                   stage;
    LightSystem             light;
    ShadowMap               shadow;
    ShadowReceiver          shadowFloor;
    PostEffect              post;
    TargetMarker            targetMarker;
    TargetSystem            target;
    //CutSceneDirector        cutscene{ ctx_, camera,input, stage, *player };
    GameOverPhase           gameOverPhase = GameOverPhase::None;
    PlayerAwakenDirector    awaken;
    PlayerAura              playerAura;
    PlayerHpUI              playerHpBar;
    BossHpBarUI             bossHpBar;
    ProjectileManager       projectiles;
   // BossDeathDirector       bossDeath{ ctx_, camera };

    float combatEndTimer    = 0.0f;     //戦闘終了からの経過
    float hitStopTimer      = 0.0f;     //hitstopの時間
    float fadeAlpha         = 0.0f;     //暗転の濃さ（0から1）
    float elapsedSec        = 0.0f;     //ゲーム開始からの経過時間
    float glitchTimer       = 0.0f;
    float reflectSlowTimer  = 0.0f;
  
    bool isBossDefeated         = false;
    bool isGameOver             = false;
    bool doorTriggerPrev        = false; //扉再生トリガー
    bool drawCollisionDebug     = false; //Hキーでコリジョン表示
    bool awakenTriggered        = false;
    bool bossBarStarted         = false;
};