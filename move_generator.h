#pragma once
#include "types.h"
#include "bitboard.h"

struct MoveList
{
    Move moves[256];
    int count = 0;

    inline void add_move(Move move)
    {
        moves[count] = move;
        count++;
    }
};

void generate_pseudo_legal_moves(Position &pos, MoveList &move_list);