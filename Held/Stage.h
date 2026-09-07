#pragma once

/// <summary>
/// ステージ
/// 当たり判定は後から作ったマス
/// 正直カメラの壁判定はcppとhに分けるべきだった
/// </summary>
class Stage
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Stage() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Stage();

	/// <summary>
	/// 表示用モデルをセットする
	/// </summary>
	/// <param name="handle"></param>モデルハンドル
	void SetViewModel(int handle);

	/// <summary>
	/// 当たり判定用モデルをセットする
	/// </summary>
	/// <param name="handle"></param>モデルハンドル
	void SetCollisionModel(int handle);

	/// <summary>
	/// 表示用モデルのサイズ
	/// </summary>
	/// <param name="sca"></param>サイズ
	void SetScale(float sca) { scale = sca; }

	/// <summary>
	/// 当たり判定用モデルのサイズ
	/// </summary>
	/// <param name="sca"></param>サイズ
	void SetCollisionScale(float sca) { collScale = sca; }

	/// <summary>
	/// ステージのポジション
	/// </summary>
	/// <param name="po"></param>座標
	void SetPosition(const VECTOR& po) { pos = po; }

	/// <summary>
	/// モデルのデタッチ
	/// </summary>
	void End();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>デルタタイム
	void Update(float dt);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()const;

	/// <summary>
	/// デバック用当たり判定描画
	/// </summary>
	void DrawCollisionDebug() const;

	/// <summary>
	/// ドアの開閉
	/// </summary>
	void PlayDoor();

	/// <summary>
	/// 指定座標の真下にある床の高さを返す。
	/// </summary>
	/// <param name="pos"></param>調べたい位置
	/// <param name="outY"></param>床が見つかったらその高さが入る
	bool GetFloorY(const VECTOR& pos, float& outY)const;

	// <summary>
	/// 壁の当たり判定
	/// </summary>
	/// <param name="footPos"></param>移動後の足元座標
	/// <param name="radius"> </param>カプセル半径
	/// <param name="height"> </param>カプセル全高
	VECTOR ResolveWall(const VECTOR& footPos, float radius, float height);

	/// <summary>
	/// 注視点からカメラへの線が壁で遮られていたら、手前まで引き寄せた位置を返す
	/// </summary>
	/// <param name="from">		</param>線の始点（注視点）
	/// <param name="to">		</param>線の終点（カメラ位置）
	/// <param name="margin">	</param>壁からどれだけ離すか
	VECTOR ClampCameraByWall(const VECTOR& from, const VECTOR& to, float margin) const;

	/// <summary>
	/// カメラを球とみなして壁から押し出す(未使用)
	/// </summary>
	/// <param name="camPos"></param>カメラ位置
	/// <param name="radis"> </param>球の半径
	VECTOR ResolveCameraSphere(const VECTOR& camPos, float radis)const;

	/// <summary>
	/// 表示用モデルのハンドルを取得する
	/// </summary>
	/// <returns></returns>モデルハンドル
	int GetViewModel() const { return viewModel; }

	/// <summary>
	/// 当たり判定用モデルの向きを設定する
	/// </summary>
	/// <param name="deg"></param>Y軸まわりの角度（度）
	void SetCollisionYawDeg(float deg) { collYawRad = deg * DX_PI_F / 180.0f; }

	/// <summary>
	/// 扉の見えない壁による押し戻し。閉じている間だけ効く
	/// </summary>
	/// <param name="pos">   </param>判定する座標
	/// <param name="radius"></param>半径
	VECTOR ResolveDoorWall(const VECTOR& pos, float radius) const;

	/// <summary>
	/// 扉の見えない壁を解除して通れるようにする
	/// </summary>
	void   OpenDoorWall() { doorWallActive = false; }  

	/// <summary>
	/// 扉の見えない壁をデバッグ表示する
	/// </summary>
	void DrawDoorWallDebug() const;

	/// <summary>
	/// 扉の見えない壁が有効かどうか
	/// </summary>
	bool IsDoorWallActive() const { return doorWallActive; }

	/// <summary>
	/// 扉を閉じる
	/// </summary>
	void CloseDoor();                          

	/// <summary>
	/// 扉の当たり判定を戻す
	/// </summary>
	void CloseDoorWall() { doorWallActive = true; }   

	/// <summary>
	///  扉アニメが完了したか
	/// </summary>
	bool IsDoorFinished() const { return !doorPlaying; }

	/// <summary>
	/// 扉アニメの現在時間を取得する
	/// </summary>
	float GetDoorTime() const { return doorTime; }

	/// <summary>
	/// 扉アニメが再生中か
	/// </summary>
	bool  IsDoorPlaying() const { return doorPlaying; }

	/// <summary>
	/// 扉アニメが逆再生中か
	/// </summary>
	bool  IsDoorReverse() const { return doorReverse; }
private:
	/// <summary>
	/// 扉アニメを表示用モデルにアタッチする
	/// </summary>
	void AttachDoorAnims();

	/// <summary>
	/// コリジョン情報を構築する
	/// </summary>
	void SetupCollision();   

	/// <summary>
	/// 位置・スケール・回転をモデルへ反映する
	/// </summary>
	/// <param name="model"></param>対象のモデルハンドル
	void ApplyTransformTo(int model) const;

	int  viewModel		= -1;					//見た目
	int  collModel		= -1;					//当たり判定
	bool collReady		= false;				//コリジョン情報を構築済みか

	VECTOR pos			= { 0.0f, 0.0f, 0.0f };	//ステージの配置座標
	float  scale		= 1.0f;					//表示用モデルの倍率
	float  collScale	= 1.0f;					//判定用モデルの倍率
	float  collYawRad	= 0.0f;					//判定用モデルのY回転（ラジアン）

	//床判定
	static constexpr float FLOOR_RAY_UP		= 200.0f;		//調べる点の上方向の開始オフセット
	static constexpr float FLOOR_RAY_DOWN	= 5000.0f;		//そこから下へ伸ばす長さ

	//壁判定用
	static constexpr float WALL_FOOT_LIFT		= 5.0f;		//床すれすれを壁と誤検出しないための浮かせ量
	static constexpr float WALL_NORMAL_Y_MAX	= 0.5f;		//法線Yがこれより大きい面は床/天井扱いで無視
	static constexpr float WALL_PUSH_STEP		= 20.0f;		//1反復あたりの押し戻し量
	static constexpr float WALL_SKIN			= 1.0f;		//押し戻し後に残す余白
	static constexpr int   WALL_RESOLVE_ITER	= 10;		//押し戻しの反復回数
	static constexpr int   CAM_RESOLVE_ITER		= 4;		//カメラ押し戻しの反復回数

	//扉アニメ
	int   doorAttachA	= -1;		//扉Aのアタッチ番号
	int   doorAttachB	= -1;		//扉Bのアタッチ番号
	float doorTotalA	= 0.0f;		//扉Aの総再生時間
	float doorTotalB	= 0.0f;		//扉Bの総再生時間
	float doorTime		= 0.0f;		//現在の再生位置
	bool  doorPlaying	= false;	//再生中か
	bool doorReverse	= false;	//逆再生フラグ

	//扉の見えない壁
	bool  doorWallActive	= true;			//扉の壁が有効か
	float doorWallZ			= -6000.0f;		//扉のZ位置
	float doorWallHalfX		= 600.0f;		//扉の左右の半幅
	float doorWallCenterX	= 0.0f;			//扉の中心X
};
