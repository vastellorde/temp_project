//
// Created by Арман Чукин on 29.11.2022.
//

#ifndef CHESS_SDL_MOVE_GENERATOR_H
#define CHESS_SDL_MOVE_GENERATOR_H


#include "../board.h"
#include "move.h"
#include "precomputed_move_data.h"
#include "bitboard_util.h"

enum PromotionMode {
    ALL,
    QUEEN_ONLY,
    QUEEN_AND_KNIGHT
};

class MoveGenerator {
public:
    PromotionMode promotionsToGenerate = ALL;

    unsigned long opponentAttackMap;
    unsigned long opponentPawnAttackMap;

    std::vector<Move> generateMoves(Board* board, bool includeQuietMoves = true);
private:
    Board* m_board;
    PrecomputedMoveData* precomputedMoveData = PrecomputedMoveData::getInstance();

    std::vector<Move> moves;
    bool isWhiteToMove;
    int friendlyColor;
    int opponentColor;
    int friendlyKingSquare;
    int friendlyColorIndex;
    int opponentColorIndex;

    bool isCheck;
    bool isDoubleCheck;
    bool pinExistInPosition;
    unsigned long checkRayBitmask;
    unsigned long pinRayBitmask;
    unsigned long opponentKnightAttacks;
    unsigned long opponentAttackMapNoPawns;
    unsigned long opponentSlidingAttackMap;

    bool genQuiets;

    void init();
    void calculateAttackData();
    void generateKingMoves();
    void generateSlidingMoves();
    void generateKnightMoves();
    void generatePawnMoves();
    void getSlidingAttackMap();
    void updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex);
    void generateSlidingPieceMove(int startSquare, int startDirIndex, int endDirIndex);
    void makePromotionMove(int startSquare, int toSquare);

    [[nodiscard]] bool squareIsInCheckRay(int square) const;
    [[nodiscard]] bool squareIsAttacked(int square) const;
    bool hasKingsideCastleRight();
    bool hasQueensideCastleRight();

    [[nodiscard]] bool isPinned(int square) const;
    bool isMovingAlongRay(int rayDir, int startSquare, int targetSquare);
    bool inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare);
    bool squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare);
};


#endif //CHESS_SDL_MOVE_GENERATOR_H
