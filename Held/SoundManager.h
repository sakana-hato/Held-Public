#pragma once
#include "SoundId.h"
#include "Precompiled.h"

/// <summary>
/// BGMやSEの再生制御
/// </summary>
class SoundManager
{
public:
    //シングルトン
    static SoundManager& Instance()
    {
        static SoundManager inst;
        return inst;
    }

    //シングルトンなのでコピーは禁止
	SoundManager(const SoundManager&)            = delete;
	SoundManager& operator=(const SoundManager&) = delete;

    /// <summary>
    /// BGMをループ再生する
    /// </summary>
    /// <param name="id"></param>再生するBGM
    void PlayBgm(BgmId id);

    /// <summary>
    /// SEをループ再生する
    /// </summary>
    /// <param name="id">           </param>再生するSE
    /// <param name="volumeScale">  </param>音量倍率
    void PlaySeLoop(SeId id, float volumeScale = 1.0f);

    /// <summary>
    /// ループ再生中のSEを止める
    /// </summary>
    /// <param name="id"></param>止めるSE
    void StopSeLoop(SeId id);

    /// <summary>
    /// そのSEがループ再生中か
    /// </summary>
    bool IsSeLoopPlaying(SeId id) const;

    /// <summary>
    /// 再生中のBGMを停止する
    /// </summary>
    void StopBgm();

    /// <summary>
    /// 再生中のBGMを徐々に消す
    /// </summary>
    void FadeOutBgm();

    /// <summary>
    /// BGMを切り替える
    /// </summary>
    /// <param name="id"></param>切り替え先のBGM
    void ChangeBgm(BgmId id, float volumeScale = 1.0f);

    /// <summary>
    /// SEを再生する
    /// </summary>
    /// <param name="id"></param>再生するSE
    void PlaySe(SeId id);

    /// <summary>
    /// 3Dサウンドを指定位置で再生する
    /// </summary>
    /// <param name="handle">再生するハンドル（Enemyが複製して持つもの）</param>
    /// <param name="pos">音源のワールド座標</param>
    /// <param name="radius">聞こえる範囲</param>
    void PlaySe3D(int handle, const VECTOR& pos, float radius);

    /// <summary>
    /// 3Dサウンドのリスナー（聞く側）の位置と向きを設定する
    /// </summary>
    /// <param name="listenerPos">リスナーの位置（カメラ）</param>
    /// <param name="frontPos">リスナーが向いている先の位置</param>
    void Set3DListener(const VECTOR& listenerPos, const VECTOR& frontPos);

    /// <summary>
    /// 連番のSEからランダムで1つ選んで再生する（ボイスなど）
    /// </summary>
    /// <param name="first"></param>最初のSeId
    /// <param name="count"></param>候補の数
    void PlaySeRandom(SeId first, int count);

    /// <summary>
    /// BGMの音量を一時的に下げる
    /// </summary>
    /// <param name="scale"></param>音量倍率
    void DuckBgm(float scale);

    /// <summary>
    /// BGMの音量を通常に戻す
    /// </summary>
    void UnduckBgm();

    /// <summary>
    /// BGMの音量を設定する
    /// </summary>
    /// <param name="vol"></param>音量
    void SetBgmVolume(float vol);

    /// <summary>
    /// SEの音量を設定する
    /// </summary>
    /// <param name="vol"></param>音量
    void SetSeVolume(float vol);

    /// <summary>
    /// BGMの音量を取得する
    /// </summary>
    /// <returns></returns>音量
    float GetBgmVolume()const { return bgmVolume; }

    /// <summary>
    /// SEの音量を取得する
    /// </summary>
    /// <returns></returns>音量
    float GetSeVolume() const { return seVolume; }

    /// <summary>
    /// 再生状態をリセットする
    /// </summary>
    void Reset();

    /// <summary>
    /// 更新
    /// </summary>
    void Update(float dt);
private:
    SoundManager() = default;

    /// <summary>
    /// BgmIdをResourceManagerの登録キーに変換する
    /// </summary>
    /// <param name="id"></param>BGMの識別子
    const char* BgmKey(BgmId id)    const;

    /// <summary>
    /// SeIdをResourceManagerの登録キーに変換する
    /// </summary>
    /// <param name="id"></param>SEの識別子
    /// <returns></returns>登録キー
    const char* SeKey(SeId id)      const;

    /// <summary>
    /// DxLib 音量へ変換
    /// </summary>
    /// <param name="vol"></param>音量
    int ToDxVolume(float vol)       const;

    /// <summary>
    /// 現在のSE音量を全SEハンドルへ反映する
    /// </summary>
    void ApplySeVolume()            const;

    int    currentBgmHandle = -1;           //再生中のBGMハンドル
    BgmId  currentBgmId     = BgmId::Count; //再生中のBGM

    float  bgmVolume        = 0.8f;         //BGM音量
    float  seVolume         = 0.9f;         //SE音量

    int   prevBgmHandle     = -1;      //フェードアウト中の前のBGM
    float fadeTimer         = 0.0f;    //フェードの経過時間
    bool  fading            = false;   //フェード中か

    float bgmDuckScale = 1.0f;
    float bgmVolumeScale = 1.0f;   //曲ごとの音量倍率
};