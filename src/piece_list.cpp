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
