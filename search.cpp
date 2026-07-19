#include "search.h"
#include "move_generator.h"
#include "make_move.h"
#include "attacks.h"
#include "evaluate_position.h"
#include "search.h"

#include <iostream>

const int INFINITY_SCORE = 50000;
const int MATE_VALUE = 49000;

int negamax(Position &pos, int depth, int alpha, int beta)
{
    if (depth == 0)
    {
        return evaluate_postion(pos);
    }

    MoveList move_list;
    generate_pseudo_legal_moves(pos, move_list);

    int legal_moves = 0;

    for (int i = 0; i < move_list.count; i++)
    {
        Position copy = pos;

        if (make_move(pos, move_list.moves[i]))
        {
            legal_moves++;

            int score = -negamax(pos, depth - 1, -beta, -alpha);

            pos = copy; // go back to the previous position

            //
            if (score >= beta)
                return beta; // if enemy has a better position we cut of the tree
            if (score > alpha)
                alpha = score; // if we find a better move
        }
    }

    // no legal moves
    if (legal_moves == 0)
    {
        int king_piece = (pos.side_to_move == white) ? w_king : b_king;
        int king_sq = __builtin_ctzll(pos.bitboards[king_piece]);

        // checking mate or draw
        if (is_squere_attacked(king_sq, pos.side_to_move ^ 1, pos))
        {
            return -MATE_VALUE; // mate
        }
        else
        {
            return 0; // draw
        }
    }

    return alpha;
}

Move search(Position &pos, int depth)
{

    MoveList move_list;
    generate_pseudo_legal_moves(pos, move_list);

    int best_score = -INFINITY_SCORE;

    Move best_move = 0;

    int alpha = -INFINITY_SCORE;
    int beta = INFINITY_SCORE;

    for (int i = 0; i < move_list.count; i++)
    {
        Position copy = pos;
        if (make_move(pos, move_list.moves[i]))
        {
            int score = -negamax(pos, depth - 1, -beta, -alpha);
            pos = copy;

            // if better move then save it
            if (score > best_score)
            {
                best_score = score;
                best_move = move_list.moves[i];
            }
            if (score > alpha)
                alpha = score;
        }
    }
    return best_move;
}
