#include "types.h"
#include "bitboard.h"
#include "attacks.h"

bool make_move(Position &pos, Move move)
{

    Position copy = pos;

    int source = get_source(move);
    int target = get_target(move);
    int flag = get_flag(move);
    int side = pos.side_to_move;

    int piece = empty;
    for (int i = 0; i < 12; i++)
    {
        if (get_bit(pos.bitboards[i], source))
        {
            piece = i;
            break;
        }
    }

    bool is_capture = get_bit(pos.occupancies[(side == white) ? black : white], target);

    clear_bit(pos.bitboards[piece], source);
    on_bit(pos.bitboards[piece], target);

    if (is_capture)
    {
        int enemy_start = (side == white) ? b_pawn : w_pawn;
        int enemy_end = (side == white) ? b_king : w_king;

        for (int i = enemy_start; i <= enemy_end; i++)
        {
            if (get_bit(pos.bitboards[i], target))
            {
                clear_bit(pos.bitboards[i], target);
                break;
            }
        }
    }

    if (flag >= PromoteToKnight)
    {
        clear_bit(pos.bitboards[piece], target);

        int type = flag & 3; // bitmask ...000011: 0 = knight, 1 = bishop, 2 = rook, 3 = queen

        if (side == white)
        {
            if (type == 0)
                on_bit(pos.bitboards[w_knight], target);
            else if (type == 1)
                on_bit(pos.bitboards[w_bishop], target);
            else if (type == 2)
                on_bit(pos.bitboards[w_rook], target);
            else
                on_bit(pos.bitboards[w_queen], target);
        }
        else
        {
            if (type == 0)
                on_bit(pos.bitboards[b_knight], target);
            else if (type == 1)
                on_bit(pos.bitboards[b_bishop], target);
            else if (type == 2)
                on_bit(pos.bitboards[b_rook], target);
            else
                on_bit(pos.bitboards[b_queen], target);
        }
    }

    // en passant
    if (flag == EnPassant)
    {
        if (side == white)
        {
            clear_bit(pos.bitboards[b_pawn], target - 8); // capture pwan bellow
        }
        else
        {
            clear_bit(pos.bitboards[w_pawn], target + 8); // above
        }
    }

    // setting en passant to no sq
    pos.en_passant_squeres = NO_SQ;
    if (flag == DoublePawnPush)
    {
        pos.en_passant_squeres = (side == white) ? target - 8 : target + 8;
        //  warunek ? if true : if false
    }

    // castling instructions
    if (flag == KingCastle)
    {
        if (side == white)
        {
            clear_bit(pos.bitboards[w_rook], H1);
            on_bit(pos.bitboards[w_rook], F1);
        }
        else
        {
            clear_bit(pos.bitboards[b_rook], H8);
            on_bit(pos.bitboards[b_rook], F8);
        }
    }
    else if (flag == QueenCastle)
    {
        if (side == white)
        {
            clear_bit(pos.bitboards[w_rook], A1);
            on_bit(pos.bitboards[w_rook], D1);
        }
        else
        {
            clear_bit(pos.bitboards[b_rook], A8);
            on_bit(pos.bitboards[b_rook], D8);
        }
    }

    // if king has been move or taken we remove right for castling
    if (source == E1 || target == E1)
    {
        pos.castling_rights &= ~(wk | wq);
    }
    if (source == E8 || target == E8)
    {
        pos.castling_rights &= ~(bk | bq);
    }
    // same for moving rooks
    if (source == A1 || target == A1)
    {
        pos.castling_rights &= ~wq;
    }
    if (source == H1 || target == H1)
    {
        pos.castling_rights &= ~wk;
    }
    if (source == A8 || target == A8)
    {
        pos.castling_rights &= ~bq;
    }
    if (source == H8 || target == H8)
    {
        pos.castling_rights &= ~bk;
    }

    // counting moves without capture or movig pawns for drwa
    if (piece == w_pawn || piece == b_pawn || is_capture || flag == EnPassant)
    {
        pos.halfmoves = 0;
    }
    else
    {
        pos.halfmoves++;
    }

    // when black figures move we add one full move to the meter
    if (side == black)
    {
        pos.fullmove_number++;
    }

    // changine who moves
    pos.side_to_move ^= 1;
    update_occupancies(pos);

    // checking if king is attacked after the move
    int king_piece = (side == white) ? w_king : b_king;
    int king_sq = __builtin_ctzll(pos.bitboards[king_piece]);

    if (is_squere_attacked(king_sq, pos.side_to_move, pos))
    {
        pos = copy; // not legal move position turns back to the copy
        return false;
    }

    return true; // legal move
}