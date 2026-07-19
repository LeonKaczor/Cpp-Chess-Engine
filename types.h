// in this file I will define enum type for figures, colors and squeres
// this is done to simplify defining board manipulations by using for example white instead of 0 and black instead of 1

#pragma once
#include <cstdint>
typedef uint64_t U64;

enum Color
{
    white, // 0
    black, // 1
    both   // 2
};

enum PieceType
{
    pawn,   // 0
    rook,   // 1
    knight, // 2
    bishop, // 3
    queen,  // 4
    king    // 5
};

enum Piece
{
    w_pawn,
    w_rook,
    w_knight,
    w_bishop,
    w_queen,
    w_king,
    b_pawn,
    b_rook,
    b_knight,
    b_bishop,
    b_queen,
    b_king,
    empty
};

enum Square
{
    A1,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
    NO_SQ
};

// here I create a way to encode moves
// Its not intuitive to pack data do 16 bit as then we have to deal with unpacking all information
// it turns out that by doing that we delegate dealing with data to cache CPU which can
// process the information much faster than RAM

typedef uint16_t Move; // i name data type uint16_t = Move

enum MoveFlag
{
    QuietMove = 0,      // normal move
    DoublePawnPush = 1, // pawn moves 2 files ahead
    KingCastle = 2,     // short castle
    QueenCastle = 3,    // long castle
    Capture = 4,        // normal capture
    EnPassant = 5,      // if enpassant

    PromoteToKnight = 8,
    PromoteToBishop = 9,
    PromoteToRook = 10,
    PromoteToQueen = 11,

    PromoteToKnight_capture = 12,
    PromoteToBishop_capture = 13,
    PromoteToRook_capture = 14,
    PromoteToQueen_capture = 15
};

enum CastlingRights
{
    wk = 1, // white kingside 0001
    wq = 2, // white queenside 0010
    bk = 4, // 0100
    bq = 8  // 1000
};
// function that packs the move into 16bits
inline Move encode_move(int source, int target, int flag)
{
    return (source) | (target << 6) | (flag << 12);
}

// functions that unpack the move: source, target and flag

inline int get_source(Move move)
{
    return move & 0x3F;
}

inline int get_target(Move move)
{
    return (move >> 6) & 0x3F;
}

inline int get_flag(Move move)
{
    return (move >> 12) & 0xF;
}