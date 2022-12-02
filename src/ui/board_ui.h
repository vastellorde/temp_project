//
// Created by Арман Чукин on 01.12.2022.
//

#ifndef CHESS_SDL_BOARD_UI_H
#define CHESS_SDL_BOARD_UI_H


#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "../utils/move.h"
#include "../utils/move_generator.h"
#include "../resources/Colors.h"
#include "../ui/piece_ui.h"
#include "spdlog/spdlog.h"

class BoardUI {
public:
    sf::Sprite boardSprites[8][8];
    sf::Sprite pieceSprites[8][8];

    bool showLegalMoves = false;
    bool whiteIsBottom = true;

    void highlightLegalMoves(Board board, sf::Vector2<int> fromSquare);
    void resetPiecePosition(sf::Vector2<int> pieceCoord);
    void selectSquare(sf::Vector2<int> coords);
    void deselectSquare(sf::Vector2<int> coords);
    bool tryGetSquareUnderMouse(sf::Vector2f mouseCoords, sf::Vector2i* selectedCoord);
    void updatePosition(Board board);
    void onMoveMade(Board board, Move move);
    void setPerspective(bool whitePov);
    void resetSquareColor(bool highlight = true);
    void dragPiece(sf::Vector2<int> piecePos, sf::Vector2f mousePos);
    sf::Vector2<int> positionFromCoords(int file, int rank);
    sf::Vector2<int> positionFromCoords(sf::Vector2<int> coords);

    BoardUI();

private:
    sf::Texture texture;
    sf::Texture boardTexture;
    Move lastMadeMove{};
    MoveGenerator moveGenerator;
    Colors colors{};

    void highLightMove(Move move);
    void createBoardUI();
    void resetSquarePositions();
    void setSquareColor(sf::Vector2<int> coords, sf::Color lightColor, sf::Color darkColor);
};


#endif //CHESS_SDL_BOARD_UI_H
