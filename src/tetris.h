// テトリスのルール（盤面・ミノの操作・ライン消去・スコア・ゲームオーバー判定）
// 描画や音は扱わない
#pragma once
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_stdinc.h>
#include <random>
#include <vector>
#include "config.h"
#include "game_event.h"
#include "pieces.h"

class Tetris {
public:
    Tetris();

    void reset();
    void update(Uint64 now);

    // プレイ中の操作（ゲームオーバー中は何もしない）
    void onKey(SDL_Keycode key);

    // 前回呼ばれてから起きた出来事を返してクリアする
    GameEvents takeEvents();

    // ---- 状態の読み取り（描画などに使う） ----
    int cell(int x, int y) const { return board_[y][x]; }  // 0 は空、それ以外はミノの種類 + 1
    const Piece& current() const { return cur_; }
    Piece ghost() const;                                    // 今のミノをそのまま落としたときの着地位置
    int nextType() const { return next_; }
    int score() const { return score_; }
    int lines() const { return lines_; }
    int level() const { return lines_ / LINES_PER_LEVEL; }
    int stackHeight() const;                                // 固定されたブロックの高さ（段数。空なら 0）
    bool isGameOver() const { return gameOver_; }

private:
    int board_[H][W]{};
    Piece cur_{};
    int next_ = 0;
    std::vector<int> bag_;
    std::mt19937 rng_{std::random_device{}()};
    int score_ = 0, lines_ = 0;
    bool gameOver_ = false;
    Uint64 lastFall_ = 0;
    Uint64 nowMs_ = 0;                  // 最後に update() で受け取った時刻
    GameEvents events_ = 0;             // takeEvents() までに起きた出来事

    void emit(GameEvent e) { events_ |= eventBit(e); }

    // ミノの生成
    int takeFromBag();
    void spawn();

    // 移動・回転
    bool canPlace(const Piece& p) const;
    bool tryMove(int dx, int dy);
    void rotate(bool clockwise);
    void hardDrop();

    // 固定・ライン消去
    void lockPiece();
    int clearLines();
};
