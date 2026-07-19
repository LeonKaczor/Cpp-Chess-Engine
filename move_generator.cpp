#include "move_generator.h"
#include "attacks.h"
#include "bitboard.h"
#include "types.h"

void generate_pseudo_legal_moves(Position &pos, MoveList &move_list)
{
    if (pos.side_to_move == white)
    {
        // pawns moves
        U64 pawns = pos.bitboards[w_pawn];
        while (pawns)
        {
            int source_square = __builtin_ctzll(pawns);

            clear_bit(pawns, source_square);

            int single_push = source_square + 8;

            if (!get_bit(pos.occupancies[both], single_push))
            {
                if (source_square >= 48 && source_square <= 55)
                {
                    move_list.add_move(encode_move(source_square, single_push, PromoteToQueen));
                    move_list.add_move(encode_move(source_square, single_push, PromoteToKnight));
                    move_list.add_move(encode_move(source_square, single_push, PromoteToRook));
                    move_list.add_move(encode_move(source_square, single_push, PromoteToBishop));
                }
                else
                {
                    move_list.add_move(encode_move(source_square, single_push, QuietMove));

                    if (source_square >= 8 && source_square <= 15)
                    {
                        int double_push = source_square + 16;

                        if (!get_bit(pos.occupancies[both], double_push))
                        {
                            move_list.add_move(encode_move(source_square, double_push, DoublePawnPush));
                        }
                    }
                }
            }

            U64 attacks = pawn_attacks[white][source_square];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);

                if (get_bit(pos.occupancies[black], target_squere))
                {
                    if (source_square >= 48 && source_square <= 55)
                    {
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                    }
                    else
                    {
                        move_list.add_move(encode_move(source_square, target_squere, Capture));
                    }
                }
                else if (target_squere == pos.en_passant_squeres && pos.en_passant_squeres != NO_SQ)
                {
                    move_list.add_move(encode_move(source_square, target_squere, EnPassant));
                }
            }
        }

        // king moves
        U64 king = pos.bitboards[w_king];
        if (king)
        {

            int source_squere = __builtin_ctzll(king);
            U64 attacks = king_attacks[source_squere];
            attacks &= ~pos.occupancies[white];

            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[black], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
            if (pos.castling_rights & wk)
            {
                if (!get_bit(pos.occupancies[both], F1) && !get_bit(pos.occupancies[both], G1))
                {
                    if (!is_squere_attacked(E1, black, pos) && !is_squere_attacked(F1, black, pos) && !is_squere_attacked(G1, black, pos))
                    {
                        move_list.add_move(encode_move(E1, G1, KingCastle));
                    }
                }
            }
            if (pos.castling_rights & wq)
            {
                if (!get_bit(pos.occupancies[both], D1) && !get_bit(pos.occupancies[both], C1) && !get_bit(pos.occupancies[both], B1))
                {
                    if (!is_squere_attacked(E1, black, pos) && !is_squere_attacked(D1, black, pos) && !is_squere_attacked(C1, black, pos))
                    {
                        move_list.add_move(encode_move(E1, C1, QueenCastle));
                    }
                }
            }
        }

        // knight moves
        U64 knight = pos.bitboards[w_knight];
        while (knight)
        {
            int source_squere = __builtin_ctzll(knight);
            clear_bit(knight, source_squere);
            U64 attacks = knight_attacks[source_squere];
            attacks &= ~pos.occupancies[white];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[black], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }

        // SLIDING FUGUYRES

        // rooks
        U64 rooks = pos.bitboards[w_rook];
        while (rooks)
        {
            int source_squere = __builtin_ctzll(rooks);
            clear_bit(rooks, source_squere);

            U64 attacks = mask_rooks_attacks(source_squere, pos.occupancies[both]);
            attacks &= ~pos.occupancies[white];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[black], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }

        // bishops

        U64 bishops = pos.bitboards[w_bishop];
        while (bishops)
        {
            int source_squere = __builtin_ctzll(bishops);
            clear_bit(bishops, source_squere);
            U64 attacks = mask_bishops_attacks(source_squere, pos.occupancies[both]);
            attacks &= ~pos.occupancies[white];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[black], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }

        // queens

        U64 queens = pos.bitboards[w_queen];
        while (queens)
        {
            int source_squere = __builtin_ctzll(queens);
            clear_bit(queens, source_squere);

            U64 attacks = mask_queen_attacks(source_squere, pos.occupancies[both]);
            attacks &= ~pos.occupancies[white];

            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[black], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }
    }

    // black

    else
    {
        // pawns moves
        U64 pawns = pos.bitboards[b_pawn];
        while (pawns)
        {
            int source_square = __builtin_ctzll(pawns);

            clear_bit(pawns, source_square);

            int single_push = source_square - 8;

            if (!get_bit(pos.occupancies[both], single_push))
            {
                if (source_square >= 8 && source_square <= 15)
                {
                    move_list.add_move(encode_move(source_square, single_push, PromoteToQueen));
                    move_list.add_move(encode_move(source_square, single_push, PromoteToKnight));
                    move_list.add_move(encode_move(source_square, single_push, PromoteToRook));
                    move_list.add_move(encode_move(source_square, single_push, PromoteToBishop));
                }
                else
                {
                    move_list.add_move(encode_move(source_square, single_push, QuietMove));

                    if (source_square >= 48 && source_square <= 55)
                    {
                        int double_push = source_square - 16;

                        if (!get_bit(pos.occupancies[both], double_push))
                        {
                            move_list.add_move(encode_move(source_square, double_push, DoublePawnPush));
                        }
                    }
                }
            }

            U64 attacks = pawn_attacks[black][source_square];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);

                if (get_bit(pos.occupancies[white], target_squere))
                {
                    if (source_square >= 8 && source_square <= 15)
                    {
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                        move_list.add_move(encode_move(source_square, target_squere, PromoteToQueen_capture));
                    }
                    else
                    {
                        move_list.add_move(encode_move(source_square, target_squere, Capture));
                    }
                }
                else if (target_squere == pos.en_passant_squeres && pos.en_passant_squeres != NO_SQ)
                {
                    move_list.add_move(encode_move(source_square, target_squere, EnPassant));
                }
            }
        }

        // king moves
        U64 king = pos.bitboards[b_king];
        if (king)
        {

            int source_squere = __builtin_ctzll(king);
            U64 attacks = king_attacks[source_squere];
            attacks &= ~pos.occupancies[black];

            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[white], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
            if (pos.castling_rights & bk)
            {
                if (!get_bit(pos.occupancies[both], F8) && !get_bit(pos.occupancies[both], G8))
                {
                    if (!is_squere_attacked(E8, white, pos) && !is_squere_attacked(F8, white, pos) && !is_squere_attacked(G8, white, pos))
                    {
                        move_list.add_move(encode_move(E8, G8, KingCastle));
                    }
                }
            }
            if (pos.castling_rights & bq)
            {
                if (!get_bit(pos.occupancies[both], D8) && !get_bit(pos.occupancies[both], C8) && !get_bit(pos.occupancies[both], B8))
                {
                    if (!is_squere_attacked(E8, white, pos) && !is_squere_attacked(D8, white, pos) && !is_squere_attacked(C8, white, pos))
                    {
                        move_list.add_move(encode_move(E8, C8, QueenCastle));
                    }
                }
            }
        }

        // knight moves
        U64 knight = pos.bitboards[b_knight];
        while (knight)
        {
            int source_squere = __builtin_ctzll(knight);
            clear_bit(knight, source_squere);
            U64 attacks = knight_attacks[source_squere];
            attacks &= ~pos.occupancies[black];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[white], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }

        // SLIDING FUGUYRES

        // rooks
        U64 rooks = pos.bitboards[b_rook];
        while (rooks)
        {
            int source_squere = __builtin_ctzll(rooks);
            clear_bit(rooks, source_squere);

            U64 attacks = mask_rooks_attacks(source_squere, pos.occupancies[both]);
            attacks &= ~pos.occupancies[black];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[white], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }

        // bishops

        U64 bishops = pos.bitboards[b_bishop];
        while (bishops)
        {
            int source_squere = __builtin_ctzll(bishops);
            clear_bit(bishops, source_squere);
            U64 attacks = mask_bishops_attacks(source_squere, pos.occupancies[both]);
            attacks &= ~pos.occupancies[black];
            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[white], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }

        // queens

        U64 queens = pos.bitboards[b_queen];
        while (queens)
        {
            int source_squere = __builtin_ctzll(queens);
            clear_bit(queens, source_squere);

            U64 attacks = mask_queen_attacks(source_squere, pos.occupancies[both]);
            attacks &= ~pos.occupancies[black];

            while (attacks)
            {
                int target_squere = __builtin_ctzll(attacks);
                clear_bit(attacks, target_squere);
                if (get_bit(pos.occupancies[white], target_squere))
                {
                    move_list.add_move(encode_move(source_squere, target_squere, Capture));
                }
                else
                {
                    move_list.add_move(encode_move(source_squere, target_squere, QuietMove));
                }
            }
        }
    }
}
