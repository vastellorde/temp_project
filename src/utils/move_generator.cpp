//
// Created by Арман Чукин on 29.11.2022.
//

#include "move_generator.h"

void MoveGenerator::init() {
    moves = std::vector<Move>();
    isCheck = false;
    isDoubleCheck = false;
    pinExistInPosition = false;
    checkRayBitmask = 0;
    pinRayBitmask = 0;

    isWhiteToMove = m_board->colorToMove == Piece::White;
    friendlyColor = m_board->colorToMove;
    opponentColor = m_board->opponentColor;
    friendlyKingSquare = m_board->kingSquare[m_board->colorToMoveIndex];
    friendlyColorIndex = m_board->whiteToMove ? Board::whiteIndex : Board::blackIndex;
    opponentColorIndex = 1 - friendlyColorIndex;
}

std::vector<Move> MoveGenerator::generateMoves(Board* board, bool includeQuietMoves) {
    m_board = board;
    genQuiets = includeQuietMoves;
    init();

    calculateAttackData();
    generateKingMoves();

    if(isDoubleCheck) {
        return moves;
    }

    generateSlidingMoves();
    generateKnightMoves();
    generatePawnMoves();

    return moves;
}

void MoveGenerator::calculateAttackData() {
    getSlidingAttackMap();
    int startDirIndex = 0;
    int endDirIndex = 0;

    if (m_board->queens[opponentColorIndex].count() == 0) {
        startDirIndex = (m_board->rooks[opponentColorIndex].count() > 0) ? 0 : 4;
        endDirIndex = (m_board->bishop[opponentColorIndex].count() > 0) ? 8 : 4;
    }

    for (int dir = startDirIndex; dir < endDirIndex; dir++) {
        bool isDiagonal = dir > 3;
        int n = precomputedMoveData->numSquaresToEdge[friendlyKingSquare][dir];
        int directionOffset = precomputedMoveData->directionOffsets[dir];
        bool isFriendlyPieceAlongRay = false;
        unsigned long rayMask = 0;

        for (int i = 0; i < n; i++) {
            int squareIdx = friendlyKingSquare + directionOffset * (i + 1);
            rayMask |= 1ul << squareIdx;
            int piece = m_board->square[squareIdx];

            if (piece != Piece::None) {
                if (Piece::isColor(piece, friendlyColorIndex)) {
                    if (!isFriendlyPieceAlongRay) {
                        isFriendlyPieceAlongRay = true;
                    } else {
                        break;
                    }
                } else {
                    int pieceType = Piece::pieceType(piece);
                    if (isDiagonal && Piece::isBishopOrQueen(pieceType) || !isDiagonal && Piece::isRookOrQueen(pieceType)) {
                        if (isFriendlyPieceAlongRay) {
                            pinExistInPosition = true;
                            pinRayBitmask |= rayMask;
                        } else {
                            checkRayBitmask |= rayMask;
                            isDoubleCheck = isCheck;
                            isCheck = true;
                        }
                        break;
                    } else {
                        break;
                    }
                }
            }
        }

        if (isDoubleCheck) {
            break;
        }
    }

    PieceList opponentKnights = m_board->knights[opponentColorIndex];
    opponentKnightAttacks = 0;
    bool isKnightCheck = false;

    for (int knightIndex = 0; knightIndex < opponentKnights.count(); knightIndex++) {
        int startSquare = opponentKnights[knightIndex];
        opponentKnightAttacks |= precomputedMoveData->knightAttackBitboards[startSquare];

        if (!isKnightCheck && BitboardUtil::containsSquare(opponentKnightAttacks, friendlyKingSquare)) {
            isKnightCheck = true;
            isDoubleCheck = isCheck;
            isCheck = true;
            checkRayBitmask |= 1ul << startSquare;
        }
    }

    PieceList opponentPawns = m_board->pawns[opponentColorIndex];
    opponentPawnAttackMap = 0;
    bool isPawnCheck = false;

    for (int pawnIndex = 0; pawnIndex < opponentPawns.count(); pawnIndex++) {
        int pawnSquare = opponentPawns[pawnIndex];
        unsigned long pawnAttacks = precomputedMoveData->pawnAttackBitboards[pawnSquare][friendlyKingSquare];
        opponentPawnAttackMap |= pawnAttacks;

        if (!isPawnCheck && BitboardUtil::containsSquare(pawnAttacks, friendlyKingSquare)) {
            isPawnCheck = true;
            isDoubleCheck = isCheck;
            isCheck = true;
            checkRayBitmask |= 1ul << pawnSquare;
        }
    }

    int enemyKingSquare = m_board->kingSquare[opponentColorIndex];

    opponentAttackMapNoPawns = opponentSlidingAttackMap | opponentKnightAttacks | precomputedMoveData->kingAttackBitboards[enemyKingSquare];
    opponentAttackMap = opponentAttackMapNoPawns | opponentPawnAttackMap;
}

void MoveGenerator::generateKingMoves() {
    for (auto & i : precomputedMoveData->kingMoves[friendlyKingSquare]) {
        int targetSquare = static_cast<int>(i);
        int pieceOnTargetSquare = m_board->square[targetSquare];

        if (Piece::isColor(pieceOnTargetSquare, friendlyColor)) {
            continue;
        }

        bool isCapture = Piece::isColor(pieceOnTargetSquare, opponentColor);

        if (isCapture) {
            if (!genQuiets || squareIsInCheckRay(targetSquare)) {
                continue;
            }
        }

        if (!squareIsAttacked(targetSquare)) {
            moves.emplace_back(friendlyKingSquare, targetSquare);

            if (!isCheck && !isCapture) {
                if ((targetSquare == BoardRepresentation::f1 || targetSquare == BoardRepresentation::f8) && hasKingsideCastleRight()) {
                    int castleKingSquare = targetSquare + 1;
                    if (m_board->square[castleKingSquare] == Piece::None) {
                        if (!squareIsAttacked(castleKingSquare)) {
                            moves.emplace_back(friendlyKingSquare, castleKingSquare, Flag::castling);
                        }
                    }
                } else if ((targetSquare == BoardRepresentation::d1 || targetSquare == BoardRepresentation::d8) && hasKingsideCastleRight()) {
                    int castleQueenSquare = targetSquare - 1;
                    if (m_board->square[castleQueenSquare] == Piece::None && m_board->square[castleQueenSquare - 1] == Piece::None) {
                        if (!squareIsAttacked(castleQueenSquare)) {
                            moves.emplace_back(friendlyKingSquare, castleQueenSquare, Flag::castling);
                        }
                    }
                }
            }
        }
    }
}

void MoveGenerator::generateSlidingMoves() {
    PieceList rooks = m_board->rooks[friendlyColorIndex];
    for (int i = 0; i < rooks.count(); i++) {
        generateSlidingPieceMove(rooks[i], 0, 4);
    }

    PieceList queens = m_board->queens[friendlyColorIndex];
    for (int i = 0; i < queens.count(); i++) {
        generateSlidingPieceMove(queens[i], 0, 8);
    }

    PieceList bishops = m_board->bishop[friendlyColorIndex];
    for (int i = 0; i < bishops.count(); i++) {
        generateSlidingPieceMove(bishops[i], 4, 8);
    }
}

void MoveGenerator::generateKnightMoves() {
    PieceList myKnights = m_board->knights[friendlyColorIndex];

    for (int i = 0; i < myKnights.count(); i++) {
        int startSquare = myKnights[i];

        if (isPinned(startSquare)) {
            continue;
        }

        for (int knightMoveIdx = 0; knightMoveIdx < precomputedMoveData->knightMoves[startSquare].size(); knightMoveIdx++) {
            int targetSquare = static_cast<int>(precomputedMoveData->knightMoves[startSquare][knightMoveIdx]);
            int targetSquarePiece = m_board->square[targetSquare];
            bool isCapture = Piece::isColor(targetSquarePiece, opponentColor);

            if (genQuiets || isCapture) {
                continue;
            }
            moves.emplace_back(startSquare, targetSquare);
        }
    }
}

void MoveGenerator::generatePawnMoves() {
    PieceList myPawns = m_board->pawns[friendlyColorIndex];

    int pawnOffset = friendlyColor == Piece::White ? 8 : -8;
    int startRank = m_board->whiteToMove ? 1 : 6;
    int finalRankBeforePromotion = m_board->whiteToMove ? 6 : 1;

    int enPassantFile = ((static_cast<int>(m_board->currentGameState) >> 4) & 15) - 1;
    int enPassantSquare = -1;

    if (enPassantFile != -1) {
        enPassantSquare = 8 * ((m_board->whiteToMove) ? 5 : 2) + enPassantFile;
    }

    for (int i = 0; i < myPawns.count(); i++) {
        int startSquare = myPawns[i];
        int rank = BoardRepresentation::rankIndex(startSquare);
        bool oneStepFromPromotion = rank == finalRankBeforePromotion;

        if (genQuiets) {
            int squareOneForward = startSquare + pawnOffset;

            if (m_board->square[squareOneForward] == Piece::None) {
                if (!isPinned(squareOneForward) || isMovingAlongRay(pawnOffset, startSquare, friendlyKingSquare)) {
                    if (!isCheck || squareIsInCheckRay(squareOneForward)) {
                        if (oneStepFromPromotion) {
                            makePromotionMove(startSquare, squareOneForward);
                        } else {
                            moves.emplace_back(startSquare, squareOneForward);
                        }
                    }

                    if (rank == startRank) {
                        int squareTwoForward = squareOneForward + pawnOffset;
                        if (m_board->square[squareTwoForward] == Piece::None) {
                            if (!isCheck || squareIsInCheckRay(squareTwoForward)) {
                                moves.emplace_back(startSquare, squareTwoForward, Flag::pawnTwoForward);
                            }
                        }
                    }
                }
            }
        }

        for(auto & pawnAttackDirection : precomputedMoveData->pawnAttackDirections) {
            if (precomputedMoveData->numSquaresToEdge[startSquare][reinterpret_cast<unsigned long>(pawnAttackDirection)] > 0) {
                int pawnCaptureDir = precomputedMoveData->directionOffsets[reinterpret_cast<unsigned long>(pawnAttackDirection)];
                int targetSquare = startSquare + pawnCaptureDir;
                int targetPiece = m_board->square[targetSquare];

                if (!isPinned(startSquare) && !isMovingAlongRay(pawnCaptureDir, friendlyKingSquare, startSquare)) {
                    continue;
                }

                if (Piece::isColor(targetPiece, opponentColor)) {
                    if (isCheck && !squareIsInCheckRay(targetSquare)) {
                        continue;
                    }
                    if (oneStepFromPromotion) {
                        makePromotionMove(startSquare, targetSquare);
                    } else {
                        moves.emplace_back(startSquare, targetSquare);
                    }
                }

                if (targetSquare == enPassantSquare) {
                    int epCapturePawnSquare = targetSquare + (m_board->whiteToMove ? -8 : 8);
                    if (!inCheckAfterEnPassant(startSquare, targetSquare, epCapturePawnSquare)) {
                        moves.emplace_back(startSquare, targetSquare, Flag::enPassantCapture);
                    }
                }
            }
        }
    }
}

void MoveGenerator::getSlidingAttackMap() {
    opponentSlidingAttackMap = 0;

    PieceList enemyRooks = m_board->rooks[opponentColorIndex];
    for (int i = 0; i < enemyRooks.count(); i++) {
        updateSlidingAttackPiece(enemyRooks[i], 0, 4);
    }

    PieceList enemyQueens = m_board->queens[opponentColorIndex];
    for (int i = 0; i < enemyQueens.count(); i++) {
        updateSlidingAttackPiece(enemyQueens[i], 0, 8);
    }

    PieceList enemyBishops = m_board->bishop[opponentColorIndex];
    for (int i = 0; i < enemyBishops.count(); i++) {
        updateSlidingAttackPiece(enemyBishops[i], 4, 8);
    }
}

void MoveGenerator::updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex) {
    for (int directionIdx = startDirIndex; directionIdx < endDirIndex; directionIdx++) {
        int currDirOffset = precomputedMoveData->directionOffsets[directionIdx];
        for (int n = 0; n < precomputedMoveData->numSquaresToEdge[startSquare][directionIdx]; n++) {
            int targetSquare = startSquare + currDirOffset * (n + 1);
            int targetSquarePiece = m_board->square[targetSquare];
            opponentSlidingAttackMap |= 1ul << targetSquare;
            if (targetSquare != friendlyKingSquare) {
                if (targetSquarePiece != friendlyKingSquare) {
                    break;
                }
            }
        }
    }
}

bool MoveGenerator::squareIsInCheckRay(int square) const {
    return isCheck && ((checkRayBitmask >> square) & 1) != 0;
}

bool MoveGenerator::squareIsAttacked(int square) const {
    return BitboardUtil::containsSquare(opponentAttackMap, square);
}

bool MoveGenerator::hasKingsideCastleRight() {
    int mask = (m_board->whiteToMove) ? 1 : 4;
    return (m_board->currentGameState & mask) != 0;
}

void MoveGenerator::generateSlidingPieceMove(int startSquare, int startDirIndex, int endDirIndex) {
    bool isPinned = this->isPinned(startSquare);

    if (isCheck && isPinned) {
        return;
    }

    for (int directionIdx = startDirIndex; directionIdx < endDirIndex; directionIdx++) {
        int currDirOffset = precomputedMoveData->directionOffsets[directionIdx];

        if (isPinned && isMovingAlongRay(currDirOffset, friendlyKingSquare, startSquare)) {
            continue;
        }

        for (int n = 0; n < precomputedMoveData->numSquaresToEdge[startSquare][directionIdx]; n++) {
            int targetSquare = startSquare + currDirOffset * (n + 1);
            int targetSquarePiece = m_board->square[targetSquare];

            if (Piece::isColor(targetSquarePiece, friendlyColor)) {
                break;
            }

            bool isCapture = targetSquarePiece != Piece::None;

            bool movePreventsCheck = squareIsInCheckRay(targetSquare);
            if (movePreventsCheck || !isCheck) {
                if (genQuiets || isCapture) {
                    moves.emplace_back(startSquare, targetSquare);
                }
            }

            if (isCapture || movePreventsCheck) {
                break;
            }
        }
    }
}

bool MoveGenerator::isPinned(int square) const {
    return pinExistInPosition && ((pinRayBitmask >> square) & 1) !=0;
}

bool MoveGenerator::isMovingAlongRay(int rayDir, int startSquare, int targetSquare) {
    int moveDir = precomputedMoveData->directionLookup[targetSquare - startSquare + 63];
    return (rayDir == moveDir || -rayDir == moveDir);
}

void MoveGenerator::makePromotionMove(int startSquare, int toSquare) {
    moves.emplace_back(startSquare, toSquare, Flag::promoteToQueen);

    if (promotionsToGenerate == ALL) {
        moves.emplace_back(startSquare, toSquare, Flag::promoteToKnight);
        moves.emplace_back(startSquare, toSquare, Flag::promoteToRook);
        moves.emplace_back(startSquare, toSquare, Flag::promoteToBishop);
    } else if (promotionsToGenerate == QUEEN_AND_KNIGHT) {
        moves.emplace_back(startSquare, toSquare, Flag::promoteToKnight);
    }
}

bool MoveGenerator::inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare) {
    m_board->square[targetSquare] = m_board->square[startSquare];
    m_board->square[startSquare] = Piece::None;
    m_board->square[epCapturedPawnSquare] = Piece::None;

    bool inCheckAfterEpCapture = false;
    if (squareAttackedAfterEPCapture(epCapturedPawnSquare, startSquare)) {
        inCheckAfterEpCapture = true;
    }
    m_board->square[targetSquare] = Piece::None;
    m_board->square[startSquare] = Piece::Pawn | friendlyColor;
    m_board->square[epCapturedPawnSquare] = Piece::Pawn | opponentColor;
    return inCheckAfterEpCapture;
}

bool MoveGenerator::squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare) {
    if (BitboardUtil::containsSquare(opponentAttackMap, friendlyKingSquare)) {
        return  true;
    }

    int dirIndex = epCaptureSquare < friendlyKingSquare ? 2 : 3;
    for (int i = 0; i < precomputedMoveData->numSquaresToEdge[friendlyKingSquare][dirIndex]; i ++) {
        int squareIdx = friendlyKingSquare + precomputedMoveData->directionOffsets[dirIndex] * (i + 1);
        int piece = m_board->square[squareIdx];

        if (piece != Piece::None) {
            if (Piece::isColor(piece, friendlyColor)) {
                break;
            } else {
                if (Piece::isRookOrQueen(piece)) {
                    return true;
                } else {
                    break;
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        if (precomputedMoveData->numSquaresToEdge[friendlyKingSquare][static_cast<int>(precomputedMoveData->pawnAttackDirections[friendlyColorIndex][i])] > 0) {
            int piece = m_board->square[friendlyKingSquare + precomputedMoveData->directionOffsets[static_cast<int>(precomputedMoveData->pawnAttackDirections[friendlyColorIndex][i])]];
            if (piece == (Piece::Pawn | opponentColor)) {
                return  true;
            }
        }
    }

    return false;
}
