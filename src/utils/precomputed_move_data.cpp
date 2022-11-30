//
// Created by Арман Чукин on 30.11.2022.
//

#include <algorithm>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "precomputed_move_data.h"

PrecomputedMoveData* PrecomputedMoveData::singleton_= nullptr;

int PrecomputedMoveData::numRookMovesToReachSquare(int startSquare, int targetSquare) {
    return orthogonalDistance[startSquare][targetSquare];
}

int PrecomputedMoveData::numKingMovesToReachSquare(int startSquare, int targetSquare) {
    return kingDistance[startSquare][targetSquare];
}

PrecomputedMoveData::PrecomputedMoveData() {
    pawnAttacksWhite = std::vector<std::vector<int>>(64);
    pawnAttacksBlack = std::vector<std::vector<int>>(64);
    numSquaresToEdge = std::vector<std::vector<int>>(64);
    knightMoves = std::vector<std::vector<std::byte>>(64);
    kingMoves = std::vector<std::vector<std::byte>>(64);
    rookMoves = std::vector<unsigned long>(64);
    bishopMoves = std::vector<unsigned long>(64);;
    queenMoves = std::vector<unsigned long>(64);

    int allKnightJumps[] = { 15, 17, -17, -15, 10, -6, 6, -10 };
    knightAttackBitboards = std::vector<unsigned long>(64);
    kingAttackBitboards = std::vector<unsigned long>(64);
    pawnAttackBitboards = std::vector<std::vector<unsigned long>>(64);

    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
        int y = squareIndex / 8;
        int x = squareIndex - y * 8;

        int north = 7 - y;
        int south = y;
        int west = x;
        int east = 7 - x;
        numSquaresToEdge[squareIndex] = std::vector<int>(8);
        numSquaresToEdge[squareIndex][0] = north;
        numSquaresToEdge[squareIndex][1] = south;
        numSquaresToEdge[squareIndex][2] = west;
        numSquaresToEdge[squareIndex][3] = east;
        numSquaresToEdge[squareIndex][4] = std::min(north, west);
        numSquaresToEdge[squareIndex][5] = std::min(south, east);
        numSquaresToEdge[squareIndex][6] = std::min(north, east);
        numSquaresToEdge[squareIndex][7] = std::min(south, west);

        std::vector<std::byte> legalKnightJumps;
        unsigned long knightBitboard = 0;
        for (const int knightJumpDelta : allKnightJumps) {
            int knightJumpSquare = squareIndex + knightJumpDelta;

            if (knightJumpSquare >= 0 && knightJumpSquare < 64) {
                int knightSquareY = knightJumpSquare / 8;
                int knightSquareX = knightJumpSquare - knightSquareY * 8;

                int maxCoordMoveDst = std::max(std::abs(x - knightSquareX), std::abs(y - knightSquareY));
                if (maxCoordMoveDst == 2) {
                    legalKnightJumps.push_back(static_cast<const std::byte>(knightJumpSquare));
                    knightBitboard |= 1ul << knightJumpSquare;
                }
            }
        }
        knightMoves[squareIndex] = legalKnightJumps;
        knightAttackBitboards[squareIndex] = knightBitboard;

        std::vector<std::byte> legalKingMoves;
        for (const auto kingMoveDelta : directionOffsets) {
            int kingMoveSquare = squareIndex + kingMoveDelta;
            if (kingMoveSquare >=0 && kingMoveSquare < 64) {
                int kingSquareY = kingMoveSquare / 8;
                int kingSquareX = kingMoveSquare - kingSquareY * 8;

                int maxCoordMoveDst = std::max(std::abs(x - kingSquareX), std::abs(y - kingSquareY));
                if (maxCoordMoveDst == 1) {
                    legalKingMoves.push_back(static_cast<const std::byte>(kingMoveSquare));
                    kingAttackBitboards[squareIndex] |= 1ul << kingMoveSquare;
                }
            }
        }
        kingMoves[squareIndex] = legalKingMoves;

        std::vector<int> pawnCaptureWhite;
        std::vector<int> pawnCaptureBlack;
        pawnAttackBitboards[squareIndex] = std::vector<unsigned long>(2);
        if (x > 0) {
            if (y < 7) {
                pawnCaptureWhite.push_back(squareIndex + 7);
                pawnAttackBitboards[squareIndex][Board::whiteIndex] |= 1ul << (squareIndex + 7);
            }
            if (y > 0) {
                pawnCaptureBlack.push_back(squareIndex - 9);
                pawnAttackBitboards[squareIndex][Board::blackIndex] |= 1ul << (squareIndex - 9);
            }
        }
        if (x < 7) {
            if (y < 7) {
                pawnCaptureWhite.push_back(squareIndex + 9);
                pawnAttackBitboards[squareIndex][Board::whiteIndex] |= 1ul << (squareIndex + 9);
            }
            if (y > 0) {
                pawnCaptureBlack.push_back(squareIndex - 7);
                pawnAttackBitboards[squareIndex][Board::blackIndex] |= 1ul << (squareIndex - 7);
            }
        }
        pawnAttacksWhite[squareIndex] = pawnCaptureWhite;
        pawnAttacksBlack[squareIndex] = pawnCaptureBlack;

        for (int directionIndex = 0; directionIndex < 4; directionIndex++) {
            int currentDirOffset = directionOffsets[directionIndex];
            for (int n = 0; n < numSquaresToEdge[squareIndex][directionIndex]; n++) {
                int targetSquare  = squareIndex + currentDirOffset * (n + 1);
                rookMoves[squareIndex] |= 1ul << targetSquare;
            }
        }

        for (int directionIndex = 0; directionIndex < 8; directionIndex++) {
            int currentDirOffset = directionOffsets[directionIndex];
            for (int n = 0; n < numSquaresToEdge[squareIndex][directionIndex]; n++) {
                int targetSquare  = squareIndex + currentDirOffset * (n + 1);
                bishopMoves[squareIndex] |= 1ul << targetSquare;
            }
        }
        queenMoves[squareIndex] = rookMoves[squareIndex] | bishopMoves[squareIndex];
    }

    directionLookup = std::vector<int>(127);
    for (int i = 0; i < 127; i++) {
        int offset = i - 63;
        int absOffset = std::abs(offset);
        int absDir = 1;
        if (absOffset % 9 == 0) {
            absDir = 9;
        } else if (absOffset % 8 == 0) {
            absDir = 8;
        } else if (absOffset % 7 == 0) {
            absDir = 7;
        }

        directionLookup[i] = absDir * sgn(offset);
    }

    orthogonalDistance = std::vector<std::vector<int>>(64);
    kingDistance = std::vector<std::vector<int>>(64);
    centreManhattanDistance = std::vector<int>(64);

    for (int squareA = 0; squareA < 64; squareA++) {
        sf::Vector2<int> coordA = BoardRepresentation::coordsFromIndex(squareA);
        int fileDstFromCenter = std::max(3 - coordA.y, coordA.y - 4);
        int rankDstFromCenter = std::max(3 - coordA.x, coordA.x - 4);
        centreManhattanDistance[squareA] = fileDstFromCenter + rankDstFromCenter;

        for (int squareB = 0; squareB < 64; squareB++) {
            auto coordB = BoardRepresentation::coordsFromIndex(squareB);
            int rankDistance = std::abs(coordA.x - coordB.x);
            int fileDistance = std::abs(coordA.y - coordB.y);
            orthogonalDistance[squareA][squareB] = fileDistance + rankDistance;
            kingDistance[squareA][squareB] = std::max(fileDistance, rankDistance);
        }
    }
}

PrecomputedMoveData *PrecomputedMoveData::getInstance() {
    if (singleton_ == nullptr) {
        singleton_ = new PrecomputedMoveData();
    }
    return singleton_;
}

template<typename T>
int PrecomputedMoveData::sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
