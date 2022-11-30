//
// Created by Арман Чукин on 27.11.2022.
//

#ifndef CHESS_SDL_PIECE_H
#define CHESS_SDL_PIECE_H


class Piece {
public:
    const static int None = 0;
    const static int King = 1;
    const static int Pawn = 2;
    const static int Knight = 3;
    const static int Bishop = 5;
    const static int Rook = 6;
    const static int Queen = 7;

    const static int White = 8;
    const static int Black = 16;

    const static int typeMask = 0b00111;
    const static int blackMask = 0b10000;
    const static int whiteMask = 0b01000;
    const static int colorMask = whiteMask | blackMask;

    static int pieceType(int piece);
    static bool isWhite(int piece);
    static bool isColor(int piece, int color);
    static bool isBishopOrQueen(int piece);
    static bool isRookOrQueen(int piece);
};


#endif //CHESS_SDL_PIECE_H
