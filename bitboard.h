#pragma once
#include "types.h"

struct Position
{
    U64 bitboards[12]; // 12 bitboards for 12 pices types
    U64 occupancies[3];

    // state of the game
    int side_to_move; // white 0 black 1
    int en_passant_squeres;
    int castling_rights;

    int halfmoves;       // rule of 50 moves without capture
    int fullmove_number; // couting number of moves played

    int win; // 0 - game is on 1 => white won 2=> black won 3 => draw
};

// here I define basic functions for board manipulation
// using inline for optimization

// OR
inline void on_bit(U64 &bitboard, int square)
{
    bitboard |= (1ULL << square);
}

// and with reversed bitboard
inline void clear_bit(U64 &bitboard, int square)
{
    bitboard &= ~(1ULL << square);
}

inline bool get_bit(U64 bitboard, int square)
{
    return (bitboard & (1ULL << square)) != 0;
    // returns 1 != 0 true if there is a piece on square
}

void board_setup(Position &pos);

void print_bitboard(U64 bitboard);

void update_occupancies(Position &pos);
