//
// Created by Арман Чукин on 27.11.2022.
//

#ifndef CHESS_SDL_FEN_UTIL_H
#define CHESS_SDL_FEN_UTIL_H

#include <string>
#include "position_info.h"
#include <map>
#include "../Piece.h"
#include "../board_representation.h"

class FenUtil {
public:
    PositionInfo PositionFromFen(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::string currentFen();
private:
    static std::vector<std::string> split(const std::string& input, const std::string& delimiter);
    std::map<char, int> pieceTypeFromSymbol = std::map<char, int>{
            {'k', Piece::King},
            {'p', Piece::Pawn},
            {'n', Piece::Knight},
            {'b', Piece::Bishop},
            {'r', Piece::Rook},
            {'q', Piece::Queen}
    };
};





#endif //CHESS_SDL_FEN_UTIL_H
