//
// Created by Арман Чукин on 28.11.2022.
//

#include "board_representation.h"

int BoardRepresentation::rankIndex(int squareIdx) {
    return squareIdx >> 3;
}

int BoardRepresentation::fileIndex(int squareIdx) {
    return squareIdx & 0b000111;
}

int BoardRepresentation::indexFromCoords(int fileIdx, int rankIdx) {
    return rankIdx * 8 + fileIdx;
}

sf::Vector2<int> BoardRepresentation::coordsFromIndex(int squareIdx) {
    return {fileIndex(squareIdx), rankIndex(squareIdx)};
}

bool BoardRepresentation::isLightSquare(int fileIdx, int rankIdx) {
    return (fileIdx + rankIdx) % 2 != 0;
}

std::string BoardRepresentation::squareNameFromCoords(int fileIdx, int rankIdx) {
    return std::to_string(fileNames[fileIdx]) + std::to_string(rankIdx + 1);
}

std::string BoardRepresentation::squareNameFromIndex(int index) {
    return squareNameFromCoords(coordsFromIndex(index));
}

std::string BoardRepresentation::squareNameFromCoords(sf::Vector2<int> coords) {
    return squareNameFromCoords(coords.x, coords.y);
}
