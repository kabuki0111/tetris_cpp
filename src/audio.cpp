#include "audio.h"

namespace {
// 出来事ごとの SE ファイルと音量（並びは GameEvent と同じ順）
struct SfxInfo { const char* file; float gain; };
const SfxInfo SFX_INFO[EVENT_COUNT] = {
    {"se_move.wav",     0.6f},
    {"se_rotate.wav",   0.7f},
    {"se_lock.wav",     0.7f},
    {"se_clear.wav",    0.8f},
    {"se_tetris.wav",   0.9f},
    {"se_gameover.wav", 0.9f},
};

constexpr const char* BGM_FILE = "bgm.wav";
constexpr float BGM_GAIN = 0.5f;
}

// ---- Sound ----

bool Sound::load(const std::string& path, float gain) {
    if (!SDL_LoadWAV(path.c_str(), &spec, &data, &len)) {
        SDL_Log("Sound load failed (%s): %s", path.c_str(), SDL_GetError());
        return false;
    }
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!stream) return false;
    SDL_SetAudioStreamGain(stream, gain);
    SDL_ResumeAudioStreamDevice(stream);        // デバイスは停止状態で開くので再開する
    return true;
}

void Sound::play() {
    if (!stream) return;
    SDL_ClearAudioStream(stream);
    SDL_PutAudioStreamData(stream, data, (int)len);
}

void Sound::stop() {
    if (stream) SDL_ClearAudioStream(stream);
}

void Sound::keepLooping() {
    if (stream && SDL_GetAudioStreamQueued(stream) < (int)len)
        SDL_PutAudioStreamData(stream, data, (int)len);
}

void Sound::setSpeed(float ratio) {
    if (stream) SDL_SetAudioStreamFrequencyRatio(stream, ratio);
}

void Sound::release() {
    SDL_DestroyAudioStream(stream);
    SDL_free(data);
    stream = nullptr; data = nullptr;
}

// ---- Audio ----

void Audio::load(const std::string& dir) {
    bgm.load(dir + BGM_FILE, BGM_GAIN);
    for (int i = 0; i < EVENT_COUNT; i++)
        se[i].load(dir + SFX_INFO[i].file, SFX_INFO[i].gain);
}

void Audio::setBgmPlaying(bool on) {
    if (on == bgmPlaying) return;
    bgmPlaying = on;
    if (on) bgm.play();
    else    bgm.stop();
}

void Audio::setBgmSpeed(float ratio) {
    if (ratio == bgmSpeed) return;
    bgmSpeed = ratio;
    bgm.setSpeed(ratio);
}

void Audio::update(GameEvents events) {
    if (bgmPlaying) bgm.keepLooping();
    for (int i = 0; i < EVENT_COUNT; i++)
        if (events & eventBit((GameEvent)i)) se[i].play();
}

void Audio::release() {
    bgm.release();
    for (auto& s : se) s.release();
}
