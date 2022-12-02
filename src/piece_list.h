//
// Created by Арман Чукин on 27.11.2022.
//

#ifndef CHESS_SDL_PIECE_LIST_H
#define CHESS_SDL_PIECE_LIST_H

#include <vector>


class PieceList {
public:
    PieceList();
    explicit PieceList(int maxPieceCount);
    std::vector<int> occupiedSquares;

    void addPieceAtSquare(int square);
    void removePieceAtSquare(int square);
    void movePiece(int startSquare, int targetSquare);

    const int& operator [](int idx) const {
        return occupiedSquares[idx];
    }

    [[nodiscard]] int count() const {
        return numPieces;
    }
private:
    int numPieces = 0;
    int mapPiece[64]{};
};


#endif //CHESS_SDL_PIECE_LIST_H
