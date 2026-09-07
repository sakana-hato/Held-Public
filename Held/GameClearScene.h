#pragma once
#include "Scene.h"
#include "SharedContext.h"

/// <summary>
/// ゲームクリア画面
/// </summary>
class GameClearScene final : public Scene
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="ctx"></param>共通データ
    explicit GameClearScene(SharedContext& ctx, InputSystem& input)
        : Scene(input), ctx_(ctx) {}

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~GameClearScene() = default;

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
    /// <param name="dt"></param>
    void Update(float dt) override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw()    override;

private:
    SharedContext& ctx_;//共通データ

    char  rank          = 'E';      // 評価ランク（AからE）
    int   cursorIndex   = 0;        // 0=もう一度, 1=タイトルへ
    bool  inputLock     = true;     // 即遷移防止
    float lockTimer     = 0.0f;     // 入力ロック開始からの経過時間
    static constexpr float INPUT_LOCK_SEC = 0.5f; // 入力を受け付けない時間

    /// <summary>
    /// クリアタイムから評価ランクを求める
    /// </summary>
    char CalcRank() const;

    /// <summary>
    /// クリア表示を描画する
    /// </summary>
    void DrawClear() const;

    /// <summary>
    /// 選択メニューを描画する
    /// </summary>
    void DrawMenu()  const;
};
