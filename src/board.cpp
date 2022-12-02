//
// Created by Арман Чукин on 27.11.2022.
//

#include "board.h"
#include "utils/fen_util.h"

PieceList Board::getPieceList(int pieceType, int color) {
    return allPieces[color * 8 + pieceType];
}

void Board::init() {
    knights = std::vector<PieceList>{
            PieceList(10),
            PieceList(10)
    };
    bishop = std::vector<PieceList>{
        PieceList(10),
        PieceList(10)
    };
    pawns = std::vector<PieceList>{
            PieceList(8),
            PieceList(8)
    };
    rooks = std::vector<PieceList>{
            PieceList(10),
            PieceList(10)
    };
    queens = std::vector<PieceList>{
            PieceList(9),
            PieceList(9)
    };

    PieceList emptyList(0);

    allPieces = std::vector<PieceList>{
        emptyList,
        emptyList,
        pawns[whiteIndex],
        knights[whiteIndex],
        emptyList,
        bishop[whiteIndex],
        rooks[whiteIndex],
        queens[whiteIndex],
        emptyList,
        emptyList,
        pawns[blackIndex],
        knights[blackIndex],
        emptyList,
        bishop[blackIndex],
        rooks[blackIndex],
        queens[blackIndex]
    };
}

void Board::LoadPosition() {
    init();
    PositionInfo loadedPosition = FenUtil().PositionFromFen();

    for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
        int piece = loadedPosition.squares[squareIndex];
        square[squareIndex] = piece;

        if (piece != Piece::None) {
            int pieceType = Piece::pieceType(piece);
            int pieceColorIndex = Piece::isWhite(piece) ? whiteIndex : blackIndex;

            switch (pieceType) {
                case Piece::Queen:
                    queens[pieceColorIndex].addPieceAtSquare(squareIndex);
                    break;
                case Piece::Rook:
                    rooks[pieceColorIndex].addPieceAtSquare(squareIndex);
                    break;
                case Piece::Bishop:
                    bishop[pieceColorIndex].addPieceAtSquare(squareIndex);
                    break;
                case Piece::Knight:
                    knights[pieceColorIndex].addPieceAtSquare(squareIndex);
                    break;
                case Piece::Pawn:
                    pawns[pieceColorIndex].addPieceAtSquare(squareIndex);
                    break;
                case Piece::King:
                    kingSquare[pieceColorIndex] = squareIndex;
                    break;
                default:
                    break;
            }
        }
    }
    whiteToMove = loadedPosition.whiteToMove;
    colorToMove = whiteToMove ? Piece::White : Piece::Black;
    opponentColor = whiteToMove ? Piece::Black : Piece::White;
    colorToMoveIndex = whiteToMove ? 0 : 1;

    int whiteCastle = ((loadedPosition.whiteCastleKingSide) ? 1 << 0 : 0) | ((loadedPosition.whiteCastleQueenSide) ? 1 << 1 : 0);
    int blackCastle = ((loadedPosition.blackCastleKingSide) ? 1 << 2 : 0) | ((loadedPosition.blackCastleQueenSide) ? 1 << 3 : 0);
    int epsState = loadedPosition.epFile << 4;
    int initialGameState = whiteCastle | blackCastle | epsState;
    gameStateHistory.push(initialGameState);
    currentGameState = initialGameState;
    plyCount = loadedPosition.plyCount;
}

void Board::makeMove(Move move, bool inSearch) {
    unsigned int const originalCastleState = currentGameState & 15;
    unsigned int newCastleState = originalCastleState;
    currentGameState = 0;

    int opponentColorIndex = 1 - colorToMoveIndex;
    int moveFrom = move.getStartSquare();
    int moveTo = move.getTargetSquare();

    int capturedPieceType = Piece::pieceType(square[moveTo]);
    int movePiece = square[moveFrom];
    int movePieceType = Piece::pieceType(movePiece);

    int moveFlag = move.getMoveFlag();

    bool isPromotion = move.isPromotion();
    bool isEnPassant = moveFlag == Flag::enPassantCapture;

    currentGameState |= capturedPieceType << 8;

    if (capturedPieceType != 0 && !isEnPassant) {
        getPieceList(capturedPieceType, opponentColorIndex).removePieceAtSquare(moveTo);
    }

    if (movePieceType == Piece::King) {
        kingSquare[colorToMoveIndex] = moveTo;
        newCastleState &= whiteToMove ? whiteCastleMask : blackCastleMask;
    } else {
        getPieceList(movePieceType, colorToMoveIndex).movePiece(moveFrom, moveTo);
    }

    int pieceOnTargetSquare = movePiece;

    if (isPromotion) {
        int promoteType = 0;
        switch (moveFlag) {
            case Flag::promoteToQueen:
                promoteType = Piece::Queen;
                queens[colorToMoveIndex].addPieceAtSquare(moveTo);
                break;
            case Flag::promoteToRook:
                promoteType = Piece::Rook;
                rooks[colorToMoveIndex].addPieceAtSquare(moveTo);
                break;
            case Flag::promoteToBishop:
                promoteType = Piece::Bishop;
                bishop[colorToMoveIndex].addPieceAtSquare(moveTo);
                break;
            case Flag::promoteToKnight:
                promoteType = Piece::Knight;
                knights[colorToMoveIndex].addPieceAtSquare(moveTo);
                break;
            default:
                break;
        }
        pieceOnTargetSquare = promoteType | colorToMove;
        pawns[colorToMoveIndex].removePieceAtSquare(moveTo);
    } else {
        if (moveFlag == Flag::enPassantCapture) {
            int epPawnSquare = moveTo + (colorToMove == Piece::White ? -8 : 8);
            currentGameState |= square[epPawnSquare] << 8;
            square[epPawnSquare] = 0;
            pawns[opponentColorIndex].removePieceAtSquare(epPawnSquare);
        } else if (moveFlag == Flag::castling) {
            bool kingside = moveTo == BoardRepresentation::g1 || moveTo == BoardRepresentation::g8;
            int castlingRookFromIndex = kingside ? moveTo + 1 : moveTo - 2;
            int castlingRookToIndex = kingside ? moveTo - 1 : moveTo + 1;

            square[castlingRookFromIndex] = Piece::None;
            square[castlingRookToIndex] = Piece::Rook | colorToMove;
            rooks[colorToMoveIndex].movePiece(castlingRookFromIndex, castlingRookToIndex);
        }
    }

    square[moveTo] = pieceOnTargetSquare;
    square[moveFrom] = 0;

    if (moveFlag == Flag::pawnTwoForward) {
        int file = BoardRepresentation::fileIndex(moveFrom) + 1;
        currentGameState |= file << 4;
    }

    if (originalCastleState != 0) {
        if (moveTo == BoardRepresentation::h1 || moveFrom == BoardRepresentation::h1) {
            newCastleState &= whiteCastleKingsideMask;
        } else if (moveTo == BoardRepresentation::a1 || moveFrom == BoardRepresentation::a1) {
            newCastleState &= whiteCastleQeensideMask;
        }
        if (moveTo == BoardRepresentation::h8 || moveFrom == BoardRepresentation::h8) {
            newCastleState &= blackCastleKingsideMask;
        } else if (moveTo == BoardRepresentation::a8 || moveFrom == BoardRepresentation::a8) {
            newCastleState &= blackCastleQeensideMask;
        }
    }

    currentGameState |= newCastleState;
    currentGameState |= fiftyMoveCounter << 14;
    gameStateHistory.push(currentGameState);

    whiteToMove = !whiteToMove;
    colorToMove = whiteToMove ? Piece::White : Piece::Black;
    opponentColor = whiteToMove ? Piece::Black : Piece::White;
    colorToMoveIndex = 1 - colorToMoveIndex;
    plyCount++;
    fiftyMoveCounter++;
}

void Board::unmakeMove(Move move, bool inSearch) {
    int opponentColorIdx = colorToMoveIndex;
    bool undoingWhiteMove = opponentColor == Piece::White;
    colorToMove = opponentColor;
    opponentColor = undoingWhiteMove ? Piece::Black : Piece::White;
    colorToMoveIndex = 1 - colorToMoveIndex;
    whiteToMove = !whiteToMove;

    unsigned int originalCastleState = currentGameState & 0b1111;

    int capturedPieceType = static_cast<int>(currentGameState >> 8) & 63;
    int capturedPiece = capturedPieceType == 0 ? 0 : capturedPieceType | opponentColor;

    int movedFrom = move.getStartSquare();
    int moveTo = move.getTargetSquare();
    int moveFlag = move.getMoveFlag();
    bool isEnPassant = moveFlag == Flag::enPassantCapture;
    bool isPromotion = move.isPromotion();

    int toSquarePieceType = Piece::pieceType(square[moveTo]);
    int movedPieceType = isPromotion ? Piece::Pawn : toSquarePieceType;

    if (capturedPieceType != 0 && !isEnPassant) {
        getPieceList(capturedPieceType, opponentColorIdx).addPieceAtSquare(moveTo);
    }

    if (movedPieceType != Piece::King) {
        kingSquare[colorToMoveIndex] = movedFrom;
    } else if (!isPromotion) {
        getPieceList(movedPieceType, colorToMoveIndex).movePiece(moveTo, movedFrom);
    }

    square[movedFrom] = movedPieceType | colorToMove;
    square[moveTo] = capturedPiece;

    if (isPromotion) {
        pawns[colorToMoveIndex].addPieceAtSquare(movedFrom);
        switch (moveFlag) {
            case Flag::promoteToQueen:
                queens[colorToMoveIndex].removePieceAtSquare(moveTo);
                break;
            case Flag::promoteToKnight:
                knights[colorToMoveIndex].removePieceAtSquare(moveTo);
                break;
            case Flag::promoteToRook:
                rooks[colorToMoveIndex].removePieceAtSquare(moveTo);
                break;
            case Flag::promoteToBishop:
                bishop[colorToMoveIndex].removePieceAtSquare(moveTo);
                break;
            default:
                break;
        }
    } else if (isEnPassant) {
        int epIdx = moveTo + (colorToMove == Piece::White ? -8 : 8);
        square[moveTo] = 0;
        square[epIdx] = capturedPiece;
        pawns[opponentColorIdx].addPieceAtSquare(epIdx);
    } else if (moveFlag == Flag::castling) {
        bool kingside = moveTo == 6 || moveTo == 62;
        int castlingRookFromIdx = kingside ? moveTo + 1 : moveTo - 2;
        int castlingRookToIdx = kingside ? moveTo - 1 : moveTo + 1;
        square[castlingRookToIdx] = 0;
        square[castlingRookFromIdx] = Piece::Rook | colorToMove;

        rooks[colorToMoveIndex].movePiece(castlingRookToIdx, castlingRookFromIdx);
    }
    gameStateHistory.pop();
    fiftyMoveCounter = static_cast<int>(currentGameState & 4294950912) >> 14;
    plyCount--;
}

Board::Board() = default;
