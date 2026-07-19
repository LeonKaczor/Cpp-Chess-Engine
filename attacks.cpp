#include <iostream>
#include "types.h"
#include "bitboard.h"
#include <cmath>

// for program to be able to compute all possible attacks a common practice
// is to define tables with all possible attack from all squares before the chess engine starts
// this optimizes the process of looking for the best move as its not neccesery to compute all optiones
// every time the position changes

U64 rook_attacks[64];
U64 knight_attacks[64]; // done // done attacking
U64 bishop_attacks[64];
U64 king_attacks[64]; // done // done attacks but not castling
U64 queen_attacks[64];

U64 pawn_attacks[2][64]; // [color][square] //done //attacks added

// this is used to prevent jumping over the boundries of the board
// 0x => hexadecimal system XX => binary representation of 8 bits
const U64 not_A_file = 0xFEFEFEFEFEFEFEFEULL;  // fe = 11111110
const U64 not_H_file = 0x7F7F7F7F7F7F7F7FULL;  // 7F = 01111111
const U64 not_AB_file = 0xFCFCFCFCFCFCFCFCULL; // FC = 11111100
const U64 not_GH_file = 0x3F3F3F3F3F3F3F3FULL; // 3F = 00111111

U64 generate_knights_attacks(int square)
{
    U64 bitboard = 0ULL;
    U64 attacks = 0ULL;

    on_bit(bitboard, square);
    // atacks to the top
    attacks |= (bitboard & not_H_file) << 17;
    attacks |= (bitboard & not_A_file) << 15;
    attacks |= (bitboard & not_GH_file) << 10;
    attacks |= (bitboard & not_AB_file) << 6;

    // atacks to the bottom
    attacks |= (bitboard & not_A_file) >> 17;
    attacks |= (bitboard & not_H_file) >> 15;
    attacks |= (bitboard & not_AB_file) >> 10;
    attacks |= (bitboard & not_GH_file) >> 6;

    return attacks;
}

U64 generate_king_attacks(int square)
{
    U64 bitboard = 0ULL;
    U64 attacks = 0ULL;

    on_bit(bitboard, square);

    attacks |= (bitboard & not_H_file) << 1;
    attacks |= (bitboard) << 8;
    attacks |= (bitboard) >> 8;
    attacks |= (bitboard & not_A_file) >> 1;

    // diagonal
    attacks |= (bitboard & not_H_file) << 9;
    attacks |= (bitboard & not_H_file) >> 7;

    attacks |= (bitboard & not_A_file) << 7;
    attacks |= (bitboard & not_A_file) >> 9;

    return attacks;
}

U64 generate_white_pawn_attacks(int square)
{
    U64 bitboard = 0ULL;
    U64 attacks_white = 0ULL;

    on_bit(bitboard, square);

    attacks_white |= (bitboard & not_H_file) << 9;
    attacks_white |= (bitboard & not_A_file) << 7;

    return attacks_white;
}

U64 generate_black_pawn_attacks(int square)
{
    U64 bitboard = 0ULL;
    U64 attacks_black = 0ULL;

    on_bit(bitboard, square);

    attacks_black |= (bitboard & not_H_file) >> 7;
    attacks_black |= (bitboard & not_A_file) >> 9;

    return attacks_black;
}

void init_leaper_attacks()
{
    for (int square = 0; square < 64; square++)
    {
        knight_attacks[square] = generate_knights_attacks(square);
        king_attacks[square] = generate_king_attacks(square);

        pawn_attacks[0][square] = generate_white_pawn_attacks(square);
        pawn_attacks[1][square] = generate_black_pawn_attacks(square);
    }
}

// all moves possible for sliding with occupanices considered

U64 mask_rooks_attacks(int square, U64 blockers)
{
    U64 attacks = 0ULL;
    int r = square / 8;
    int f = square % 8;

    // upp
    for (int rank = r + 1; rank <= 7; rank++)
    {
        int sq = rank * 8 + f;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break; // if we are on blocked squere then stop going
    }

    // down
    for (int rank = r - 1; rank >= 0; rank--)
    {
        int sq = rank * 8 + f;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    // right
    for (int file = f + 1; file <= 7; file++)
    {
        int sq = r * 8 + file;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    //  left
    for (int file = f - 1; file >= 0; file--)
    {
        int sq = r * 8 + file;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    return attacks;
}

U64 mask_bishops_attacks(int square, U64 blockers)
{
    U64 attacks = 0ULL;
    int r = square / 8;
    int f = square % 8;

    // upp + right
    for (int rank = r + 1, file = f + 1; rank <= 7 && file <= 7; rank++, file++)
    {
        int sq = rank * 8 + file;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    // down + right
    for (int rank = r - 1, file = f + 1; rank >= 0 && file <= 7; rank--, file++)
    {
        int sq = rank * 8 + file;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    // down + left
    for (int rank = r - 1, file = f - 1; rank >= 0 && file >= 0; rank--, file--)
    {
        int sq = rank * 8 + file;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    // upp + left
    for (int rank = r + 1, file = f - 1; rank <= 7 && file >= 0; rank++, file--)
    {
        int sq = rank * 8 + file;
        on_bit(attacks, sq);
        if (get_bit(blockers, sq))
            break;
    }

    return attacks;
}

U64 mask_queen_attacks(int square, U64 blockers)
{
    return mask_rooks_attacks(square, blockers) | mask_bishops_attacks(square, blockers);
}

// function that check if a given square is attacked - used for castiling checks etc
bool is_squere_attacked(int square, int whos_attacking, Position &pos)
{
    if (whos_attacking == white)
    {
        if (pawn_attacks[black][square] & pos.bitboards[w_pawn])
        {
            return true;
        }
        if (knight_attacks[square] & pos.bitboards[w_knight])
        {
            return true;
        }
        if (king_attacks[square] & pos.bitboards[w_king])
        {
            return true;
        }
        if (mask_rooks_attacks(square, pos.occupancies[both]) & pos.bitboards[w_rook])
        {
            return true;
        }
        if (mask_bishops_attacks(square, pos.occupancies[both]) & pos.bitboards[w_bishop])
        {
            return true;
        }
        if (mask_queen_attacks(square, pos.occupancies[both]) & pos.bitboards[w_queen])
        {
            return true;
        }
    }
    else
    {
        if (pawn_attacks[white][square] & pos.bitboards[b_pawn])
        {
            return true;
        }
        if (knight_attacks[square] & pos.bitboards[b_knight])
        {
            return true;
        }
        if (king_attacks[square] & pos.bitboards[b_king])
        {
            return true;
        }
        if (mask_rooks_attacks(square, pos.occupancies[both]) & pos.bitboards[b_rook])
        {
            return true;
        }
        if (mask_bishops_attacks(square, pos.occupancies[both]) & pos.bitboards[b_bishop])
        {
            return true;
        }
        if (mask_queen_attacks(square, pos.occupancies[both]) & pos.bitboards[b_queen])
        {
            return true;
        }
    }

    return false;
}