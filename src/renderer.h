// 画面の描画（ゲームの状態を読んで描くだけで、状態は変えない）
#pragma once
#include <SDL3/SDL.h>

class Tetris;
class GameOverScreen;
struct Piece;

class Renderer {
public:
    explicit Renderer(SDL_Renderer* r) : r_(r) {}

    // 1フレーム分を描いて画面に出す（ゲームオーバー中は gameOver の演出も重ねる）
    void drawFrame(const Tetris& game, const GameOverScreen& gameOver);

private:
    SDL_Renderer* r_;

    void drawField(const Tetris& game, const GameOverScreen& gameOver);
    void drawSidePanel(const Tetris& game);
    void drawGameOver(const GameOverScreen& gameOver);

    void drawTextCentered(const char* text, float centerY, float scale, SDL_Color c);
    void drawCell(int x, int y, SDL_Color c);
    void drawPiece(const Piece& p, SDL_Color c);
    void setColor(SDL_Color c);
};
