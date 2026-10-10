#include "tetris.h"
#include <algorithm>
#include <iterator>

Tetris::Tetris() { reset(); }

void Tetris::reset() {
    for (auto& row : board_) std::fill(std::begin(row), std::end(row), 0);
    score_ = lines_ = 0;
    gameOver_ = false;
    lastFall_ = nowMs_;                 // 再スタート直後に1段落ちないように
    bag_.clear();
    next_ = takeFromBag();
    spawn();
}

void Tetris::update(Uint64 now) {
    nowMs_ = now;
    if (gameOver_) return;
    // 符号なしの引き算でマイナスに回り込まないよう、先に最短値で止める
    Uint64 step = level() * FALL_MS_STEP;
    Uint64 interval = (step >= FALL_MS_START - FALL_MS_MIN) ? FALL_MS_MIN : FALL_MS_START - step;
    if (now - lastFall_ >= interval) {
        lastFall_ = now;
        if (!tryMove(0, 1)) lockPiece();
    }
}

void Tetris::onKey(SDL_Keycode key) {
    if (gameOver_) return;
    if (key == SDLK_R) { reset(); return; }
    switch (key) {
    case SDLK_LEFT:  if (tryMove(-1, 0)) emit(EVENT_MOVE); break;
    case SDLK_RIGHT: if (tryMove(1, 0))  emit(EVENT_MOVE); break;
    case SDLK_DOWN:  if (tryMove(0, 1)) score_ += SOFT_DROP_SCORE; break;
    case SDLK_UP:
    case SDLK_X:     rotate(true);  break;      // 時計回り
    case SDLK_Z:     rotate(false); break;      // 反時計回り
    case SDLK_SPACE: hardDrop(); break;
    default: break;
    }
}

GameEvents Tetris::takeEvents() {
    GameEvents e = events_;
    events_ = 0;
    return e;
}

int Tetris::stackHeight() const {
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            if (board_[y][x]) return H - y;     // 上から探して最初に見つかった段
    return 0;
}

Piece Tetris::ghost() const {
    Piece p = cur_;
    while (canPlace(p)) p.y++;
    p.y--;
    return p;
}

// ---- ミノの生成 ----

// 7種を1巡ずつシャッフルして出す（7-bag）
int Tetris::takeFromBag() {
    if (bag_.empty()) {
        bag_ = {0, 1, 2, 3, 4, 5, 6};
        std::shuffle(bag_.begin(), bag_.end(), rng_);
    }
    int t = bag_.back();
    bag_.pop_back();
    return t;
}

void Tetris::spawn() {
    cur_ = {SHAPES[next_], SPAWN_X, 0, next_};
    next_ = takeFromBag();
    if (!canPlace(cur_)) {
        gameOver_ = true;
        emit(EVENT_GAMEOVER);
    }
}

// ---- 移動・回転 ----

bool Tetris::canPlace(const Piece& p) const {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++) {
            if (!p.shape[r][c]) continue;
            int bx = p.x + c, by = p.y + r;
            if (bx < 0 || bx >= W || by >= H) return false;
            if (by >= 0 && board_[by][bx]) return false;
        }
    return true;
}

bool Tetris::tryMove(int dx, int dy) {
    Piece t = cur_;
    t.x += dx; t.y += dy;
    if (!canPlace(t)) return false;
    cur_ = t;
    return true;
}

void Tetris::rotate(bool clockwise) {
    if (cur_.type == PIECE_O) return;           // Oミノは回転しない
    Piece t = cur_;
    int n = (cur_.type == PIECE_I) ? 4 : 3;     // Iは4x4、それ以外は3x3で回す
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++) {
            if (clockwise) t.shape[c][n - 1 - r] = cur_.shape[r][c];
            else           t.shape[n - 1 - c][r] = cur_.shape[r][c];
        }
    // 簡易壁蹴り：そのまま→左右1→左右2の順に試す
    for (int kick : {0, -1, 1, -2, 2}) {
        Piece k = t;
        k.x += kick;
        if (canPlace(k)) { cur_ = k; emit(EVENT_ROTATE); return; }
    }
}

void Tetris::hardDrop() {
    while (tryMove(0, 1)) score_ += HARD_DROP_SCORE;
    lockPiece();
}

// ---- 固定・ライン消去 ----

void Tetris::lockPiece() {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            if (cur_.shape[r][c] && cur_.y + r >= 0)
                board_[cur_.y + r][cur_.x + c] = cur_.type + 1;
    int cleared = clearLines();
    score_ += LINE_SCORE[cleared] * (level() + 1);
    lines_ += cleared;
    if (cleared == 4)  emit(EVENT_TETRIS);
    else if (cleared)  emit(EVENT_CLEAR);
    else               emit(EVENT_LOCK);
    spawn();
}

int Tetris::clearLines() {
    int cleared = 0;
    for (int r = H - 1; r >= 0; r--) {
        bool full = true;
        for (int c = 0; c < W; c++) if (!board_[r][c]) { full = false; break; }
        if (!full) continue;
        for (int y = r; y > 0; y--)
            for (int c = 0; c < W; c++) board_[y][c] = board_[y - 1][c];
        for (int c = 0; c < W; c++) board_[0][c] = 0;
        cleared++;
        r++;                                    // 詰めた行をもう一度チェック
    }
    return cleared;
}
