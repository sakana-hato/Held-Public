#pragma once

/// <summary>
/// ゲームステータス
/// csvにすればよかった
/// </summary>
namespace Config 
{
	//ウィンドウ関連
	namespace Window
	{
		
		constexpr int WINDOW_W		= 1280;	// windowの横の長さ
		constexpr int WINDOW_H		= 720;	// windowの縦の長さ
		constexpr int COLOR_DEPTH	= 32;	// 色深度
	}
	
	//フレームレート関連
	namespace FrameRate
	{
		constexpr double FIXED_DT		= 1.0 / 90.0;	// 固定ステップ
		constexpr double FRAME_CLAMP	= 0.25;			// スパイク時の上限
	}
	
	//地面関連
	namespace Graund
	{
		constexpr float GROUND_Y = 0.0f; // 地面の高さ
	}
	
	//扉アニメション関連
	namespace Door
	{
		constexpr int DOOR_ANIM_A		= 0;		// 扉アニメ番号
		constexpr int DOOR_ANIM_B		= 3;		// 扉アニメ番号
		constexpr float DOOR_ANIM_SPEED = 0.2f;		// 扉アニメの速度
	}

	//エフェクト関連
	namespace Effect
	{ 
		constexpr int   MAX_PARTICLES			= 15000;	// パーティクル上限
		constexpr float HIT_SCALE_1				= 3.0f;		// 1段目ヒットエフェクト
		constexpr float HIT_SCALE_2				= 5.0f;		// 2段目ヒットエフェクト
		constexpr float HIT_SCALE_3				= 7.0f;		// 3段目ヒットエフェクト
		constexpr float JUMP_ATTACK_HIT_SCALE	= 9.0f;		// 落下攻撃のヒットエフェクト
		constexpr float PARRY_SCALE				= 2.0f;		// 魔法跳ね返しのエフェクト
		constexpr float JUST_DODGE_SCALE		= 15.0f;	// ジャスト回避エフェクト
		constexpr float MAGIC_HIT_SCALE			= 3.0f;		// 魔法がボスに当たったエフェクト
		constexpr float MAGIC_HIT_Y_OFFSET		= 200.0f;   // 魔法ヒットエフェクトの高さ調整
		constexpr float JUMP_LANDING_SCALE		= 2.0f;		// 落下攻撃の着地エフェクト
		constexpr float JUMP_DUST_SCALE			= 2.0f;		// ジャンプの踏み切りエフェクト
		constexpr float DAMAGE_SCALE			= 5.0f;		// 被弾
		constexpr float BOSS_APPEAR_SCALE		= 7.0f;		// 出現魔法陣
		constexpr float BEAM_SCALE				= 2.0f;		// ビーム攻撃
		constexpr float CHARGE_WIND_SCALE		= 3.0f;     // 突進エフェクト
		constexpr float BOSS_SLAM_SCALE			= 5.0f;		// ボスの地面叩きつけエフェクト
		constexpr float MAGIC_ORB_SCALE			= 1.0f;		// 魔法弾のエフェクト
		constexpr float	MAGIC_REFLECTED_SCALE	= 1.0f;		// 跳ね返した魔法弾のエフェクト
		constexpr float BOSS_MAGIC_CIRCLE_SCALE = 2.0f;
	}

	//カメラ関連
	namespace Camera
	{
		constexpr float CAM_NEAR				= 10.0f;		// カメラ描画最小近距離
		constexpr float CAM_FAR					= 15000.0f;		// カメラ描画最大距離
		constexpr float CAM_YAW					= 180.0f;		// カメラの初期回転
		constexpr float CAM_PITCH				= 20.0f;		// カメラのピッチ
		constexpr float CAM_DISTANCE			= 450.0f;		// プレイヤーからのカメラの距離
		constexpr float CAM_FOV					= 80.0f;		// カメラの描画の大きさ
		constexpr float CAM_PIVOT_UP			= 60.0f;		// カメラ壁判定の注視点を上げる量
		constexpr float CAM_WALL_MARGIN			= 500.0f;		// 壁からカメラを離す余白
		constexpr float CAM_FADE_NEAR			= 120.0f;		// これより近いとプレイヤー完全透明
		constexpr float CAM_FADE_FAR			= 60.0f;		// これより遠いと完全不透明
		constexpr float CAM_SPHERE_RADIUS		= 80.0f;		// カメラの当たり球の半径
		constexpr float CAM_LOOK_UP				= 180.0f;		// 注視点を足元から上げる量（胸 頭の高さ）
		constexpr float CAM_GROUND_CLEARANCE	= 100.0f;		// カメラを地面から最低これだけ浮かせる
		constexpr float SHAKE_HIT				= 8.0f;			// 攻撃ヒット時の揺れの強さ
		constexpr float SHAKE_DECAY				= 40.0f;		// 揺れの減衰速度（大きいほど早く収まる）
		constexpr float SHAKE_MAX				= 30.0f;		// 揺れの強さの上限
		constexpr float SHOULDER_OFFSET			= 0.0f;			// 肩越しの横ずらし量（プラスで右にカメラ プレイヤーが左寄り)
		constexpr float LOCKON_LERP				= 12.0f;		// ターゲット追従の速さ
	}
	
	//アニメーション再生フレーム関連
	namespace Anime
	{
		constexpr float ANIM_FPS			= 60.0f;	// アニメション再生フレーム
		constexpr float ANIM_BLEND_DEFAULT	= 0.15f;	// アニメションブレンドフレーム

	}

	//ポストエフェクト関連
	namespace PostEffect
	{
		//常にかけるエフェクト
		constexpr int POST_TINT_R				= 190;	//R
		constexpr int POST_TINT_G				= 190;	//G
		constexpr int POST_TINT_B				= 190;	//B
		constexpr float GLITCH_DURATION			= 0.5f;	//グリット線時間
		constexpr float RIPPLE_DURATION			= 1.0f;	//スロー演出の時間
		constexpr float HITSTOP_DURATION		= 0.1f;	//攻撃ヒット時の演出時間

		//モノクロエフェクト
		constexpr float SLOWMO_SCALE			= 0.2f;	//スロー中の速度倍率
		constexpr float SLOWMO_DURATION			= 0.5f; //スローの持続時間
		constexpr float REFLECT_SLOWMO_DURATION = 0.5f;	//スローから戻る際の速度倍率
		constexpr float REFLECT_SLOWMO_SCALE	= 0.3f;	//スローから戻る際の持続時間
		constexpr float RIPPLE_SHRINK_DURATION	= 0.7f; //縮んで戻る時間
	}
	
	//UI関連
	namespace UI
	{
		namespace PlayerHpBar
		{
			constexpr int   BAR_X			= 340;    // バーの左端X座標
			constexpr int   BAR_Y			= 670;    // バーの上端Y座標
			constexpr int   BAR_W			= 600;    // バーの幅
			constexpr int   BAR_H			= 20;     // バーの高さ
			constexpr int   FRAME_MARGIN	= 6;      // 枠まわりの余白
			constexpr int   FONT_SIZE		= 15;     // 数字のフォントサイズ
			constexpr float FOLLOW_SPEED	= 5.0f;   // バーが減るときの追従速度
		}

		namespace BossHpBar
		{
			constexpr int   BAR_X			= 240;    //バーの左端X
			constexpr int   BAR_Y			= 60;     //バーのY（画面上部）
			constexpr int   BAR_W			= 800;    //バーの幅（横長）
			constexpr int   BAR_H			= 22;     //バーの高さ
			constexpr int   FRAME_MARGIN	= 3;      //枠の余白
			constexpr int   FONT_SIZE		= 18;     //数字フォント
			constexpr int   NAME_FONT_SIZE	= 24;     //名前フォント
			constexpr float FOLLOW_SPEED	= 8.0f;   //減る追従速度
			constexpr float REVEAL_TIME		= 2.0f;   //みなぎる演出の時間（秒）
			constexpr char  BOSS_NAME[]		= "鹿王　アドルフ";   //ボス名
		}
	}

	//岩エフェクト関連
	namespace RockRing
	{
		constexpr float RISE_TIME			= 0.2f;		// せり上がる時間
		constexpr float HOLD_TIME			= 1.0f;		// 地面で維持する時間
		constexpr float SINK_TIME			= 0.5f;		// 沈む時間
		constexpr float RISE_DEPTH			= 300.0f;	// 地面下からの深さ
		constexpr float MODEL_BASE_RADIUS	= 300.0f;	// モデルの大きさ
	}
	
	//プレイヤー関連
	namespace Player
	{
		//待機状態関連
		namespace Idle
		{
			constexpr float IDLE_ACT_WAIT_MIN	= 7.0f;		// 操作していない秒数
			constexpr float IDLE_ACT_WAIT_MAX	= 12.0f;	// 何秒で待機モーションに移行するか
			constexpr int   IDLE_ACT_COUNT		= 3;		// 待機モーションの種類数
		}
		
		//プレイヤー基本ステータス関連
		namespace Status
		{
			constexpr float PLAYER_HP_MAX				= 1000.0f;		// プレイヤーHP
			constexpr float PLAYER_HP_REGEN				= 0.5f;			// 自動HP回復量（簡）
			constexpr float PLAYER_ENHANCE_THRESHOLD	= 0.20f;		// プレイヤー強化状態HP割合
			constexpr float GRAVITY						= 1400;		    // 重力
		}
		
		//プレイヤー基本移動関連
		namespace Move
		{
			constexpr float PLAYER_MOVE_SPEED		= 500.0f;	// プレイヤーの通常速度
			constexpr float PLAYER_DASH_SPEED		= 750.0f;	// プレイヤーのダッシュ速度
			constexpr float PLAYER_ROTATE_SPEED		= 700.0f;	// プレイヤーの回転速度
			constexpr float PLAYER_JUMP_SPEED		= 600.0f;	// プレイヤーのジャンプのスピード
			constexpr float AIR_CONTROL				= 1.0f;		// プレイヤーの空中での移動量
			constexpr float RUNSTOP_DURATION		= 0.92f;	// プレイヤーの走り止まりモーションの長さ
		}
		
		//プレイヤー回避関連(ジャスト回避)
		namespace Evasion
		{
			constexpr float DODGE_SPEED					= 1800.0f;		// 回避開始速度
			constexpr float DODGE_DURATION				= 0.50f;		// 回避開始秒数
			constexpr float DODGE_INVINCIBLE			= 0.20f;		// プレイヤー無敵時間
			constexpr float JUST_DODGE_WINDOW			= 0.15f;		// ジャスト受付
			constexpr float SLOWMO_DURATION				= 7.0f;			// スロー継続時間
			constexpr float PLAYER_SLOWMO_SCALE			= 0.2f;			// プレイヤーのスロー割合
			constexpr float SLOWMO_SCALE				= 0.20f;		// プレイヤーの以外のスロー割合
			constexpr float JUST_DODGE_SPHERE_RADIUS	= 200.0f;		// ジャスト回避の球サイズ
			constexpr float BACK_DODGE_ANIM_SPEED		= 2.5f;			// 後ろ回避のアニメーション再生速度
			constexpr float PLAYER_SLOW_TIME			= 1.6f;			// プレイヤーのスロー時間
			constexpr float DODGE_FX_FORWARD			= 600.0f;		// 回避エフェクトを前に出す距離
			constexpr float DODGE_FX_HEIGHT				= 80.0f;		// 回避エフェクトの高さ
			constexpr float DODGE_FX_SCALE				= 4.0f;			// 回避エフェクトのスケール
		}
		
		//プレイヤーの当たり判定関連
		namespace Hit
		{
			constexpr float PLAYER_RADIUS			= 70.0f;	// プレイヤーのカプセルの半径
			constexpr float PLAYER_HEIGHT			= 240.0f;	// プレイヤーの高さ
			constexpr float DAMAGED_DURATION		= 0.40f;	// のけぞり時間
			constexpr float DAMAGED_KNOCKBACK		= 1200.0f;  // のけぞりで押される初速
		}

		//プレイヤーのジャスト回避のラッシュ攻撃関連
		namespace Counter
		{
			constexpr float HIT_INTERVAL	= 0.20f;		//ダメージが入る間隔（秒）
			constexpr int   HIT_COUNT		= 5;			//ヒット回数
			constexpr float HIT_RADIUS		= 500.0f;		//ダメージの効果範囲
			constexpr float HIT_DAMAGE_MUL	= 5.0f;			//1ヒットあたりのダメージ倍率
			constexpr float EFFECT_A_SCALE	= 3.0f;			//エフェクトAのスケール
			constexpr float EFFECT_B_SCALE	= 3.0f;			//エフェクトBのスケール
			constexpr float HIT_FX_SCALE	= 8.0f;			//ラッシュのヒットエフェクトのスケール
			constexpr float FINISH_OFFSET	= 100.0f;		//ラッシュ攻撃後プレイヤーが敵のどの程度前に出るか
			constexpr float SEARCH_MAX		= 100000.0f;	//敵探索の最大距離
		}
		
		//プレイヤーの攻撃関連
		namespace Attack
		{
			//攻撃コンボ
			constexpr int   COMBO_MAX				= 3;		// 最大コンボ段数
			constexpr float ATTACK_DURATION			= 0.45f;	// 1段あたりの時間
			constexpr float COMBO_INPUT_OPEN		= 0.0f;		// 次入力の受付開始(段開始から)
			constexpr float COMBO_INPUT_CLOSE		= 0.90f;	// 次入力の受付終了
			constexpr float ATTACK_ANIM_SPEED		= 1.7f;		// 攻撃アニメの再生速度1.7
			constexpr float ATTACK_STEP_SPEED		= 120.0f;	// 攻撃中の前進
			constexpr float ATTACK_STEP_DISTANCE	= 350.0f;   // 踏み込む距離
			constexpr float ATTACK_STEP_TIME		= 0.3f;		// 踏み込む時間

			//コンボ1段目
			constexpr float ATTACK1_HIT_START		= 0.15f;// 攻撃判定開始
			constexpr float ATTACK1_HIT_END			= 0.60f;// 攻撃判定終了
			//コンボ2段目
			constexpr float ATTACK2_HIT_START		= 0.40f;// 攻撃判定開始
			constexpr float ATTACK2_HIT_END			= 0.80f;// 攻撃判定終了
			//コンボ3段目
			constexpr float ATTACK3_HIT_START		= 0.40f;// 攻撃判定開始
			constexpr float ATTACK3_HIT_END			= 0.80f;// 攻撃判定終了
			
			//ジャンプ攻撃
			constexpr float JUMP_ATTACK_FALL_SPEED		= 1600.0f;	// 急降下速度
			constexpr float JUMP_ATTACK_RECOVERY		= 0.50f;	// 着地後の硬直
			constexpr float JUMP_ATTACK_RISE_SPEED		= 600.0f;   // 落下攻撃の上昇初速
			constexpr float JUMP_ATTACK_RISE_GRAVITY	= 1200.0f;  // 上昇の減速
			constexpr float JUMP_ATTACK_RISE_TIME		= 0.4f;     // 上昇フェーズの時間
			constexpr float JUMP_ATTACK_ANIM_SPEED		= 1.5f;		// ジャンプ攻撃のアニメーション再生速度
			constexpr float JUMP_ATTACK_HIT_START		= 0.3f;		// 攻撃判定が開始時間
			constexpr float JUMP_ATTACK_HIT_END			= 0.8f;		// 攻撃判定が終了時間
			constexpr float JUMP_ATTACK_HOMING_RANGE	= 500.0f;   // 吸い付く範囲
			constexpr float JUMP_ATTACK_HOMING_SPEED	= 200.0f;   // 吸い付く速度
			constexpr float JUMP_ATTACK_HOMING_TURN		= 8.0f;		// 向きの補間速度
			constexpr float JUMP_ATTACK_HOMING_LERP		= 7.0f;		// 位置の補間の強さ

			//プレイヤー通常攻撃の吸い付き処理
			constexpr float PLAYER_ENHANCE_MUL			= 1.5f;		// 強化状態の攻撃倍率
			constexpr float ATTACK_HOMING_RANGE			= 600.0f;   // この範囲内の敵に吸い付く
			constexpr float ATTACK_HOMING_RANGE_BOSS	= 4000.0f;  // ボス用の吸い付き範囲
			constexpr float ATTACK_HOMING_FACE			= 1.0f;     // 向きの吸い付き強度(0=無効,1=完全に敵を向く)
			constexpr float ATTACK_HOMING_MOVE			= 1.0f;     // 移動の吸い付き強度(0=正面,1=完全に敵方向)

			//コンボの攻撃力パラメーター
			constexpr float ATTACK_POWER_1			= 20.0f;	// 1段目
			constexpr float ATTACK_POWER_2			= 25.0f;	// 2段目
			constexpr float ATTACK_POWER_3			= 40.0f;	// 3段目
			constexpr float JUMP_ATTACK_POWER		= 500.0f;	// 落下攻撃
			constexpr float ENHANCED_DAMAGE_MULT	= 1.5;		// 強化状態の攻撃倍率
		}

		//強化状態関連
		namespace Awaken
		{
			constexpr float FADE_TIME		= 1.0f;   //暗転にかける時間
			constexpr float TEXT1_TIME		= 1.2f;   //「私はまだ」を見せる時間
			constexpr float TEXT2_TIME		= 1.5f;   //「負けたくない」を見せる時間
			constexpr float CRACK_TIME		= 0.6f;   //ヒビが入る時間
			constexpr float SHATTER_TIME	= 1.0f;   //割れて落ちる時間
		}
		
		//必殺技関連
		namespace Ult
		{
			constexpr float ULT_GAUGE_MAX				= 100.0f;	//必殺技ゲージ最大
			constexpr float ULT_GAUGE_REGEN				= 3.0f;		//必殺技ゲージ経過時間増加量
			constexpr float ULT_GAUGE_REGEN_ENHANCED	= 6.0f;		//強化状態でのゲージ増加量
			constexpr float ULT_GAUGE_HIT				= 5.0f;		//敵に攻撃HIT時に溜まるゲージ量
			constexpr float ULTIMATE_RUSH_SPEED			= 1200.0f;	//突進速度
			constexpr float ULTIMATE_DURATION			= 0.80f;	//突進時間
		}
		
	}
	
	//ボスステータス関連(分けたほうが良い)
	namespace Boss
	{
		//サイズ
		constexpr float RADIUS			= 200.0f;			// 半径
		constexpr float HEIGHT			= 700.0f;			// 高さ
		constexpr float MODEL_SCALE		= 3.0f;				// モデルのサイズ

		//HP
		constexpr float BOSS_HP_EASE	= 3000.0f;			// 簡単モードのHP
		constexpr float BOSS_HP_NORMAL	= 5000.0f;			// 普通モードのHP
		constexpr float BOSS_HP_HARD	= 7000.0f;			// 難しいモードのHP
		constexpr float HP_MAX			= BOSS_HP_NORMAL;	// 今はノーマルHP

		//移動・旋回
		constexpr float MOVE_SPEED		= 700.0f;			// 移動速度
		constexpr float TURN_SPEED		= 300.0f;			// 旋回速度
		
		//プレイヤーとの距離
		constexpr float STOP_DISTANCE	= 500.0f;			// この距離で止まる
		constexpr float ATTACK_RANGE	= 560.0f;			// この距離で攻撃
		constexpr float CHASE_RANGE		= 650.0f;			// この距離でチェス平行する

		//攻撃間隔
		constexpr float ATTACK_COOLDOWN_P1 = 3.0f;			// フェーズ1
		constexpr float ATTACK_COOLDOWN_P2 = 2.2f;			// フェーズ2
		constexpr float ATTACK_COOLDOWN_P3 = 1.5f;			// フェーズ3

		//ボスの攻撃の種類
		constexpr int ATTACK_COUNT_EASE		= 3;			// 簡単モードの攻撃数
		constexpr int ATTACK_COUNT_NORMAL	= 5;			// 普通モードの攻撃数
		constexpr int ATTACK_COUNT_HARD		= 7;			// 難しいモードの攻撃数

		//フェーズのHP境界
		constexpr float PHASE2_HP_RATE		= 0.66f;		// HP66%以下でフェーズ2
		constexpr float PHASE3_HP_RATE		= 0.33f;		// HP33%以下でフェーズ3

		//被弾・死亡
		constexpr float DAMAGED_DURATION	= 0.4f;			// 被弾時間
		constexpr float DEAD_DURATION		= 3.0f;			// 死亡時間

		//出現演出
		constexpr float INTRO_RISE_TIME		= 7.0f;			// 地面から出てくる時間
		constexpr float INTRO_RISE_DEPTH	= 600.0f;		// 地下からの深さ
		constexpr float INTRO_DELAY			= 1.5f;			// 魔法陣が広がってから、せり上がり開始までの待ち

		//ボスカメラの位置
		constexpr float BOSS_CAM_POS_X		= 7.0f;			// 横
		constexpr float BOSS_CAM_POS_Y		= 200.0f;		// 高さ
		constexpr float BOSS_CAM_POS_Z		= -8400.0f;		// ボスの手前
		constexpr float BOSS_CAM_TGT_X		= 7.0f;			// 横
		constexpr float BOSS_CAM_TGT_Y		= 200.0f;		// ボスの上半身を見上げる
		constexpr float BOSS_CAM_TGT_Z		= -10000.0f;    // ボスの位置
		constexpr float BOSS_ROAR_TIME		= 6.0f;			// 咆哮の長さ
		constexpr float ARENA_CENTER_X		= 7.0f;			// ボス部屋の中心X
		constexpr float ARENA_CENTER_Z		= -10000.0f;	// ボス部屋の中心Z
		constexpr float ARENA_RADIUS		= 3700.0f;		// 中心からの行動半径

		constexpr float BOSS_MARKER_EXTRA	= 20.0f;		//	ターゲットの矢印の高さ

		constexpr float STAGGER_DURATION	= 4.0f;			//	フェーズ2への移行時間

		//近接攻撃
		namespace Melee
		{
			constexpr float DURATION		= 2.0f;			// 攻撃アニメ全体
			constexpr float HIT_START		= 0.6f;			// 判定ON
			constexpr float HIT_END			= 1.7f;			// 判定OFF 
			constexpr float POWER			= 30.0f;		// ダメージ
			constexpr float HAND_RADIUS		= 180.0f;		// 手の攻撃判定の太さ
		}

		//ジャンプ攻撃
		namespace Jump
		{
			constexpr float BACK_DISTANCE	= 1200.0f;		// 離れる距離
			constexpr float BACK_SPEED		= 500.0f;		// 離れる速度
			constexpr float CHARGE_TIME		= 0.3f;			// アニメの溜め部分の長さ
			constexpr float JUMP_TIME		= 1.3f;			// 実際に飛んでいる時間
			constexpr float JUMP_HEIGHT		= 800.0f;		// ジャンプの高さ
			constexpr float LAND_RADIUS		= 1200.0f;		// 攻撃範囲
			constexpr float POWER			= 40.0f;		// 攻撃力
			constexpr float BACK_MAX_TIME	= 2.0f;			// バックする最大時間
		}

		//地面範囲攻撃
		namespace Shockwave
		{
			constexpr int   COUNT			= 3;			// 連続回数
			constexpr float CHARGE_TIME		= 1.8f;			// 1回目の溜め
			constexpr float REPEAT_TIME		= 1.8f;			// 2回目以降の溜め
			constexpr float RADIUS			= 1500.0f;		// 攻撃半径
			constexpr float POWER			= 50.0f;		// 攻撃力
			constexpr float WARNING_TIME	= 1.3f;			// 警告時間
		}

		//大剣装備
		namespace DrawSword
		{
			constexpr float ROAR_TIME		= 4.0f;			// 咆哮の長さ
			constexpr float PUSH_TIME		= 3.0f;			// プレイヤーを押す時間
			constexpr float PUSH_RANGE		= 3000.0f;		// この距離以内のプレイヤーを押す
			constexpr float PUSH_SPEED		= 1000.0f;		// 押す速度（近いほど強い）
			constexpr float DRAW_TIME		= 4.6f;			// この時間で大剣を抜く
			constexpr float DRAW_ANIM_START = 5.0f;			// 剣を抜くアニメーション開始時間
			constexpr float TOTAL_TIME		= 6.0f;			// 演出全体の長さ
			constexpr float DRAW_ANIM_SPEED = 0.8f;			// アニメーション再生スピード
		}

		//エクスカリバー攻撃
		namespace Beam
		{
			constexpr float BACK_DISTANCE	= 1000.0f;		// 撃つ前に離れる距離
			constexpr float BACK_SPEED		= 700.0f;		// 離れる速度
			constexpr float CHARGE_TIME		= 3.0f;			// ためる時間
			constexpr float FIRE_TIME		= 1.0f;			// 斬撃が飛んでいる時間
			constexpr float LENGTH			= 3000.0f;		// 斬撃の長さ
			constexpr float RADIUS			= 150.0f;		// 斬撃の太さ
			constexpr float POWER			= 600.0f;		// ダメージ
		}

		//魔法玉攻撃
		namespace Magic
		{
			constexpr float BACK_DISTANCE	= 1500.0f;		// 撃つ前に離れる距離
			constexpr float BACK_SPEED		= 800.0f;		// 離れる速度
			constexpr float BACK_MAX_TIME	= 2.0f;			// 後退の最大時間
			constexpr float CHARGE_TIME		= 0.0f;			// 撃つ前の溜め
			constexpr float SHOT_CHARGE		= 2.0f;			// 打っているときの溜め
			constexpr int   SHOT_COUNT		= 3;			// 撃つ弾の数
			constexpr float SHOT_SPEED		= 2000.0f;		// 弾の速度
			constexpr float SHOT_RADIUS		= 90.0f;		// 弾の大きさ
			constexpr float SHOT_POWER		= 250.0f;		// 弾のダメージ
			constexpr float ANIM_SPEED		= 0.8f;			// アニメーション再生速度
			constexpr float HOMING_TURN		= 2.0f;			// ホーミングの強さ
			constexpr float CAST_TO_FIRE	= 0.3f;			// アニメ再生から発射まで
			constexpr float SHOT_INTERVAL	= 1.8f;			// 発射後、次の発射までの間隔
		}

		//突進大剣攻撃
		namespace Charge
		{
			constexpr float ORBIT_RADIUS		= 2500.0f;		// 外周の半径
			constexpr float ORBIT_SPEED			= 2500.0f;		// 外周を走る速度
			constexpr float DASH_SPEED			= 10000.0f;		// 突進の速度
			constexpr float DASH_TRIGGER		= 5.0f;			// 外周をこれだけ進んだら突進
			constexpr int   DASH_COUNT			= 3;			// 突進する回数
			constexpr float DASH_END_DIST		= 400.0f;		// 外周からこの距離まで戻ったら外周再開
			constexpr float POWER				= 400.0f;		// 攻撃力(未使用)
			constexpr float AFTERIMAGE_INTERVAL = 0.2f;			// 残像を出す間隔
			constexpr float AFTERIMAGE_LIFE		= 1.0f;			// 残像の生存時間
			constexpr float AFTERIMAGE_ALPHA	= 1.0f;			// 残像の濃さ（1.0で不透明、0.5で半透明）

			//残像の色
			constexpr float AI_COLOR_START_R	= 0.2f;			// 残像の最初の頃のR
			constexpr float AI_COLOR_START_G	= 0.7f;			// 残像の最初の頃のG
			constexpr float AI_COLOR_START_B	= 1.0f;			// 残像の最初の頃のB
			constexpr float AI_COLOR_END_R		= 0.5f;			// 残像の最後の頃のR
			constexpr float AI_COLOR_END_G		= 1.0f;			// 残像の最後の頃のG
			constexpr float AI_COLOR_END_B		= 0.3f;			// 残像の最後の頃のB

			constexpr float AIM_TIME			= 0.6f;			// 突進前に狙いを定める時間
			constexpr float DASH_WIDTH			= 200.0f;		// 突進の警告帯の幅
			constexpr float DASH_LENGTH			= 6000.0f;		// 警告帯の長さ

			constexpr float WIND_EFFECT_Y		= 200.0f;		// エフェクトの高さ(未使用)
			constexpr float WIND_EFFECT_FORWARD = 100.0f;		// ボスの正面にどれだけ前に出すか(未使用)
		}

		namespace Death
		{
			constexpr float CAM_BACK = 300.0f;   //カメラを中心方向へ引く水平距離
			constexpr float CAM_HEIGHT = 800.0f;   //カメラの高さ（斜め上から見下ろす）
			constexpr float CAM_LOOK_HEIGHT = 100.0f;   //注視点の高さ（ボスの胴体）
			constexpr float FALL_TIME = 2.0f;     //倒れるのを見せる時間
			constexpr float CIRCLE_TIME = 1.0f;     //魔法陣が出て広がる時間
			constexpr float SINK_TIME = 2.0f;     //地面に沈む時間
			constexpr float SINK_DEPTH = 600.0f;   //地面の下へ沈む深さ
		}
	}

	//ボスの大剣関連
	namespace BossSword
	{
		constexpr float MODEL_SCALE		= 1.5f;		// モデルサイズ

		//背中に背負うオフセット
		constexpr float BACK_OFFSET_X	= 0.0f;		// 左右
		constexpr float BACK_OFFSET_Y	= 30.0f;	// 上下
		constexpr float BACK_OFFSET_Z	= 40.0f;	// 前後
		constexpr float BACK_PITCH_RAD	= 30.0f;	// X軸回転
		constexpr float BACK_YAW_RAD	= 0.0f;		// Y軸回転
		constexpr float BACK_ROLL_RAD	= 3.0f;		// Z軸回転

		//手に持つオフセット
		constexpr float HAND_OFFSET_X	= -27.0f;	// 左右
		constexpr float HAND_OFFSET_Y	= 23.0f;	// 上下
		constexpr float HAND_OFFSET_Z	=-15.0f;	// 前後
		constexpr float HAND_PITCH_RAD	= -1.660f;	// X軸回転
		constexpr float HAND_YAW_RAD	= -0.560f;	// Y軸回転
		constexpr float HAND_ROLL_RAD	= 1.500f;	// Z軸回転

		//当たり判定
		constexpr float BLADE_ROOT_Z	= 20.0f;	// 刃の根本
		constexpr float BLADE_TIP_Z		= 150.0f;	// 刃の先端
		constexpr float BLADE_RADIUS	= 100.0f;	// 刃の太さ
		constexpr float BLADE_OFFSET_X	= 0.0f;		// 当たり判定の左右
		constexpr float BLADE_OFFSET_Y	= 0.0f;		// 当たり判定の上下
		constexpr float BLADE_OFFSET_Z	= 0.0f;		// 当たり判定の前後
		constexpr float BLADE_PITCH_RAD = 0.0f;		// 当たり判定のX軸回転
		constexpr float BLADE_YAW_RAD	= 0.0f;		// 当たり判定のY軸回転
		constexpr float BLADE_ROLL_RAD	= 0.0f;		// 当たり判定のZ軸回転
	}
	
	//リザルト評価関連
	namespace Result
	{
		constexpr float RANK_A_SEC = 240.0f;		//評価Aになるクリアまでの時間
		constexpr float RANK_B_SEC = 300.0f;		//評価Bになるクリアまでの時間
		constexpr float RANK_C_SEC = 360.0f;		//評価Cになるクリアまでの時間
		constexpr float RANK_D_SEC = 420.0f;		//評価Dになるクリアまでの時間
	}
	
	//ライティング関連
	namespace Light
	{
		//平行光源の向き
		constexpr float LIGHT_DIR_X		= 0.2f;		// 左右
		constexpr float LIGHT_DIR_Y		= -1.0f;	// 上下
		constexpr float LIGHT_DIR_Z		= 0.3f;		// 前後

		//メインライトの拡散色
		constexpr float LIGHT_DIF_R		= 0.90f;	// メインライト拡散色
		constexpr float LIGHT_DIF_G		= 0.80f;	// 緑を落として青紫寄りにする
		constexpr float LIGHT_DIF_B		= 0.90f;	// 青寄りで冷たく

		//ライトの環境光
		constexpr float LIGHT_AMB_R		= 0.12f;	// ライトの環境光R
		constexpr float LIGHT_AMB_G		= 0.12f;	// ライトの環境光G
		constexpr float LIGHT_AMB_B		= 0.16f;	// ライトの環境光B
		constexpr float GLOBAL_AMB_R	= 0.60f;	// 全体環境光R
		constexpr float GLOBAL_AMB_G	= 0.60f;	// 全体環境光B
		constexpr float GLOBAL_AMB_B	= 0.64f;	// 全体環境光G
    }
	 
	//シャドウマップ関連
	namespace Shadow
	{
		constexpr int   SHADOW_MAP_SIZE		= 8192;   // 解像度（大きいほど綺麗・重い）
		constexpr float SHADOW_AREA_HALF	= 4000.0f;// 影を計算する範囲（プレイヤー中心からの半幅）
	}
	
	//刀関連
	namespace Katana
	{
		//刀の当たり判定
		constexpr float BLADE_ROOT_Z	= 13.0f;	// 刃の根元
		constexpr float BLADE_TIP_Z		= 98.0f;	// 切っ先
		constexpr float BLADE_RADIUS	= 30.0f;	// 刃の当たり判定の太さ
		constexpr float MODEL_SCALE		= 1.0f;		// 刀モデルの表示倍率

		//刀につけるカプセルのオフセット関連
		constexpr float BLADE_OFFSET_X	= 1.0f;		// 当たり判定の左右
		constexpr float BLADE_OFFSET_Y	= 20.0f;	// 当たり判定の上下
		constexpr float BLADE_OFFSET_Z	= 0.0f;		// 当たり判定の前後
		constexpr float BLADE_PITCH_RAD = -1.6f;	// X軸回転
		constexpr float BLADE_YAW_RAD	= 0.0f;		// Y軸回転
		constexpr float BLADE_ROLL_RAD	= 0.0f;		// Z軸回転

		//腰に差すオフセット関連
		constexpr float WAIST_OFFSET_X	= 15.0f;	// 左右
		constexpr float WAIST_OFFSET_Y	= 2.0f;		// 上下
		constexpr float WAIST_OFFSET_Z	= -40.0f;	// 前後
		constexpr float WAIST_PITCH_RAD = 1.5f;		// X軸回転
		constexpr float WAIST_YAW_RAD	= 0.0f;		// Y軸回転
		constexpr float WAIST_ROLL_RAD	= 0.0f;		// Z軸回転

		//手に持つオフセット関連
		constexpr float HAND_OFFSET_X	= -20.0f;	// 左右
		constexpr float HAND_OFFSET_Y	= -13.0f;	// 上下
		constexpr float HAND_OFFSET_Z	= -6.0f;	// 前後
		constexpr float HAND_PITCH_RAD	= 0.0f;		// X軸回転
		constexpr float HAND_YAW_RAD	= -0.75f;	// Y軸回転
		constexpr float HAND_ROLL_RAD	= -1.0f;	// Z軸回転

		//アニメション関連
		constexpr float DRAW_SWITCH_TIME	= 0.8f;	// 抜刀アニメ開始から刀を手へ移すまでの時間
		constexpr float SHEATHE_SWITCH_TIME = 1.0f;	// 納刀アニメ開始から刀を腰へ戻すまで
		constexpr float SHEATHE_DELAY		= 3.0f; // 戦闘終了から納刀までの秒数

		//斬撃関連
		constexpr float TRAIL_FLASH_MAX		= 1.0f; // 斬った瞬間の発光の強さ
		constexpr float TRAIL_FLASH_DECAY	= 4.0f; // 発光が戻る速さ
	}

	//鞘関連
	namespace Sheath
	{
		//鞘の腰オフセット
		constexpr float WAIST_OFFSET_X	= 13.0f;	// 左右
		constexpr float WAIST_OFFSET_Y	= 3.0f;		// 上下
		constexpr float WAIST_OFFSET_Z	= -10.0f;	// 前後
		constexpr float WAIST_PITCH_RAD = 1.48f;	// X軸回転
		constexpr float WAIST_YAW_RAD	= 0.04f;	// Y軸回転
		constexpr float WAIST_ROLL_RAD	= 0.5f;		// Z軸回転
	}

	//雑魚敵関連
	namespace Enemy
	{
		namespace HpBar
		{
			constexpr int   WIDTH				= 80;     //バーの幅
			constexpr int   HEIGHT				= 6;      //バーの高さ
			constexpr float HEIGHT_OFFSET		= 20.0f;  //頭上どれだけ上に出すか
		}

		constexpr float HP_MAX					= 150.0f;	// HP
		constexpr float RADIUS					= 60.0f;	// 半径
		constexpr float HEIGHT					= 300.0f;	// 高さ
		constexpr float ATTACK_DURATION			= 4.0f;		// 攻撃アニメ全体の長さ
		constexpr float ATTACK_HIT_START		= 1.0f;		// 腕を振り始める瞬間で判定ON
		constexpr float ATTACK_HIT_END			= 2.0f;		// 振り終わりで判定OFF
		constexpr float ATTACK_COOLDOWN			= 3.0f;		// 次の攻撃までの待ち
		constexpr float MOVE_SPEED				= 250.0f;	// 移動速度
		constexpr float TURN_SPEED				= 360.0f;   // 旋回速度
		constexpr float DETECT_RANGE			= 800.0f;	// 探知範囲
		constexpr float STOP_DISTANCE			= 170.0f;	// 停止範囲
		constexpr float ATTACK_RANGE			= 200.0f;	// 攻撃範囲
		constexpr float HAND_CAPSULE_RADIUS		= 50.0f;	// 手の攻撃球サイズ
		constexpr float DAMAGED_DURATION		= 0.6f;		// 被弾時間
		constexpr float DEAD_DURATION			= 1.5f;		// 死亡時間
		constexpr float ENEMY_ATTACK_POWER		= 500.0f;	// 攻撃力
		constexpr float PUSH_MARGIN				= 50.0f;	// 押し戻しの余白
		constexpr float DAMAGED_KNOCKBACK		= 200.0f;   // のけぞりで押される初速
		constexpr float PUSH_RESISTANCE			= 0.12f;	// ノックバック
		constexpr float DEAD_LAUNCH_SPEED		= 600.0f;   // 吹き飛びの水平速度
		constexpr float DEAD_LAUNCH_VY			= 1000.0f;  // 吹き飛びの上向き初速（放物線の高さ）
		constexpr float DEAD_ANIM_SPEED			= 2.0f;		// 死亡アニメーション再生速度
	}

	//ターゲット関連
	namespace Target
	{
		constexpr float MARKER_HEIGHT	= 100.0f;   //頭上どれだけ上に置くか
		constexpr float MARKER_SCALE	= 10.0f;	//矢印の表示倍率
		constexpr float ROTATE_SPEED	= 3.0f;		//回転速度
		constexpr float BOB_SPEED		= 6.0f;		//上下揺れの速さ
		constexpr float BOB_HEIGHT		= 20.0f;	//上下揺れの幅
	}

	//タイトルシーン(たぶん消す)
	namespace Title
	{
		constexpr int   TITLE_BLINK_FRAMES = 60;//「Press Button」点滅間隔
	}

	//ゲームオーバーシーン関連
	namespace GameOver
	{
		constexpr float GAMEOVER_FADE_TIME = 2.0f;
	}
	
	//カットシーン関連
	namespace Cutscene
	{
		constexpr float FADE_SPEED			= 8.0f;     // フェード速度
		constexpr float FADE_BOSS_SPEED		= 13.0f;	// ボスカメラに移行する際フェード時間

		//扉見上げカメラの位置と注視点
		constexpr float CAM_POS_X			= 7.0f;		// 横
		constexpr float CAM_POS_Y			= 40.0f;    // 低い位置
		constexpr float CAM_POS_Z			= -5200.0f; // 扉の手前
		constexpr float CAM_TGT_X			= 7.0f;		// 横
		constexpr float CAM_TGT_Y			= 320.0f;   // 上を見る
		constexpr float CAM_TGT_Z			= -6110.0f; // 扉の位置

		constexpr float PLAYER_WALK_YAW		= 180.0f;	// プレイヤー向く方向
		constexpr float TRIGGER_RANGE		= 700.0f;   // 扉の入力受付範囲
		
		constexpr float WALK_SPEED			= 250.0f;   // 自動歩行の速度
		constexpr float BOSS_ROOM_ENTER_Z	= -6700.0f; // ここまで進んだらボス部屋

		constexpr float PLAYER_START_X		= 7.0f;     // 扉の中央X
		constexpr float PLAYER_START_Z		= -5700.0f; // 歩き始める位置

		constexpr int LETTERBOX_HEIGHT		= 80;		// 上下に出る帯の幅

		constexpr float ROAR_SHAKE			= 0.9f;		// 咆哮時のカメラシェイクの強さ
		constexpr float ROAR_SHAKE_START	= 0.5f;		// 咆哮開始からこの秒数で揺れ始める
		constexpr float ROAR_SHAKE_END		= 4.9f;		// この秒数で揺れを止める
	}
}