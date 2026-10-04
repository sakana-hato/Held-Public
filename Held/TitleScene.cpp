#include "DxLib.h"
#include "TitleScene.h"
#include "Config.h"
#include "Fader.h"
#include "ResourceManager.h"


void TitleScene::OnEnter()
{
	if (!result.titleResourceLoaded)
	{
		ResourceManager::Instance().LoadJson("Data/json/TitleResources.json");
		result.titleResourceLoaded = true;
	}

	stage.SetScale(170.0f);
	stage.SetCollisionScale(170.0f);
	stage.SetCollisionYawDeg(180.0f);
	stage.SetViewModel(ResourceManager::Instance().Model("stage"));
	stage.SetCollisionModel(ResourceManager::Instance().Model("stage_collision"));
	camera.Init();
	camera.SetStage(&stage);

	player = std::make_unique<Player>(camera, input, stage, target, difficulty);
	player->SetModel(ResourceManager::Instance().Model("player_base"), 1.7f);
	player->SetKatana(ResourceManager::Instance().Model("katana"));
	player->SetSheath(ResourceManager::Instance().Model("sheath"));

	titleDirector = std::make_unique<TitleDirector>(camera, *player);

	post.Init(Config::Window::WINDOW_W, Config::Window::WINDOW_H);
              
	Fader::GetInstance().FadeIn(Config::Title::FADE_SPEED);

	const std::string fontName	= ResourceManager::Instance().FontName(Config::Title::FONT_ID);
	const char* namePtr			= fontName.empty() ? nullptr : fontName.c_str();
	fontHandle					= CreateFontToHandle(namePtr, Config::Title::FONT_SIZE, -1, DX_FONTTYPE_ANTIALIASING_EDGE);

	
	if (!result.titleIntroPlayed)
	{
		titleDirector->Start();
		result.titleIntroPlayed = true;
	}
	else
	{
		titleDirector->SkipToEnd();
	}

	blinkTimer = 0;
	fadingOut = false;
	logoTimer = 0.0f;
	prevPressed = true;
}

void TitleScene::OnExit()
{
	post.End();

	if (fontHandle >= 0)
	{
		DeleteFontToHandle(fontHandle);
		fontHandle = -1;
	}
}

void TitleScene::Update(float dt)
{
	post.Update(dt);
	blinkTimer += dt;

	Fader::GetInstance().Update();

	//演出の更新
	if (titleDirector)
	{
		titleDirector->Update(dt);
	}

	//プレイヤーとステージの更新（アニメを進めるため）
	if (player)
	{
		player->UpdateKatanaSwitch(dt);
		player->UpdateAnimOnly(dt);
	}
	stage.Update(dt);

	//カメラを反映する
	camera.Apply();

	//フェードアウト中はシーン遷移を待つ
	if (fadingOut)
	{
		if (Fader::GetInstance().IsFinishFadeOut())
		{
			RequestChange(SceneId::Load);
		}
		return;
	}

	if (titleDirector && titleDirector->IsLogoVisible())
	{
		logoTimer += dt;
	}

	//今のフレームで押されているか
	const bool nowPressed = (CheckHitKey(KEY_INPUT_SPACE) != 0)|| ((GetJoypadInputState(DX_INPUT_PAD1) & PAD_INPUT_B) != 0);

	//「押した瞬間」だけを拾う（前は押していなくて、今押している）
	const bool justPressed = (nowPressed && !prevPressed);

	prevPressed = nowPressed;   //次のフレームのために覚えておく

	if (justPressed)
	{
		if (titleDirector && titleDirector->IsLogoVisible())
		{
			//ロゴが出ている状態なら、ゲームへ進む
			Fader::GetInstance().FadeOut(Config::Title::FADE_SPEED);
			fadingOut = true;
		}
		else if (titleDirector)
		{
			//演出中なら、スキップして扉前の状態にする
			titleDirector->SkipToEnd();
		}
	}
}

void TitleScene::Draw()
{
	post.BeginScene();


	//3D描画（ステージとプレイヤー）
	camera.Apply();

	SetUseLighting(FALSE);
	stage.Draw();
	if (player)
	{
		player->Draw();
	}
	SetUseLighting(TRUE);

	post.Composite();


	//ロゴが出る状態になったら、タイトル文字を描く
	if (titleDirector && titleDirector->IsLogoVisible())
	{
		//フェードインの進み具合
		const float t = logoTimer / Config::Title::LOGO_FADE_TIME;
		const float clamped = (t > 1.0f) ? 1.0f : t;
		const int alpha = static_cast<int>(255.0f * clamped);

		//タイトルロゴ
		const int logo = ResourceManager::Instance().Image("ui_title_logo");
		if (logo >= 0 && alpha > 0)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawRotaGraph3(Config::Title::LOGO_POS_X,Config::Title::LOGO_POS_Y,Config::Title::LOGO_CENTER_X,Config::Title::LOGO_CENTER_Y,
				Config::Title::LOGO_SCALE_X,Config::Title::LOGO_SCALE_Y,Config::Title::LOGO_ANGLE,logo,TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		//「Press SPACE」は、ロゴが出きってから点滅させる
		if (clamped >= 1.0f && fontHandle >= 0)
		{
			
			const float wave = (std::sin(blinkTimer * Config::Title::BLINK_SPEED) + 1.0f) * 0.5f;

			//明るさの範囲（最低でも BLINK_MIN の明るさを保つ）
			const float bright = Config::Title::BLINK_MIN+ (1.0f - Config::Title::BLINK_MIN) * wave;

			const int c = static_cast<int>(255.0f * bright);

			//コントローラの接続で文字を変える
			const bool usePad = input.IsPadConnected();
			const char* text = usePad ? Config::Title::TEXT_PAD : Config::Title::TEXT_KEY;

			//黒で縁取り（上下左右にずらして描く）
			const int ox = Config::Title::TEXT_POS_X;
			const int oy = Config::Title::TEXT_POS_Y;
			const unsigned int edge = GetColor(0, 0, 0);

			DrawStringToHandle(ox - 2, oy, text, edge, fontHandle);
			DrawStringToHandle(ox + 2, oy, text, edge, fontHandle);
			DrawStringToHandle(ox, oy - 2, text, edge, fontHandle);
			DrawStringToHandle(ox, oy + 2, text, edge, fontHandle);
			DrawStringToHandle(ox - 2, oy - 2, text, edge, fontHandle);
			DrawStringToHandle(ox + 2, oy - 2, text, edge, fontHandle);
			DrawStringToHandle(ox - 2, oy + 2, text, edge, fontHandle);
			DrawStringToHandle(ox + 2, oy + 2, text, edge, fontHandle);

			//本体（白、明滅する）
			DrawStringToHandle(ox, oy, text, GetColor(c, c, c), fontHandle);
		}
	}

	//フェードの黒幕
	Fader::GetInstance().Draw();
}