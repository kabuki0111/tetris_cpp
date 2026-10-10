// ゲーム全体で使う定数
#pragma once
#include <SDL3/SDL_stdinc.h>

// 盤面と画面のサイズ
inline constexpr int W = 10, H = 20, CELL = 30;    // 盤面の列数・段数、1マスのピクセル数
inline constexpr int SIDE = 160;                    // 右側の情報欄の幅
inline constexpr int WIN_W = W * CELL + SIDE, WIN_H = H * CELL;

inline constexpr int SPAWN_X = 3;                   // ミノの出現位置（左端の列）

// 落下間隔（ミリ秒）：開始時の間隔からレベルごとに短くなり、最短値で止まる
inline constexpr Uint64 FALL_MS_START = 800, FALL_MS_STEP = 70, FALL_MS_MIN = 100;
inline constexpr int LINES_PER_LEVEL = 10;

// 同時に消したライン数ごとの得点（レベル + 1 倍される）
inline constexpr int LINE_SCORE[5] = {0, 100, 300, 500, 800};
inline constexpr int SOFT_DROP_SCORE = 1, HARD_DROP_SCORE = 2;     // 1マス落とすごとの得点

// ピンチ演出：積まれたブロックがこの段数以上になったら BGM を速くする（速度と一緒に音程も上がる）
inline constexpr int DANGER_HEIGHT = 14;
inline constexpr float BGM_DANGER_SPEED = 1.25f;    // 通常を 1.0 とした再生速度
