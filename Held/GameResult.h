#pragma once

struct GameResult
{
	float clearTimeSec	= 0.0f;
	bool  isGameClear	= false;

	bool  titleIntroPlayed = false;   //タイトルの導入演出を再生済みか

	//リソースの読み込み状態
	bool  titleResourceLoaded	= false;   //タイトル用を読み込み済みか
	bool  gameResourceLoaded	= false;   //ゲーム用を読み込み済みか
};