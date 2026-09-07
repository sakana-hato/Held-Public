#include "DxLib.h"
#include "Precompiled.h"
#include "GameScene.h"
#include "HomingProjectile.h"
#include "ResourceManager.h"
#include "Fader.h"
#include "EffectManager.h"
#include "SoundManager.h"


void GameScene::OnEnter()
{
	elapsedSec = 0.0f;
	isBossDefeated = false;
	isGameOver = false;
    camera.Init();
    camera.SetMode(CameraMode::ThirdPerson);

    //ワールドライト
    light.Apply(); 

    //シャドウマップ
    shadow.Init();
    shadow.SetLightDirection(VGet(Config::Light::LIGHT_DIR_X, Config::Light::LIGHT_DIR_Y, Config::Light::LIGHT_DIR_Z));

    Fader::GetInstance().Init();

    //ポストエフェクト用オフスクリーン
    post.Init(Config::Window::WINDOW_W, Config::Window::WINDOW_H);

    //ステージモデルは ResourceManager
    stage.SetScale(170.0f);
    stage.SetCollisionScale(170.0f);
    stage.SetCollisionYawDeg(180.0f);
    stage.SetViewModel(ResourceManager::Instance().Model("stage"));
    stage.SetCollisionModel(ResourceManager::Instance().Model("stage_collision"));

    //プレイヤー生成
    player = std::make_unique<Player>(ctx_, camera, input, stage, target);
    player->SetModel(ResourceManager::Instance().Model("player_base"), 1.7f);
    player->SetKatana(ResourceManager::Instance().Model("katana"));
    player->SetSheath(ResourceManager::Instance().Model("sheath"));
    //ctx_.player = player.get();

    
    boss = std::make_unique<Boss>(ctx_, camera, stage,*player, projectiles);
    player->SetBoss(boss.get());
    boss->SetModel(ResourceManager::Instance().Model("boss_base"), Config::Boss::MODEL_SCALE);
    boss->SetPosition(VGet(7.0f, 0.0f, -9000.0f));
    boss->ChangeState(BossStateId::Intro);
    boss->SetSword(ResourceManager::Instance().Model("boss_sword"));

    //ctx_.stage = &stage;
    camera.SetStage(&stage);


    //雑魚敵生成
    enemies = std::make_unique<EnemyManager>(ctx_, stage, *player);
    player->SetEnemies(enemies.get());
    enemies->Spawn(ResourceManager::Instance().Model("enemy_base"), 1.7f, VGet(0, 0, -1800));
    //enemies->Spawn(ResourceManager::Instance().Model("enemy_base"), 1.7f, VGet(0, 0, -2200));

    targetMarker.Init(ResourceManager::Instance().Model("target_arrow"), Config::Target::MARKER_SCALE);

    SoundManager::Instance().PlayBgm(BgmId::Normal);

    player->AddHealthObserver(&playerHpBar);
    player->NotifyInitialHp();

    boss->AddHealthObserver(&bossHpBar);
    boss->NotifyInitialHp();

    cutscene    = std::make_unique<CutSceneDirector>(ctx_, camera, input, stage, *player, *boss);
    bossDeath   = std::make_unique<BossDeathDirector>(ctx_, camera, *player, *boss);
}

void GameScene::OnExit()
{
  //もっと増える予定ダッピ
    player. reset();
    stage.  End();
    shadow. End();
    post.   End();

}

void GameScene::Update(float dt)
{
    elapsedSec += dt;
    const float rawDt = dt;

    playerHpBar.Update(dt);
    bossHpBar.  Update(dt);

      //メンバ変数にするのが望ましい
    if (cutscene->IsFinished() && !bossBarStarted)
    {
        bossHpBar.StartRevealAnimation();
        bossBarStarted = true;
    }


    if (cutscene->IsPlaying())
    {
        cutscene->Update(dt);
        stage.Update(dt);
        Fader::GetInstance().Update();
        player->UpdateAnimOnly(dt);
        EffectManager::Instance().Update();
        if (boss)
        {
            boss->Update(dt);
        }
        camera.Update(dt, player->GetPosition(), player->GetForward());
        camera.Apply();
        return;   //通常のゲーム更新はしない
    }

    if (boss && boss->IsDead() && !bossDeath->IsPlaying() && !bossDeath->IsFinished())
    {
        bossDeath->Start();
    }

    if (bossDeath->IsPlaying())
    {
        bossDeath->Update(dt);
        return;   //演出中は通常更新を止める
    }
    
    bool hitStopping = false;
    if (hitStopTimer > 0.0f)
    {
        hitStopTimer -= rawDt;
        if (hitStopTimer < 0.0f)
        {
            hitStopTimer = 0.0f;
        }

        hitStopping = true;
    }

    float normalDt = hitStopping ? 0.0f : dt;

    //ctx_.damageTexts.Update(dt);

    bool reflectSlow = false;
    if (reflectSlowTimer > 0.0f)
    {
        reflectSlowTimer -= rawDt;   //実時間で減らす 
        if (reflectSlowTimer < 0.0f)
        {
            reflectSlowTimer = 0.0f;
        }

        reflectSlow = true;
    }

    //世界の時間
    float worldDt = normalDt;
    if (player && player->IsSlowMoActive())   //ジャスト回避スロー
    {
        worldDt = normalDt * Config::PostEffect::SLOWMO_SCALE;
    }
    if (reflectSlow)   //跳ね返しスロー
    {
        worldDt = normalDt * Config::PostEffect::REFLECT_SLOWMO_SCALE;
    }

    //視点切り替え（Back ボタン）
    if (input.IsPressed(InputAction::ToggleCamera))
    {
        const CameraMode mode = camera.GetMode();
        camera.SetMode(mode == CameraMode::ThirdPerson ? CameraMode::FirstPerson : CameraMode::ThirdPerson);
    }

    EffectManager::Instance().Update();


    if (player)
    {
        bool playerAlsoSlow = false;
        if (player->IsSlowMoActive())
        {
            const float slowElapsed = Config::Player::Evasion::SLOWMO_DURATION - player->GetSlowMoTimer();
            if (slowElapsed < Config::Player::Evasion::PLAYER_SLOW_TIME)
            {
                playerAlsoSlow = true;
            }
        }

        float playerDt;
        if (reflectSlow)
        {
            playerDt = worldDt;   //跳ね返しスローはプレイヤーも世界と同じ
        }
        else if (playerAlsoSlow)
        {
            //ジャスト回避の最初：プレイヤーは少し速いスロー
            playerDt = normalDt * Config::Player::Evasion::PLAYER_SLOWMO_SCALE;
        }
        else
        {
            playerDt = normalDt;   //通常速度
        }

        const float pRaw = hitStopping ? 0.0f : dt;
        if (!awaken.IsPlaying())
        {
            player->Update(playerDt, pRaw);
        }
       

        if (target.IsBossLocked())
        {
            Boss* b = target.GetBossTarget();
            if (b)
            {
                VECTOR tp = b->Comp().pos;
                tp.y += b->Comp().height * 0.5f;  
                camera.SetLockOnTarget(true, tp);
            }
        }
        else if (target.IsLocked())
        {
            Enemy* t = target.GetTarget();
            if (t)
            {
                VECTOR tp = t->Comp().pos;
                tp.y += t->Comp().height * 0.5f;
                camera.SetLockOnTarget(true, tp);
            }
        }
        else
        {
            camera.SetLockOnTarget(false, VGet(0, 0, 0));
        }

        //カメラへ実際のプレイヤー座標／向きを渡す
        camera.Update(dt, player->GetPosition(), player->GetForward());

        if (player->IsDead() && gameOverPhase == GameOverPhase::None)
        {
            gameOverPhase = GameOverPhase::DeathAnim;
            playerHpBar.SetEmptyImmediate();
        }

        UpdateGameOverSequence(dt);
    }

    if (gameOverPhase == GameOverPhase::None)
    {
        if (player && !player->IsDrawingKatana()&& !awaken.IsPlaying())
        {
            enemies->Update(worldDt, rawDt);
        }
    }
   
    if (player)
    {
        enemies->ResolvePlayerCollision(*player);
    }

    if (player && boss && boss->IsActive() && boss->IsAlive() && boss->IsBeamFiring() && !boss->HasHitPlayer())
    {
        const Capsule beam = boss->GetBeamCapsule();
        const Capsule playerBody = player->GetBodyCapsule();

        //カプセル同士の交差判定
        if (CapsuleMath::Intersect(beam, playerBody))
        {
            player->TakeDamage(Config::Boss::Beam::POWER, boss->Comp().pos);
            boss->SetHitPlayer(true);

            VECTOR hitPos = player->GetPosition();
            hitPos.y += 100.0f;
            EffectManager::Instance().Play(ResourceManager::Instance().Effect("hit_damage"), hitPos, Config::Effect::DAMAGE_SCALE);
        }
    }

    //プレイヤーの攻撃判定が有効なら、刀カプセルで敵を殴る
    if (player->IsBladeActive())
    {
        const int attackId = player->GetAttackId();
        const float power = player->GetCurrentAttackPower();
        const bool hit = enemies->CheckPlayerAttack(player->GetBladeCapsule(), power, attackId, player->GetPosition());

        if (hit && hitStopTimer <= 0.0f)
        {
            camera.AddShake(Config::Camera::SHAKE_HIT);
            hitStopTimer = Config::PostEffect::HITSTOP_DURATION;
            
            EffectManager::Instance().Play(ResourceManager::Instance().Effect("hit_slash"),player->GetBladeCapsule().p1,player->GetCurrentHitEffectScale());

        }
    }

    if (player && boss && boss->IsActive() && boss->IsAlive() && player->IsBladeActive())
    {
        const Capsule blade = player->GetBladeCapsule();
        const Capsule bossBody = boss->GetBodyCapsule();

        static int lastBossHitAttackId = -1;
        const int attackId = player->GetAttackId();

        if (attackId != lastBossHitAttackId &&
            CapsuleMath::Intersect(blade, bossBody))
        {
            boss->TakeDamage(player->GetCurrentAttackPower(), player->GetPosition());
            //ctx_.damageTexts.Spawn(player->GetBladeCapsule().p1, player->GetCurrentAttackPower(), DamageTextManager::Kind::ToEnemy);
            lastBossHitAttackId = attackId;

            EffectManager::Instance().Play(ResourceManager::Instance().Effect("hit_slash"),player->GetBladeCapsule().p1,player->GetCurrentHitEffectScale());

            //ヒットストップ・カメラシェイク
            if (hitStopTimer <= 0.0f)
            {
                camera.AddShake(Config::Camera::SHAKE_HIT);
                hitStopTimer = Config::PostEffect::HITSTOP_DURATION;
            }
        }
    }

    //ターゲットロックの入力
    auto& in = input;
    if (in.IsPressed(InputAction::TargetLock))
    {
        //ボス戦中
        if (boss && boss->IsActive() && boss->IsAlive())
        {
            if (target.IsBossLocked())
            {
                target.ClearBoss();  
            }
            else
            {
                target.LockBoss(boss.get());
            }
        }
        else
        {
            target.ToggleLock(*enemies, player->GetPosition());
        }
    }

    //切り替え
    if (in.IsPressed(InputAction::TargetSwitch))
    {
        target.SwitchTarget(*enemies, +1);
    }
    //ターゲット更新（死んだ敵の解除）
    target.Update(*enemies);

    if (target.IsBossLocked() && boss && !boss->IsAlive())
    {
        target.ClearBoss();
    }

    targetMarker.Update(dt);

    if (player)
    {
        VECTOR bossPos = boss ? boss->Comp().pos : VGet(0, 0, 0);
        bossPos.y += 100.0f;   //ボスの胴を狙う
        VECTOR playerPos = player->GetPosition();
        playerPos.y += 100.0f;
        projectiles.Update(dt, playerPos, bossPos);
    }

    if (player && player->ShouldStartAwaken() && !awaken.IsPlaying()
        && player->CurrentStateId() != PlayerStateId::Damaged)  
    {
        awaken.Start();
    }

    awaken.Update(dt);

    //演出が終わったら強化状態へ
    if (awaken.IsFinished() && !player->IsEnhanced())
    {
        player->EnterEnhanced();
    }

    if (player && player->IsEnhanced())
    {
        playerAura.Update(dt, player->GetPosition());
    }
    else
    {
        playerAura.Clear();
    }

    if (player)
    {
        const Capsule playerBody = player->GetBodyCapsule();
        //カプセルの中心（p0とp1の中間）
        const VECTOR capCenter = VScale(VAdd(playerBody.p0, playerBody.p1), 0.5f);

        for (auto& p : projectiles.All())
        {
            if (!p->IsAlive()) continue;

            //敵の弾だけがプレイヤーに当たる
            if (p->GetOwner() != ProjectileOwner::Enemy) continue;

            const VECTOR bulletPos = p->GetPos();
            VECTOR d = VSub(bulletPos, capCenter);
            const float dist = VSize(d);

            //弾とプレイヤーの半径の合計以内なら当たり
            if (dist <= p->GetRadius() + playerBody.radius)
            {
                player->TakeDamage(p->GetPower(), bulletPos);
                EffectManager::Instance().Play(ResourceManager::Instance().Effect("hit_damage"),bulletPos,Config::Effect::DAMAGE_SCALE);
                //ctx_.damageTexts.Spawn(bulletPos, p->GetPower(), DamageTextManager::Kind::ToPlayer);
                p->Kill();
            }
        }
    }

    if (player && player->IsBladeActive() && boss)
    {
        const VECTOR dodgeCenter = player->GetJustDodgeSphereCenter();
        const float  dodgeRadius = player->GetJustDodgeSphereRadius();

        //跳ね返しで発射する弾を、一旦溜める
        std::vector<std::unique_ptr<Projectile>> reflectedBullets;

        for (auto& p : projectiles.All())
        {
            if (!p->IsAlive())
            {
                continue;
            }
            if (p->GetOwner() != ProjectileOwner::Enemy)
            {
                continue;
            }

            const VECTOR bulletPos = p->GetPos();
            VECTOR d = VSub(bulletPos, dodgeCenter);
            const float dist = VSize(d);

            if (dist <= p->GetRadius() + dodgeRadius)
            {
                p->Kill();   //古い弾を消す

                VECTOR toBoss = VSub(boss->Comp().pos, bulletPos);
                toBoss.y = 0.0f;
                const float len = VSize(toBoss);
                if (len > 1e-4f) toBoss = VScale(toBoss, 1.0f / len);

                const float speed = 1200.0f;
                VECTOR vel = VScale(toBoss, speed);

                auto reflected = std::make_unique<HomingProjectile>(
                    bulletPos, vel, ProjectileOwner::Player,
                    p->GetRadius(), p->GetPower() * 1.5f, 5.0f);
                reflected->SetGroundY(0.0f);

                const int inst = EffectManager::Instance().Play(
                    ResourceManager::Instance().Effect("magic_reflected"),
                    bulletPos,
                    Config::Effect::MAGIC_REFLECTED_SCALE);
                reflected->SetEffectInstance(inst);


                //Spawnせず、一旦溜める
                reflectedBullets.push_back(std::move(reflected));

                reflectSlowTimer = Config::PostEffect::REFLECT_SLOWMO_DURATION;
                camera.AddShake(5.0f);

                EffectManager::Instance().Play(ResourceManager::Instance().Effect("parry_flash"),bulletPos,Config::Effect::PARRY_SCALE);
            }
        }

        //ループが終わってから、溜めた弾を追加
        for (auto& b : reflectedBullets)
        {
            projectiles.Spawn(std::move(b));
        }
    }

    if (boss && boss->IsActive() && boss->IsAlive())
    {
        const Capsule bossBody = boss->GetBodyCapsule();
        const VECTOR bossCenter = VScale(VAdd(bossBody.p0, bossBody.p1), 0.5f);

        for (auto& p : projectiles.All())
        {
            if (!p->IsAlive())
            {
                continue;
            }

            if (p->GetOwner() != ProjectileOwner::Player)
            {
                continue;   //跳ね返した弾だけ
            }

            const VECTOR bulletPos = p->GetPos();
            VECTOR d = VSub(bulletPos, bossCenter);
            const float dist = VSize(d);

            if (dist <= p->GetRadius() + bossBody.radius)
            {
                //ボスにダメージ＋怯み
                boss->TakeDamage(p->GetPower(), bulletPos);
                //ctx_.damageTexts.Spawn(bulletPos, p->GetPower(), DamageTextManager::Kind::ToEnemy);

                VECTOR hitPos = bulletPos;
                hitPos.y += Config::Effect::MAGIC_HIT_Y_OFFSET;  
                EffectManager::Instance().Play(ResourceManager::Instance().Effect("magic_hit"),hitPos,Config::Effect::MAGIC_HIT_SCALE);


                boss->EnterStagger();  
                p->Kill();

                //演出
                hitStopTimer = Config::PostEffect::HITSTOP_DURATION;
                camera.AddShake(10.0f);
            }
        }
    }

    //敵の攻撃がプレイヤーのジャスト回避球に入っているか
    if (player && enemies)
    {
        const VECTOR dodgeCenter = player->GetJustDodgeSphereCenter();
        const float  dodgeRadius = player->GetJustDodgeSphereRadius();

        //雑魚の攻撃
        bool incoming = enemies->AnyAttackHitsSphere(dodgeCenter, dodgeRadius);

        //ボスの近接攻撃：攻撃カプセルがジャスト回避球に入ったら
        if (!incoming && boss && boss->IsActive() && boss->IsAlive() && boss->IsAttackActive())
        {
            const Capsule bossAttack = boss->GetAttackCapsule();
            VECTOR d = VSub(bossAttack.p0, dodgeCenter);
            const float dist = VSize(d);
            if (dist <= bossAttack.radius + dodgeRadius)
            {
                incoming = true;
            }
        }

        //ボスのジャンプ攻撃など、カプセルを使わない攻撃：IsJustDodgeWindowで
        if (!incoming && boss && boss->IsActive() && boss->IsAlive())
        {
            if (boss->IsAttackJustDodgeWindow())
            {
                incoming = true;
            }
        }

        player->SetIncomingAttack(incoming);
    }


    if (CheckHitKey(KEY_INPUT_B) && boss && boss->CurrentStateId() == BossStateId::Idle)
    {
        boss->ChangeState(BossStateId::Intro);
    }

    //ボス更新
    if (boss && !awaken.IsPlaying())
    {
        boss->Update(worldDt);   //スロー対応するなら worldDt、しないなら dt
    }

    if (player && boss && boss->IsActive() && boss->IsAlive() && !boss->IsCharging())
    {
        VECTOR pp = player->GetPosition();
        const VECTOR bp = boss->Comp().pos;

        VECTOR d = VSub(pp, bp);
        d.y = 0.0f;
        const float dist = VSize(d);

        const float minDist = player->Comp().radius + boss->Comp().radius;

        if (dist < minDist && dist > 1e-4f)
        {
            //重なっている
            const VECTOR pushDir = VScale(d, 1.0f / dist);  
            const float pushAmount = minDist - dist;
            pp.x += pushDir.x * pushAmount;
            pp.z += pushDir.z * pushAmount;
            player->SetPosition(pp);
        }
    }

    if (player && boss && boss->IsActive() && boss->IsAlive() && boss->IsAttackActive())
    {
        if (!boss->HasHitPlayer())
        {
            const Capsule bossAttack = boss->GetAttackCapsule();
            const Capsule playerBody = player->GetBodyCapsule();

            if (CapsuleMath::Intersect(bossAttack, playerBody))
            {
                player->TakeDamage(Config::Boss::Melee::POWER, boss->Comp().pos);
                boss->SetHitPlayer(true);  

                VECTOR hitPos = player->GetPosition();
                hitPos.y += 100.0f;
                EffectManager::Instance().Play(ResourceManager::Instance().Effect("hit_damage"),hitPos,Config::Effect::DAMAGE_SCALE);
                //ctx_.damageTexts.Spawn(hitPos, Config::Boss::Melee::POWER, DamageTextManager::Kind::ToPlayer);
            }
        }
    }

    {
        const bool slashing =
            boss && boss->IsActive() && boss->IsAlive() &&
            boss->IsSwordDrawn() && boss->IsAttackActive();

        if (slashing)
        {
            //大剣の刃カプセル（根本 先端）
            const Capsule blade = boss->GetSwordBladeCapsule();

            //刃の根本・先端を画面座標へ変換
            const VECTOR rootScreen = ConvWorldPosToScreenPos(blade.p0);
            const VECTOR tipScreen = ConvWorldPosToScreenPos(blade.p1);

        }

      
    }

#if defined(_DEBUG)
    //扉アニメ再生キーボードG / コントローラA
    XINPUT_STATE pad = {};
    GetJoypadXInputState(DX_INPUT_PAD1, &pad);
    const bool doorNow = (CheckHitKey(KEY_INPUT_G) != 0) || (pad.Buttons[XINPUT_BUTTON_A] != 0);

    if (doorNow && !doorTriggerPrev)
    {
        stage.PlayDoor();
    }
    doorTriggerPrev = doorNow;

    if (CheckHitKey(KEY_INPUT_P) && boss)
    {
        VECTOR pos = boss->Comp().pos;
        pos.y += 100.0f;
        VECTOR dir = boss->DirToPlayer();
        VECTOR vel = VScale(dir, 800.0f);
        projectiles.Spawn(std::make_unique<Projectile>(pos, vel, ProjectileOwner::Enemy, 40.0f, 200.0f));
    }

    //Pキーで弾のデバッグ表示をトグル
    //if (ctx_.input.IsKeyDown(KEY_INPUT_P))   //キー入力の取り方は環境に合わせて
    //{
        //ctx_.projectiles.SetDebugDraw(!ctx_.projectiles.IsDebugDraw());
   // }

   
#endif

    //雑魚全滅＋扉の近く＋入力
    if (enemies->AllDead())
    {
        const VECTOR doorPos = VGet(7.0f, 0.0f, -6110.0f);
        VECTOR toDoor = VSub(player->GetPosition(), doorPos);
        toDoor.y = 0.0f;
        if (VSize(toDoor) < Config::Cutscene::TRIGGER_RANGE)
        {
            if (input.IsPressed(InputAction::Interact))
            {
                cutscene->Start();
                return;
            }
        }
    }

    stage.Update(dt);

    post.Update(dt);

    static float prevHp = -1.0f;
    if (player)
    {
        const float hp = player->Comp().hp;
        if (prevHp >= 0.0f && hp < prevHp)
        {
            glitchTimer = Config::PostEffect::GLITCH_DURATION;   // 被弾した瞬間グリッチ開始
        }
        prevHp = hp;
    }

    // グリッチタイマーを減らして、強さに反映
    if (glitchTimer > 0.0f)
    {
        glitchTimer -= dt;
        const float rate = glitchTimer / Config::PostEffect::GLITCH_DURATION;
        post.SetGlitchRate(rate);
        post.SetDamageRate(rate);
    }
    else
    {
        post.SetGlitchRate(0.0f);
        post.SetDamageRate(0.0f);
    }

    if (player)
    {
        static bool prevSlow = false;
        const bool slow = player->IsSlowMoActive();

        //スローが始まった瞬間に波紋開始
        if (slow && !prevSlow)
        {
            post.StartRipple();
        }
        //スローが終わったら波紋解除
        if (!slow && prevSlow)
        {
            post.StopRipple();
        }
        prevSlow = slow;
    }

    //自動抜刀・納刀
    if (player && enemies)
    {
        const bool engagedEnemy = enemies->AnyEnemyEngaged();
        const bool engagedBoss = (boss && boss->IsActive() && boss->IsAlive() && boss->IsPlayerInArena());
        const bool engaged = engagedEnemy || engagedBoss;
        
        if (engagedBoss)
        {
            SoundManager::Instance().ChangeBgm(BgmId::Boss);
        }
        else if (engagedEnemy)
        {
            SoundManager::Instance().ChangeBgm(BgmId::Battle);
        }
        else
        {
            SoundManager::Instance().ChangeBgm(BgmId::Normal);
        }

        if (engaged)
        {
            //交戦中
            combatEndTimer = 0.0f;
            if (!player->IsKatanaDrawn() && !player->IsDrawingKatana())
            {
                player->ToggleKatanaDraw();   //抜刀
            }
        }
        else
        {
            //非交戦
            if (player->IsKatanaDrawn() && !player->IsDrawingKatana())
            {
                combatEndTimer += dt;
                if (combatEndTimer >= Config::Katana::SHEATHE_DELAY)
                {
                    player->ToggleKatanaDraw();   //納刀
                    combatEndTimer = 0.0f;
                }
            }
        }
    }

#if defined(_DEBUG)
    static bool uPrev = false;
    const bool uNow = (CheckHitKey(KEY_INPUT_U) != 0);
    if (uNow && !uPrev && player)
    {
        player->ToggleKatanaDraw();
    }
    uPrev = uNow;

    static bool hPrev = false;
    const bool hNow = (CheckHitKey(KEY_INPUT_H) != 0);
    if (hNow && !hPrev)
    {
        drawCollisionDebug = !drawCollisionDebug;
    }
    hPrev = hNow;
#endif

    //遷移チェック
    if (bossDeath->IsFinished() && !isBossDefeated)
    {
        isBossDefeated = true;
        ctx_.isGameClear = true;               //クリアフラグ
        RequestChange(SceneId::GameClear);     //ゲームクリアシーンへ
    }
    else if (gameOverPhase == GameOverPhase::Done)  
    {
        RequestChange(SceneId::GameOver);
    }
}

void GameScene::UpdateGameOverSequence(float dt)
{
    switch (gameOverPhase)
    {
    case GameOverPhase::None:
        break;

    case GameOverPhase::DeathAnim:
        //死亡アニメが終わったら暗転へ
    {
        auto* dead = static_cast<DeadState*>(player->GetState(PlayerStateId::Dead));
        if (dead && dead->IsAnimFinished())
        {
            gameOverPhase = GameOverPhase::FadeOut;
        }
    }
    break;

    case GameOverPhase::FadeOut:
        fadeAlpha += dt / Config::GameOver::GAMEOVER_FADE_TIME;
        if (fadeAlpha >= 1.0f)
        {
            fadeAlpha = 1.0f;
            gameOverPhase = GameOverPhase::Done;
        }
        break;

    case GameOverPhase::Done:
        break;   //遷移は下の遷移判定で
    }
}

void GameScene::DrawShadowCasters() const
{
    if (player)
    {
        player->Draw();
    }

    enemies->Draw();

    if (boss)
    {
        boss->Draw();
    }

}

void GameScene::Draw()
{
    //影を効かせる範囲をプレイヤー周辺に合わせる
    const VECTOR sha = player ? player->GetPosition() : VGet(0.0f, 0.0f, 0.0f);
    const float  H = Config::Shadow::SHADOW_AREA_HALF;
    shadow.SetArea(VGet(sha.x - H, sha.y - 100.0f, sha.z - H),VGet(sha.x + H, sha.y + H, sha.z + H));
    shadowFloor.SetPlane(sha, H, Config::Graund::GROUND_Y+1.0f); //地面より少し上に置く

    //光から見た深度を書き込む
    shadow.BeginCast();
    DrawShadowCasters();
    shadow.EndCast();

    //本描画
    post.BeginScene();

    camera.Apply();

    SetUseLighting(FALSE);
    stage.Draw();
    SetUseLighting(TRUE);

    shadow.BeginReceive();
    shadowFloor.Draw();
    shadow.EndReceive();

#if defined(_DEBUG)
    stage.DrawDoorWallDebug();
#endif


    if (player)
    {
        player->Draw();
    }

    if (boss)
    {
        boss->Draw();
        boss->DrawWarning();
        boss->DrawBeamWarning();
        boss->DrawChargeWarning();
    }

    if (player && player->IsEnhanced())
    {
        playerAura.Draw();
    }

    enemies->Draw();

    projectiles.Draw();

    if (target.IsBossLocked())
    {
        Boss* b = target.GetBossTarget();
        if (b)
        {
            VECTOR head = b->Comp().pos;
            head.y += b->Comp().height + Config::Boss::BOSS_MARKER_EXTRA;  
            targetMarker.Draw(head);
        }
    }
    else if (target.IsLocked())
    {
        Enemy* t = target.GetTarget();
        if (t)
        {
            VECTOR head = t->Comp().pos;
            head.y += t->Comp().height;
            targetMarker.Draw(head);
        }
    }

    if (drawCollisionDebug)
    {
        stage.DrawCollisionDebug();
    }

    EffectManager::Instance().Draw();


#if defined(_DEBUG)
    light.DrawDebug();
#endif

    //ctx_.damageTexts.UpdateScreenPos();

    post.Composite();

    if (player && player->IsSlowMoActive())
    {
        //3Dカメラを再設定してから描く
        camera.Apply();
        player->Draw();
    }

    if (player->IsSlowMoActive())
    {
        camera.Apply();
        EffectManager::Instance().Draw();
    }
    
    
    awaken.Draw();

    camera.Apply();
    enemies->DrawHpBars();

    playerHpBar.Draw();
    bossHpBar.  Draw();

#if defined(_DEBUG)
  

    DrawString(10, 10, "GameScene - WIP", GetColor(255, 255, 0));

    char buf[64];
    sprintf_s(buf, sizeof(buf), "Time: %.1f sec", elapsedSec);
    DrawString(10, 30, buf, GetColor(200, 200, 200));

    if (player)
    {
        const VECTOR p = player->GetPosition();
        DrawFormatString(10, 100, GetColor(255, 255, 0),
            "Player POS  X:%.1f  Y:%.1f  Z:%.1f", p.x, p.y, p.z);
    }

    DrawFormatString(10, 220, GetColor(0, 255, 255),
        "door time:%.1f playing:%d reverse:%d",
        stage.GetDoorTime(), stage.IsDoorPlaying() ? 1 : 0, stage.IsDoorReverse() ? 1 : 0);


    if (boss) boss->DebugDrawSwordAdjust();   
    if (player) player->DebugDrawKatanaAdjust();

#endif

    if (gameOverPhase == GameOverPhase::FadeOut || gameOverPhase == GameOverPhase::Done)
    {
        const int a = static_cast<int>(fadeAlpha * 255);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
        DrawBox(0, 0, Config::Window::WINDOW_W , Config::Window::WINDOW_H, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    cutscene->Draw();
    Fader::GetInstance().Draw();
}

