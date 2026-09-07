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
    /// 再生中のBGMを停止する
    /// </summary>
    void StopBgm();

    /// <summary>
    /// BGMを切り替える
    /// </summary>
    /// <param name="id"></param>切り替え先のBGM
    void ChangeBgm(BgmId id);

    /// <summary>
    /// SEを再生する
    /// </summary>
    /// <param name="id"></param>再生するSE
    void PlaySe(SeId id);

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
};