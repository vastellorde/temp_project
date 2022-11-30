//
// Created by Арман Чукин on 27.11.2022.
//

#ifndef CHESS_SDL_BOARD_H
#define CHESS_SDL_BOARD_H


#include <stack>
#include "piece_list.h"

class Board {
public:
    Board();
    inline static int whiteIndex = 0;
    inline static int blackIndex = 1;
    int pieceList[64]{};
    int kingSquare[2]{};
    unsigned int currentGameState;
    int colorToMove;
    int colorToMoveIndex;
    int opponentColor;
    bool whiteToMove;
    std::vector<int> square = std::vector<int>(64);

    std::vector<PieceList> rooks;
    std::vector<PieceList> bishop;
    std::vector<PieceList> queens;
    std::vector<PieceList> knights;
    std::vector<PieceList> pawns;

    std::vector<PieceList> allPieces;

    PieceList getPieceList(int pieceType, int color);

    void init();

    void LoadPosition();
private:
    std::stack<unsigned int> gameStateHistory;
};


#endif //CHESS_SDL_BOARD_H
