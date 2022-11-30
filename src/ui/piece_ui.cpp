//
// Created by Арман Чукин on 28.11.2022.
//

#include "piece_ui.h"

PieceUi::PieceUi() {
    whitePieceSprites = {
            {1, sf::Vector2<int>{0, 0}},
            {2, sf::Vector2<int>{333 * 5, 0}},
            {3, sf::Vector2<int>{333 * 3, 0}},
            {5, sf::Vector2<int>{333 * 2, 0}},
            {6, sf::Vector2<int>{333 * 4, 0}},
            {7, sf::Vector2<int>{333 * 1, 0}},
    };
   blackPieceSprites = {
            {1, sf::Vector2<int>{0, 334}},
            {2, sf::Vector2<int>{333 * 5, 334}},
            {3, sf::Vector2<int>{333 * 3, 334}},
            {5, sf::Vector2<int>{333 * 2, 334}},
            {6, sf::Vector2<int>{333 * 4, 334}},
            {7, sf::Vector2<int>{333 * 1, 334}},
    };
}

sf::Vector2<int> PieceUi::getSprite(int piece) {
    std::map<int, sf::Vector2<int>> pieces = Piece::isWhite(piece) ? whitePieceSprites : blackPieceSprites;
    if (pieces.contains(Piece::pieceType(piece))) {
        return pieces[Piece::pieceType(piece)];
    }
    return {-1, -1};
}
