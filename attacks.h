#pragma once
#include "types.h"

U64 generate_knights_attacks(int square);
U64 generate_king_attacks(int square);
U64 generate_white_pawn_attacks(int square);
U64 generate_black_pawn_attacks(int square);
void init_leaper_attacks();
U64 mask_rooks_attacks(int square, U64 blockers);
U64 mask_bishops_attacks(int square, U64 blockers);
U64 mask_queen_attacks(int square, U64 blockers);
bool is_squere_attacked(int square, int whos_attacking, Position &pos);

// lists with attacks

extern U64 rook_attacks[64];
extern U64 knight_attacks[64];
extern U64 bishop_attacks[64];
extern U64 king_attacks[64];
extern U64 queen_attacks[64];
extern U64 pawn_attacks[2][64];

extern U64 passed_pawn_masks_white[64];
extern U64 passed_pawn_masks_black[64];
