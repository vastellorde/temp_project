//
// Created by Арман Чукин on 29.11.2022.
//

#ifndef CHESS_SDL_MOVE_H
#define CHESS_SDL_MOVE_H

#include <string>
#include "../Piece.h"
#include "../board_representation.h"

struct Flag {
public:
    static const int none = 0;
    static const int enPassantCapture = 1;
    static const int castling = 2;
    static const int promoteToQueen = 3;
    static const int promoteToKnight = 4;
    static const int promoteToRook = 5;
    static const int promoteToBishop = 6;
    static const int pawnTwoForward = 7;
};

struct Move {
public:
    explicit Move(unsigned short moveValueEx) {
        moveValue = moveValueEx;
    }

    Move(int startSquare, int targetSquare) {
        moveValue = startSquare | targetSquare << 6;
    }

    Move(int startSquare, int targetSquare, int flag) {
        moveValue = startSquare | targetSquare << 6 | flag << 12;
    }

    [[nodiscard]] int getStartSquare() const {
        return moveValue & startSquareMask;
    }

    [[nodiscard]] int getTargetSquare() const {
        return (moveValue & targetSquareMask) >> 6;
    }

    [[nodiscard]] bool isPromotion() const {
        int flag = getMoveFlag();
        return flag == Flag::promoteToQueen || flag == Flag::promoteToRook || flag == Flag::promoteToKnight || flag == Flag::promoteToBishop;
    }

    [[nodiscard]] int getMoveFlag() const {
        return moveValue >> 12;
    };

    [[nodiscard]] int getPromotionPieceType() const {
        switch (getMoveFlag()) {
            case Flag::promoteToRook:
                return Piece::Rook;
            case Flag::promoteToKnight:
                return Piece::Knight;
            case Flag::promoteToBishop:
                return Piece::Bishop;
            case Flag::promoteToQueen:
                return Piece::Queen;
            default:
                return Piece::None;
        }
    }

    static Move invalidMove() {
        return Move(0);
    }

    static bool sameMove(Move a, Move b) {
        return a.moveValue == b.moveValue;
    }

    [[nodiscard]] unsigned short getValue() const {
        return moveValue;
    }

    [[nodiscard]] bool isInvalid() const {
        return moveValue == 0;
    }

    [[nodiscard]] std::string getName() const {
        return BoardRepresentation::squareNameFromIndex(getStartSquare()) + "-" + BoardRepresentation::squareNameFromIndex(getTargetSquare());
    }

private:
    unsigned short moveValue;
    const unsigned short startSquareMask = 0b0000000000111111;
    const unsigned short targetSquareMask = 0b0000111111000000;
    const unsigned short flagMask = 0b1111000000000000;
};
#endif //CHESS_SDL_MOVE_H
