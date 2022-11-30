//
// Created by Арман Чукин on 28.11.2022.
//

#ifndef CHESS_SDL_BOARD_REPRESENTATION_H
#define CHESS_SDL_BOARD_REPRESENTATION_H


#include <string>
#include <SFML/System/Vector2.hpp>

static const std::string fileNames = "abcdefgh";
static const std::string rankNames = "12345678";

class BoardRepresentation {
public:
    const int a1 = 0;
    const int b1 = 1;
    const int c1 = 2;
    static const int d1 = 3;
    const int e1 = 4;
    static const int f1 = 5;
    const int g1 = 6;
    const int h1 = 7;

    const int a8 = 56;
    const int b8 = 57;
    const int c8 = 58;
    static const int d8 = 59;
    const int e8 = 60;
    static const int f8 = 61;
    const int g8 = 62;
    const int h8 = 63;

    static int rankIndex(int squareIdx);
    static int fileIndex(int squareIdx);
    static int indexFromCoords(int fileIdx, int rankIdx);
    static sf::Vector2<int> coordsFromIndex(int squareIdx);
    static bool isLightSquare(int fileIdx, int rankIdx);
    static std::string squareNameFromCoords(int fileIdx, int rankIdx);
    static std::string squareNameFromCoords(sf::Vector2<int> coords);
    static std::string squareNameFromIndex(int index);
};


#endif //CHESS_SDL_BOARD_REPRESENTATION_H
