#pragma once
#include "UiBase.h"
#include "PlayerEventObserver.h"

class InputSystem;

/// <summary>
/// ジャスト回避成功時に表示するUi用のクラス
/// </summary>
class CounterUi :public UiBase, public PlayerEventObserver
{
public:
	~CounterUi();

	void Init(int buttonImage, int keyImage,const InputSystem& input);

	void OnPlayerEvent(PlayerEvent eve)override;

	void Update(float dt)override;

	void Draw()const override;

private:
	int _buttonImage			= -1;		//コントローラのボタン画像
	int _keyImage				= -1;		//キーボードのキー画像
	int _fontHandle				= -1;		//テキスト用フォント

	const InputSystem* _input	= nullptr;  //パッド入力
	bool  usePad				= false;	//パッド接続中フラグ
	float glowTimer				= 0.0f;		//発光の経過時間
};