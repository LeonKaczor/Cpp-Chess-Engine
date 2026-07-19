// in this file I define position and bitboards

#include "types.h"

#include "bitboard.h"

#include <iostream>

void print_bitboard(U64 bitboard)

{

    for (int rank = 7; rank >= 0; rank--)

    {

        std::cout << rank + 1 << " ";

        for (int file = 0; file < 8; file++)

        {

            int square = rank * 8 + file;

            if (get_bit(bitboard, square))

            {

                std::cout << "1 ";
            }

            else

            {

                std::cout << ". ";
            }
        }

        std::cout << "\n";
    }

    std::cout << "\n a b c d e f g h\n\n";
}

void board_setup(Position &pos)

{
    pos.side_to_move = white;
    pos.en_passant_squeres = NO_SQ;

    pos.castling_rights = 15; // 1111

    for (int i = 0; i < 12; i++)
    {
        pos.bitboards[i] = 0ULL;
    }
    // white pawns
    on_bit(pos.bitboards[w_pawn], A2);
    on_bit(pos.bitboards[w_pawn], B2);
    on_bit(pos.bitboards[w_pawn], C2);
    on_bit(pos.bitboards[w_pawn], D2);
    on_bit(pos.bitboards[w_pawn], E2);
    on_bit(pos.bitboards[w_pawn], F2);
    on_bit(pos.bitboards[w_pawn], G2);
    on_bit(pos.bitboards[w_pawn], H2);

    // white rooks
    on_bit(pos.bitboards[w_rook], A1);
    on_bit(pos.bitboards[w_rook], H1);

    // white knights
    on_bit(pos.bitboards[w_knight], B1);
    on_bit(pos.bitboards[w_knight], G1);

    // white bishops
    on_bit(pos.bitboards[w_bishop], C1);
    on_bit(pos.bitboards[w_bishop], F1);

    // white queen
    on_bit(pos.bitboards[w_queen], D1);

    // white king
    on_bit(pos.bitboards[w_king], E1);

    // black pawns
    on_bit(pos.bitboards[b_pawn], A7);
    on_bit(pos.bitboards[b_pawn], B7);
    on_bit(pos.bitboards[b_pawn], C7);
    on_bit(pos.bitboards[b_pawn], D7);
    on_bit(pos.bitboards[b_pawn], E7);
    on_bit(pos.bitboards[b_pawn], F7);
    on_bit(pos.bitboards[b_pawn], G7);
    on_bit(pos.bitboards[b_pawn], H7);

    // black rooks
    on_bit(pos.bitboards[b_rook], A8);
    on_bit(pos.bitboards[b_rook], H8);

    // black knights
    on_bit(pos.bitboards[b_knight], B8);
    on_bit(pos.bitboards[b_knight], G8);

    // black bishops
    on_bit(pos.bitboards[b_bishop], C8);
    on_bit(pos.bitboards[b_bishop], F8);

    // black queen
    on_bit(pos.bitboards[b_queen], D8);

    // black king
    on_bit(pos.bitboards[b_king], E8);
}

void update_occupancies(Position &pos)

{

    pos.occupancies[white] = 0ULL;
    pos.occupancies[black] = 0ULL;
    pos.occupancies[both] = 0ULL;

    // white pices

    for (int i = w_pawn; i <= w_king; i++)
    {
        pos.occupancies[white] |= pos.bitboards[i];
    }

    // black pices
    for (int i = b_pawn; i <= b_king; i++)
    {
        pos.occupancies[black] |= pos.bitboards[i];
    }
    pos.occupancies[both] = pos.occupancies[white] | pos.occupancies[black];
}
