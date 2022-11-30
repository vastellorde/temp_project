//
// Created by Арман Чукин on 27.11.2022.
//

#include "Piece.h"

int Piece::pieceType(int piece) {
    return piece & typeMask;
}

bool Piece::isWhite(int piece) {
    return (piece & colorMask) == White;
}

bool Piece::isColor(int piece, int color) {
    return (piece & colorMask) == color;
}

bool Piece::isRookOrQueen(int piece) {
    return (piece & 0b110) == 0b110;
}

bool Piece::isBishopOrQueen(int piece) {
    return (piece & 0b101) == 0b101;
}
