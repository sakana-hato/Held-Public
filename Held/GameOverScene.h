#pragma once
#include "Scene.h"
#include "SharedContext.h"

/// <summary>
/// ゲームオーバー画面
/// </summary>
class GameOverScene final : public Scene
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="ctx"></param>共通データ
    explicit GameOverScene(SharedContext& ctx, InputSystem& input)
        : Scene(input), ctx_(ctx) {}

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GameOverScene() = default;

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
    SharedContext& ctx_; // 共通データ

    int   cursorIndex   = 0;            // 0=ゲームに戻る, 1=タイトルへ
    float lockTimer     = 0.0f;         // 入力ロック開始からの経過時間
    bool  inputLock     = true;         // 即遷移防止

    static constexpr float INPUT_LOCK_SEC = 0.5f; // 入力を受け付けない時間

    /// <summary>
   /// ゲームオーバーの見出しを描画する
   /// </summary>
    void DrawGameOver() const;

    /// <summary>
    /// 選択メニューを描画する
    /// </summary>
    void DrawMenu()     const;
};