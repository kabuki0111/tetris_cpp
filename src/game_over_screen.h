// ゲームオーバー画面の状態（演出の進み具合とメニューの選択）
// 演出：盤面のブロックを上の段から順にグレーにし、終わったら暗転と GAME OVER のフェードイン、
//       最後に項目を表示して操作を受け付ける
#pragma once
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_stdinc.h>

// ゲームオーバー画面の項目（上から順に表示）
enum MenuItem { MENU_RESTART, MENU_QUIT, MENU_COUNT };
inline constexpr const char* MENU_LABELS[MENU_COUNT] = {"RESTART", "QUIT"};

class GameOverScreen {
public:
    enum class Action { None, Restart, Quit };

    // ゲームオーバーになった瞬間に呼ぶ（演出を最初から始め、選択を RESTART に戻す）
    void start(Uint64 now);
    void update(Uint64 now) { nowMs_ = now; }

    // 決定された項目を返す（演出中や、選択を動かしただけなら None）
    Action onKey(SDL_Keycode key);

    int grayRows() const;           // グレーになった段数（上から数える。0 〜 H）
    float fade() const;             // 暗転の進み具合：グレー化が終わるまで 0.0、そこから 1.0 まで
    bool menuVisible() const { return fade() >= 1.0f; }
    int cursor() const { return cursor_; }

private:
    Uint64 startAt_ = 0;
    Uint64 nowMs_ = 0;
    int cursor_ = MENU_RESTART;
};
