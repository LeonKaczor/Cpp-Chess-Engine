#include "types.h"
#include "bitboard.h"
#include "evaluate_position.h"
#include "attacks.h"
#include "move_generator.h"
#include "make_move.h"

// function that flips bit number to the correspodning squere for white,
// its used when calculating score for white pices

// 56 = 111000
// for example A1 => 0^56 = 56 => A8
// the file stays the same just the row changes

inline int flip(int squere)
{
    return squere ^ 56;
}

// tables with score constatns
const int piece_values[12] = {
    100, 500, 320, 330, 900, 20000,
    100, 500, 320, 330, 900, 20000};

const int knight_position_values[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};

const int pawn_position_values[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};

const int bishop_position_values[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};

const int rook_position_values[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0};

const int queen_position_values[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20};

const int king_midgame_position_values[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20 // on castling position we get a bonus
};

// king whats to go to the center
const int king_endgame_position_values[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -30, 0, 0, 0, 0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50};

// table for attacks

const int attacks_bonus[6] = {
    0, // pawn
    3, // rook
    4, // knight
    5, // bishop
    2, // queen
    0  // king
};

// masks for columns
// used for doubled pawns evaluation
const U64 file_masks[8] = {
    0x0101010101010101ULL, // A
    0x0202020202020202ULL, // B
    0x0404040404040404ULL, // C
    0x0808080808080808ULL, // D
    0x1010101010101010ULL, // E
    0x2020202020202020ULL, // F
    0x4040404040404040ULL, // G
    0x8080808080808080ULL  // H
};

// adding passed pawn bonus - endgame

const int passed_pawn_bonus[8] = {0, 10, 30, 50, 75, 120, 150, 0};

// penalty for doubled pawns
const int doubled_pawn_penalty = 15;

int evaluate_postion(Position &pos)
{
    // __builtin_popcountl
    // __builtin_popcount()  is a built-in function of GCC compiler.
    // This function is used to count the number of set bits in an unsigned integer.
    // In other words, it counts the number of 1's in the binary form of a positive integer.

    // pices values
    int score = 0;
    // for white
    score += __builtin_popcountll(pos.bitboards[w_pawn]) * piece_values[w_pawn];
    score += __builtin_popcountll(pos.bitboards[w_knight]) * piece_values[w_knight];
    score += __builtin_popcountll(pos.bitboards[w_bishop]) * piece_values[w_bishop];
    score += __builtin_popcountll(pos.bitboards[w_rook]) * piece_values[w_rook];
    score += __builtin_popcountll(pos.bitboards[w_queen]) * piece_values[w_queen];

    // for black
    score -= __builtin_popcountll(pos.bitboards[b_pawn]) * piece_values[b_pawn];
    score -= __builtin_popcountll(pos.bitboards[b_knight]) * piece_values[b_knight];
    score -= __builtin_popcountll(pos.bitboards[b_bishop]) * piece_values[b_bishop];
    score -= __builtin_popcountll(pos.bitboards[b_rook]) * piece_values[b_rook];
    score -= __builtin_popcountll(pos.bitboards[b_queen]) * piece_values[b_queen];

    // positioning of pieces

    // knights
    U64 knights_w = pos.bitboards[w_knight];
    while (knights_w)
    {
        int squere = __builtin_ctzll(knights_w);
        clear_bit(knights_w, squere);

        score += knight_position_values[flip(squere)];

        U64 attacks = knight_attacks[squere] & ~pos.occupancies[white];
        score += __builtin_popcountll(attacks) * attacks_bonus[knight];
    }

    U64 knights_b = pos.bitboards[b_knight];
    while (knights_b)
    {
        int squere = __builtin_ctzll(knights_b);
        clear_bit(knights_b, squere);
        score -= knight_position_values[(squere)];

        U64 attacks = knight_attacks[squere] & ~pos.occupancies[black];
        score -= __builtin_popcountll(attacks) * attacks_bonus[knight];
    }

    // pawns
    U64 pawns_w = pos.bitboards[w_pawn];
    while (pawns_w)
    {
        int squere = __builtin_ctzll(pawns_w);
        clear_bit(pawns_w, squere);
        score += pawn_position_values[flip(squere)];

        // adding bonus for free pawn
        if ((passed_pawn_masks_white[squere] & pos.bitboards[b_pawn]) == 0)
        {
            int rank = squere / 8;
            score += passed_pawn_bonus[rank];
        }
    }

    U64 pawns_b = pos.bitboards[b_pawn];
    while (pawns_b)
    {
        int squere = __builtin_ctzll(pawns_b);
        clear_bit(pawns_b, squere);
        score -= pawn_position_values[(squere)];

        // adding bonus for free pawn
        if ((passed_pawn_masks_black[squere] & pos.bitboards[w_pawn]) == 0)
        {
            int rank = squere / 8;
            score += passed_pawn_bonus[rank];
        }
    }

    // bishops
    U64 bishops_w = pos.bitboards[w_bishop];
    while (bishops_w)
    {
        int squere = __builtin_ctzll(bishops_w);
        clear_bit(bishops_w, squere);
        score += bishop_position_values[flip(squere)];

        U64 attacks = mask_bishops_attacks(squere, pos.occupancies[both]) & ~pos.occupancies[white];
        score += __builtin_popcountll(attacks) * attacks_bonus[bishop];
    }

    U64 bishops_b = pos.bitboards[b_bishop];
    while (bishops_b)
    {
        int squere = __builtin_ctzll(bishops_b);
        clear_bit(bishops_b, squere);
        score -= bishop_position_values[(squere)];

        U64 attacks = mask_bishops_attacks(squere, pos.occupancies[both]) & ~pos.occupancies[black];
        score -= __builtin_popcountll(attacks) * attacks_bonus[bishop];
    }

    // rook
    U64 rook_w = pos.bitboards[w_rook];
    while (rook_w)
    {
        int squere = __builtin_ctzll(rook_w);
        clear_bit(rook_w, squere);
        score += rook_position_values[flip(squere)];

        U64 attacks = mask_rooks_attacks(squere, pos.occupancies[both]) & ~pos.occupancies[white];
        score += __builtin_popcountll(attacks) * attacks_bonus[rook];
    }

    U64 rook_b = pos.bitboards[b_rook];
    while (rook_b)
    {
        int squere = __builtin_ctzll(rook_b);
        clear_bit(rook_b, squere);
        score -= rook_position_values[(squere)];

        U64 attacks = mask_rooks_attacks(squere, pos.occupancies[both]) & ~pos.occupancies[black];
        score -= __builtin_popcountll(attacks) * attacks_bonus[rook];
    }

    // queens
    U64 queen_w = pos.bitboards[w_queen];
    while (queen_w)
    {
        int squere = __builtin_ctzll(queen_w);
        clear_bit(queen_w, squere);
        score += queen_position_values[flip(squere)];

        U64 attacks = mask_queen_attacks(squere, pos.occupancies[both]) & ~pos.occupancies[white];
        score += __builtin_popcountll(attacks) * attacks_bonus[queen];
    }

    U64 queen_b = pos.bitboards[b_queen];
    while (queen_b)
    {
        int squere = __builtin_ctzll(queen_b);
        clear_bit(queen_b, squere);
        score -= queen_position_values[(squere)];

        U64 attacks = mask_queen_attacks(squere, pos.occupancies[both]) & ~pos.occupancies[black];
        score -= __builtin_popcountll(attacks) * attacks_bonus[queen];
    }

    // king

    int non_pawn_material = 0;

    non_pawn_material += __builtin_popcountll(pos.bitboards[w_knight] | pos.bitboards[b_knight]) * piece_values[w_knight];
    non_pawn_material += __builtin_popcountll(pos.bitboards[w_bishop] | pos.bitboards[b_bishop]) * piece_values[w_bishop];
    non_pawn_material += __builtin_popcountll(pos.bitboards[w_rook] | pos.bitboards[b_rook]) * piece_values[w_rook];
    non_pawn_material += __builtin_popcountll(pos.bitboards[w_queen] | pos.bitboards[b_queen]) * piece_values[w_queen];

    bool is_endgame = (non_pawn_material <= 1600);

    U64 king_w = pos.bitboards[w_king];
    while (king_w)
    {
        int square = __builtin_ctzll(king_w);
        clear_bit(king_w, square);
        if (is_endgame)
        {
            score += king_endgame_position_values[flip(square)];
        }
        else
        {
            score += king_midgame_position_values[flip(square)];
        }
    }

    U64 king_b = pos.bitboards[b_king];
    while (king_b)
    {
        int square = __builtin_ctzll(king_b);
        clear_bit(king_b, square);

        if (is_endgame)
        {
            score -= king_endgame_position_values[(square)];
        }
        else
        {
            score -= king_midgame_position_values[(square)];
        }
    }

    // to promote developing pieces in the oppening
    if (pos.fullmove_number < 5)
    {

        if (pos.bitboards[w_knight] & (1ULL << 1))
        {
            score -= 15;
        }
        if (pos.bitboards[w_knight] & (1ULL << 6))
        {
            score -= 15;
        }
        if (pos.bitboards[w_bishop] & (1ULL << 2))
        {
            score -= 15;
        }
        if (pos.bitboards[w_bishop] & (1ULL << 5))
        {
            score -= 15;
        }
        if (pos.bitboards[b_knight] & (1ULL << 57))
        {
            score += 15;
        }
        if (pos.bitboards[b_knight] & (1ULL << 62))
        {
            score += 15;
        }
        if (pos.bitboards[b_bishop] & (1ULL << 58))
        {
            score += 15;
        }
        if (pos.bitboards[b_bishop] & (1ULL << 61))
        {
            score += 15;
        }
    }

    // doubled pawns penalty
    for (int i = 0; i < 8; i++)
    {
        int w_pawns_on_file = __builtin_popcountll(pos.bitboards[w_pawn] & file_masks[i]);
        // if doubled pawns white
        if (w_pawns_on_file > 1)
        {
            score -= (w_pawns_on_file - 1) * doubled_pawn_penalty;
        }

        // black
        int b_pawns_on_file = __builtin_popcountll(pos.bitboards[b_pawn] & file_masks[i]);
        if (b_pawns_on_file > 1)
        {
            score += (b_pawns_on_file - 1) * doubled_pawn_penalty;
        }
    }

    // pair of bishops is powerful
    if (__builtin_popcountll(pos.bitboards[w_bishop]) >= 2)
        score += 20;
    if (__builtin_popcountll(pos.bitboards[b_bishop]) >= 2)
        score -= 20;

    // for minmax convention + is winning - is loosing for both sides
    return (pos.side_to_move == white) ? score : -score;
    // condition ? if true : if black => get -- score + for black winning
}
int get_king_square(Position &pos, int side)
{
    int king_piece = (side == white) ? w_king : b_king;

    for (int sq = 0; sq < 64; sq++)
    {
        if (get_bit(pos.bitboards[king_piece], sq))
        {
            return sq;
        }
    }
    return -1;
}

// 0 game is on 1 white win 2 black win 3 draw
int check_game_over(Position &pos)
{
    MoveList move_list;
    generate_pseudo_legal_moves(pos, move_list);
    bool has_legal_moves = false;

    // check if ther are any legal move
    for (int i = 0; i < move_list.count; i++)
    {
        Position copy = pos;
        if (make_move(pos, move_list.moves[i]))
        {
            pos = copy;
            has_legal_moves = true;
            break; // if there is legal move and function
        }
    }

    // analyse of situation when no legal moves
    if (!has_legal_moves)
    {
        // check where is king of player who is playing
        int king_square = get_king_square(pos, pos.side_to_move);
        int opponent = (pos.side_to_move == white) ? black : white;

        if (is_squere_attacked(king_square, opponent, pos))
        {
            // mat
            if (pos.side_to_move == white)
                return 2; // black win
            else
                return 1; // white win
        }
        else
        {
            // draw
            return 3;
        }
    }

    return 0; // game is on
}
