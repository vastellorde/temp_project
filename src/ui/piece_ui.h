//
// Created by Арман Чукин on 28.11.2022.
//

#ifndef CHESS_SDL_PIECE_UI_H
#define CHESS_SDL_PIECE_UI_H


#include <SFML/System/Vector2.hpp>
#include <map>
#include "../Piece.h"

class PieceUi {
public:
    sf::Vector2<int> getSprite(int piece);

    PieceUi();

private:
    std::map<int, sf::Vector2<int>> whitePieceSprites;
    std::map<int, sf::Vector2<int>> blackPieceSprites;
};


#endif //CHESS_SDL_PIECE_UI_H
