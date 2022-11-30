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
}

Board::Board() = default;
