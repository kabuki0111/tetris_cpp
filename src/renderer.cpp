#include "renderer.h"
#include <string>
#include "config.h"
#include "game_over_screen.h"
#include "tetris.h"

namespace {
const SDL_Color BACKGROUND = {10, 10, 15, 255};

// 盤面の値ごとの色（0 は空、1〜7 はミノの種類 + 1）
const SDL_Color COLORS[PIECE_TYPES + 1] = {
    {30, 30, 40, 255},                              // 0: 空
    {0, 220, 220, 255},
    {230, 210, 0, 255},
    {170, 0, 220, 255},
    {0, 210, 0, 255},
    {230, 0, 0, 255},
    {0, 90, 230, 255},
    {240, 140, 0, 255},
};

constexpr Uint8 GHOST_ALPHA = 70;                   // ゴースト（落下予定位置）の濃さ

// ゲームオーバー演出
const SDL_Color GAMEOVER_GRAY = {110, 110, 110, 255};
constexpr Uint8 GAMEOVER_DIM_ALPHA = 180;           // 暗転の濃さ（0〜255）

// 右側の情報欄に出す操作説明（キー, 操作）
struct ControlHelp { const char* key; const char* action; };
const ControlHelp CONTROLS[] = {
    {"<- ->",  "MOVE"},
    {"DOWN",   "SOFT DROP"},
    {"SPACE",  "HARD DROP"},
    {"UP / X", "ROTATE R"},
    {"Z",      "ROTATE L"},
    {"R",      "RESTART"},
    {"ESC",    "QUIT"},
};

SDL_Color pieceColor(int type) { return COLORS[type + 1]; }
}

void Renderer::drawFrame(const Tetris& game, const GameOverScreen& gameOver) {
    setColor(BACKGROUND);
    SDL_RenderClear(r_);
    drawField(game, gameOver);
    drawSidePanel(game);
    if (game.isGameOver()) drawGameOver(gameOver);
    SDL_RenderPresent(r_);
}

// 盤面・ゴースト・操作中のミノ
void Renderer::drawField(const Tetris& game, const GameOverScreen& gameOver) {
    int gray = game.isGameOver() ? gameOver.grayRows() : 0;    // ゲームオーバー中は上から gray 段をグレーで描く
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            int v = game.cell(x, y);
            bool toGray = v && y < gray;
            drawCell(x, y, toGray ? GAMEOVER_GRAY : COLORS[v]);
        }

    if (game.isGameOver()) return;
    SDL_Color color = pieceColor(game.current().type);
    SDL_Color ghostColor = color; ghostColor.a = GHOST_ALPHA;
    drawPiece(game.ghost(), ghostColor);
    drawPiece(game.current(), color);
}

// 右側の情報欄（NEXT・スコア・操作説明）
void Renderer::drawSidePanel(const Tetris& game) {
    float sx = W * CELL + 16.0f;
    SDL_SetRenderDrawColor(r_, 255, 255, 255, 255);
    SDL_RenderDebugText(r_, sx, 20, "NEXT");
    int next = game.nextType();
    setColor(pieceColor(next));
    for (int py = 0; py < 4; py++)
        for (int px = 0; px < 4; px++)
            if (SHAPES[next][py][px]) {
                SDL_FRect rc{sx + px * 24.0f, 40 + py * 24.0f, 22, 22};
                SDL_RenderFillRect(r_, &rc);
            }

    SDL_SetRenderDrawColor(r_, 255, 255, 255, 255);
    SDL_RenderDebugText(r_, sx, 160, ("SCORE " + std::to_string(game.score())).c_str());
    SDL_RenderDebugText(r_, sx, 180, ("LINES " + std::to_string(game.lines())).c_str());
    SDL_RenderDebugText(r_, sx, 200, ("LEVEL " + std::to_string(game.level())).c_str());

    // 操作説明：左にキー（明るめ）、右に操作内容（暗め）
    const float helpY = 260, lineH = 16, actionX = sx + 56;
    SDL_RenderDebugText(r_, sx, helpY, "CONTROLS");
    for (int i = 0; i < (int)SDL_arraysize(CONTROLS); i++) {
        float y = helpY + 24 + i * lineH;
        SDL_SetRenderDrawColor(r_, 220, 220, 220, 255);
        SDL_RenderDebugText(r_, sx, y, CONTROLS[i].key);
        SDL_SetRenderDrawColor(r_, 140, 140, 150, 255);
        SDL_RenderDebugText(r_, actionX, y, CONTROLS[i].action);
    }
}

// 画面全体を暗くし、GAME OVER をフェードインさせ、終わったら項目を出す
void Renderer::drawGameOver(const GameOverScreen& gameOver) {
    float t = gameOver.fade();
    if (t <= 0.0f) return;                          // まだブロックのグレー化中
    SDL_SetRenderDrawColor(r_, 0, 0, 0, (Uint8)(GAMEOVER_DIM_ALPHA * t));
    SDL_RenderFillRect(r_, nullptr);

    const float titleY = WIN_H * 0.4f;
    drawTextCentered("GAME OVER", titleY, 4.0f, {255, 80, 80, (Uint8)(255 * t)});
    if (!gameOver.menuVisible()) return;

    for (int i = 0; i < MENU_COUNT; i++) {
        bool selected = (i == gameOver.cursor());
        std::string label = selected ? std::string("> ") + MENU_LABELS[i] + " <" : MENU_LABELS[i];
        SDL_Color c = selected ? SDL_Color{255, 230, 0, 255} : SDL_Color{160, 160, 160, 255};
        drawTextCentered(label.c_str(), titleY + 70 + i * 36.0f, 2.0f, c);
    }
}

// ウィンドウの横中央、縦位置 centerY に scale 倍の文字を描く
void Renderer::drawTextCentered(const char* text, float centerY, float scale, SDL_Color c) {
    const float glyph = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * scale;
    float x = (WIN_W - SDL_strlen(text) * glyph) / 2;
    float y = centerY - glyph / 2;
    setColor(c);
    SDL_SetRenderScale(r_, scale, scale);           // 座標も scale 倍されるので割って渡す
    SDL_RenderDebugText(r_, x / scale, y / scale, text);
    SDL_SetRenderScale(r_, 1.0f, 1.0f);
}

void Renderer::drawCell(int x, int y, SDL_Color c) {
    SDL_FRect rc{x * (float)CELL + 1, y * (float)CELL + 1, CELL - 2.0f, CELL - 2.0f};
    setColor(c);
    SDL_RenderFillRect(r_, &rc);
}

void Renderer::drawPiece(const Piece& p, SDL_Color c) {
    for (int py = 0; py < 4; py++)
        for (int px = 0; px < 4; px++)
            if (p.shape[py][px] && p.y + py >= 0)
                drawCell(p.x + px, p.y + py, c);
}

void Renderer::setColor(SDL_Color c) {
    SDL_SetRenderDrawColor(r_, c.r, c.g, c.b, c.a);
}
