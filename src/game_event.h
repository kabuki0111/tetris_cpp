// ゲーム内で起きた出来事（効果音などのきっかけに使う）
#pragma once

enum GameEvent { EVENT_MOVE, EVENT_ROTATE, EVENT_LOCK, EVENT_CLEAR, EVENT_TETRIS, EVENT_GAMEOVER, EVENT_COUNT };

// 複数の出来事をビットで束ねたもの
using GameEvents = unsigned;

inline constexpr GameEvents eventBit(GameEvent e) { return 1u << e; }
