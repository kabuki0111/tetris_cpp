// サウンド（BGM と効果音）
#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "game_event.h"

// WAV を1本のストリームで鳴らす（BGM・SE 共通）
// 読み込みに失敗しても、play などは何もしないだけでゲームは続行できる
struct Sound {
    SDL_AudioSpec spec{};
    Uint8* data = nullptr;
    Uint32 len = 0;
    SDL_AudioStream* stream = nullptr;

    bool load(const std::string& path, float gain = 1.0f);
    void play();            // 頭から1回鳴らす（鳴っている途中なら止めて鳴らし直す）
    void stop();            // 鳴っている音を止める
    void keepLooping();     // ループ再生用：残りが1曲分を切ったら次の1曲分を継ぎ足す（毎フレーム呼ぶ）
    void setSpeed(float ratio);     // 再生速度（1.0 で通常。速度と一緒に音程も変わる）
    void release();
};

// BGM と SE をまとめて管理する（どの出来事でどの音を鳴らすかは audio.cpp の表で決める）
class Audio {
public:
    // dir: WAV ファイルのあるフォルダ（末尾に区切り文字付き）
    // BGM は読み込むだけで、鳴らし始めるのは setBgmPlaying(true) から
    void load(const std::string& dir);

    // true で BGM を頭から再生、false で停止（同じ値が続く間は何もしない）
    void setBgmPlaying(bool on);

    // BGM の再生速度（1.0 で通常。同じ値が続く間は何もしない）
    void setBgmSpeed(float ratio);

    // 毎フレーム呼ぶ。events は Tetris::takeEvents() の戻り値
    void update(GameEvents events);

    void release();

private:
    Sound bgm;
    bool bgmPlaying = false;
    float bgmSpeed = 1.0f;
    Sound se[EVENT_COUNT];
};
