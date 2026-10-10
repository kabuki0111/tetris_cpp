// SDL3 テトリス
// 操作: ←→ 移動 / ↓ ソフトドロップ / ↑ or X 右回転 / Z 左回転 / Space ハードドロップ / R リスタート / Esc 終了
// ゲームオーバー時: ↑↓ 項目選択 / Enter or Space 決定
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include "audio.h"
#include "config.h"
#include "game_over_screen.h"
#include "renderer.h"
#include "tetris.h"

int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    SDL_Window* window = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Tetris (SDL3)", WIN_W, WIN_H, 0, &window, &sdlRenderer)) {
        SDL_Log("Create failed: %s", SDL_GetError());
        return 1;
    }
    SDL_SetRenderVSync(sdlRenderer, 1);
    SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);

    // サウンドは実行ファイルの横の assets フォルダから読む
    const char* base = SDL_GetBasePath();
    Audio audio;
    audio.load(std::string(base ? base : "") + "assets/");

    Tetris game;
    GameOverScreen gameOverScreen;
    Renderer renderer(sdlRenderer);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode key = e.key.key;
                if (key == SDLK_ESCAPE) running = false;
                else if (!game.isGameOver()) game.onKey(key);
                else switch (gameOverScreen.onKey(key)) {
                    case GameOverScreen::Action::Restart: game.reset(); break;
                    case GameOverScreen::Action::Quit:    running = false; break;
                    case GameOverScreen::Action::None:    break;
                }
            }
        }

        Uint64 now = SDL_GetTicks();
        game.update(now);
        gameOverScreen.update(now);

        GameEvents events = game.takeEvents();
        if (events & eventBit(EVENT_GAMEOVER)) gameOverScreen.start(now);
        audio.setBgmPlaying(!game.isGameOver());    // ゲームオーバーで停止、再スタートで頭から
        bool danger = game.stackHeight() >= DANGER_HEIGHT;
        audio.setBgmSpeed(danger ? BGM_DANGER_SPEED : 1.0f);   // 高く積まれたら BGM を速くする
        audio.update(events);

        renderer.drawFrame(game, gameOverScreen);
    }

    audio.release();
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
