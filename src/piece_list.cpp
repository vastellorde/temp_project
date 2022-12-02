//
// Created by Арман Чукин on 27.11.2022.
//

#include "piece_list.h"

PieceList::PieceList(int maxPiecesCount) {
    occupiedSquares = std::vector<int>(maxPiecesCount);
}

void PieceList::addPieceAtSquare(int square) {
    occupiedSquares[numPieces] = square;
    mapPiece[square] = numPieces;
    numPieces++;
}

PieceList::PieceList() {
    occupiedSquares = std::vector<int>();
}

void PieceList::removePieceAtSquare(int square) {
    int pieceIndex = mapPiece[square];
    occupiedSquares[pieceIndex] = occupiedSquares[numPieces - 1];
    mapPiece[occupiedSquares[pieceIndex]] = pieceIndex;
    numPieces--;
}

void PieceList::movePiece(int startSquare, int targetSquare) {
    int pieceIndex = mapPiece[startSquare];
    occupiedSquares[pieceIndex] = targetSquare;
    mapPiece[targetSquare] = pieceIndex;
}
