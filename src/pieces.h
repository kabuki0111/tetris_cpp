// ミノの形と、盤面上のミノ
#pragma once
#include <array>

using Shape = std::array<std::array<int, 4>, 4>;

inline constexpr int PIECE_TYPES = 7;
inline constexpr int PIECE_I = 0, PIECE_O = 1;      // 回転で特別扱いするミノの種類

// 7種類のミノ（I, O, T, S, Z, J, L）
inline constexpr Shape SHAPES[PIECE_TYPES] = {
    {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}},
    {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
    {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}},
    {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}},
    {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
    {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}},
    {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}},
};

// 盤面上のミノ（形・左上の位置・種類）
struct Piece { Shape shape; int x, y, type; };
