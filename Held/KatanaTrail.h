#pragma once

/// <summary>
/// 刀の軌跡（残像）を描画するクラス
/// </summary>
class KatanaTrail
{
public:
	void Init(int maxSegments);

	void LoadShader();

	void Clear();

	void Update(float dt);

	
	void Push(const VECTOR& root, const VECTOR& tip);

	void Draw() const;

	bool Empty() const { return segments.empty(); }

	void TriggerFlash() { flashIntensity = Config::Katana::TRAIL_FLASH_MAX; }   //斬った瞬間

private:

	struct Segment
	{
		VECTOR root = VGet(0.0f, 0.0f, 0.0f);
		VECTOR tip = VGet(0.0f, 0.0f, 0.0f);
	};

	std::vector<Segment> segments;
	int maxSegments = 15;

	int vsHandle	= -1;		//頂点シェーダー
	int psHandle	= -1;		//ピクセルシェーダー
	int cbHandle	= -1;		//定数バッファ
	float time		= 0.0f;		//経過時間
	float flashIntensity = 1.0f; //現在の発光の強さ
	int trailTexture	= -1;
};
