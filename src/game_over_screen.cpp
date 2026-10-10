#include "game_over_screen.h"
#include <algorithm>
#include "config.h"

namespace {
constexpr Uint64 GRAY_MS = 1000;        // 全段がグレーになるまでの時間
constexpr Uint64 FADE_MS = 1000;        // 暗転と文字のフェードインにかける時間
}

void GameOverScreen::start(Uint64 now) {
    startAt_ = nowMs_ = now;
    cursor_ = MENU_RESTART;
}

GameOverScreen::Action GameOverScreen::onKey(SDL_Keycode key) {
    // 演出中は受け付けない（ハードドロップの押しっぱなしで誤って決定しないように）
    if (!menuVisible()) return Action::None;
    switch (key) {
    case SDLK_UP:   cursor_ = (cursor_ + MENU_COUNT - 1) % MENU_COUNT; break;
    case SDLK_DOWN: cursor_ = (cursor_ + 1) % MENU_COUNT; break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
    case SDLK_SPACE:
        return cursor_ == MENU_RESTART ? Action::Restart : Action::Quit;
    default: break;
    }
    return Action::None;
}

int GameOverScreen::grayRows() const {
    Uint64 elapsed = nowMs_ - startAt_;
    if (elapsed >= GRAY_MS) return H;
    return (int)(elapsed * H / GRAY_MS);
}

float GameOverScreen::fade() const {
    Uint64 elapsed = nowMs_ - startAt_;
    if (elapsed < GRAY_MS) return 0.0f;
    return std::min(1.0f, (elapsed - GRAY_MS) / (float)FADE_MS);
}
