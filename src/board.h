//
// Created by Арман Чукин on 27.11.2022.
//

#ifndef CHESS_SDL_BOARD_H
#define CHESS_SDL_BOARD_H


#include <stack>
#include "piece_list.h"
#include "utils/move.h"

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
    int fiftyMoveCounter = 0;
    int plyCount;
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

    void makeMove(Move move, bool inSearch = false);
    void unmakeMove(Move move, bool inSearch = false);
private:
    std::stack<unsigned int> gameStateHistory;

    const unsigned int whiteCastleKingsideMask = 0b1111111111111110;
    const unsigned int whiteCastleQeensideMask = 0b1111111111111101;
    const unsigned int blackCastleKingsideMask = 0b1111111111111011;
    const unsigned int blackCastleQeensideMask = 0b1111111111110111;

    const unsigned int whiteCastleMask = whiteCastleKingsideMask & whiteCastleQeensideMask;
    const unsigned int blackCastleMask = blackCastleKingsideMask & blackCastleQeensideMask;
};


#endif //CHESS_SDL_BOARD_H
