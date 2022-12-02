//
// Created by Арман Чукин on 30.11.2022.
//

#ifndef CHESS_SDL_PRECOMPUTED_MOVE_DATA_H
#define CHESS_SDL_PRECOMPUTED_MOVE_DATA_H


#include <cstddef>
#include <array>
#include "../board.h"
#include "../board_representation.h"

class PrecomputedMoveData {
public:
    std::array<int8_t, 8> directionOffsets = {8, -8, -1, 1, 7, -7, 9, -9};
    std::vector<std::vector<int>> numSquaresToEdge;
    std::vector<std::vector<uint8_t>> knightMoves;
    std::vector<std::vector<uint8_t>> kingMoves;

    std::array<std::array<uint8_t, 2>, 2> pawnAttackDirections = {{
                                                                          {4, 6},
                                                                          {5, 7}
                                                                  }};

    std::vector<std::vector<int>> pawnAttacksWhite;
    std::vector<std::vector<int>> pawnAttacksBlack;
    std::vector<int> directionLookup;
    std::vector<unsigned long> kingAttackBitboards;
    std::vector<unsigned long> knightAttackBitboards;
    std::vector<std::vector<unsigned long>> pawnAttackBitboards;
    std::vector<unsigned long> rookMoves;
    std::vector<unsigned long> bishopMoves;
    std::vector<unsigned long> queenMoves;

    std::vector<std::vector<int>> orthogonalDistance;
    std::vector<std::vector<int>> kingDistance;
    std::vector<int> centreManhattanDistance;

    int numRookMovesToReachSquare(int startSquare, int targetSquare);
    int numKingMovesToReachSquare(int startSquare, int targetSquare);

    PrecomputedMoveData(PrecomputedMoveData &other) = delete;

    void operator=(const PrecomputedMoveData &) = delete;

    static PrecomputedMoveData *getInstance();

private:
    template <typename T> int sgn(T val);

protected:
    PrecomputedMoveData();

    static PrecomputedMoveData* singleton_;
};


#endif //CHESS_SDL_PRECOMPUTED_MOVE_DATA_H
