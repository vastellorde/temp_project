//
// Created by Арман Чукин on 01.12.2022.
//

#include "board_ui.h"

#include <utility>

void BoardUI::highlightLegalMoves(Board board, sf::Vector2<int> fromSquare) {
    if (showLegalMoves) {
        auto moves = moveGenerator.generateMoves(&board);

        for (auto move : moves) {
            if (move.getStartSquare() == BoardRepresentation::indexFromCoords(fromSquare.x, fromSquare.y)) {
                sf::Vector2<int> coord = BoardRepresentation::coordsFromIndex(move.getTargetSquare());
                setSquareColor(coord, colors.lightColors.legal, colors.darkColors.legal);
            }
        }
    }
}

void BoardUI::resetPiecePosition(sf::Vector2<int> pieceCoord) {
    auto position = positionFromCoords(pieceCoord.x, pieceCoord.y);
    pieceSprites[pieceCoord.x][pieceCoord.y].setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
}

void BoardUI::selectSquare(sf::Vector2<int> coords) {
    setSquareColor(coords, colors.lightColors.selected, colors.darkColors.selected);
}

void BoardUI::deselectSquare(sf::Vector2<int> coords) {
    resetSquareColor();
}

bool BoardUI::tryGetSquareUnderMouse(sf::Vector2f mouseCoords, sf::Vector2i* selectedCoord) {
    for (int i = 0; i < 8; i++) {
        for (auto & boardSprite : boardSprites) {
            auto bounds = boardSprite[i].getGlobalBounds();
            if (bounds.contains(mouseCoords)) {
                selectedCoord->x = static_cast<int>(boardSprite[i].getPosition().x / 200);
                selectedCoord->y = static_cast<int>(boardSprite[i].getPosition().y / 200);
                return true;
            }
        }
    }
    return false;
}

void BoardUI::updatePosition(Board board) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            sf::Vector2<int> coords = {file, rank};
            int piece = board.square[BoardRepresentation::indexFromCoords(coords.x, coords.y)];
            auto sprite = PieceUi().getSprite(piece);
            if (sprite.x != -1 && sprite.y != -1) {
                pieceSprites[file][rank].setTextureRect(sf::IntRect{sprite.x, sprite.y, 334, 334});
            } else {
                pieceSprites[file][rank].setTextureRect(sf::IntRect{0, 0, 0, 0});
            }
            pieceSprites[file][rank].setPosition(200 * file, 200 * rank);
        }
    }
}

void BoardUI::onMoveMade(Board board, Move move) {
    lastMadeMove = move;
    updatePosition(std::move(board));
    resetSquareColor();
}

void BoardUI::highLightMove(Move move) {
    setSquareColor(BoardRepresentation::coordsFromIndex(move.getTargetSquare()), colors.lightColors.moveFromHighlight, colors.darkColors.moveFromHighlight);
    setSquareColor(BoardRepresentation::coordsFromIndex(move.getTargetSquare()), colors.lightColors.moveToHighlight, colors.darkColors.moveToHighlight);
}

void BoardUI::createBoardUI() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool isWhite = (file + rank) % 2 != 0;
            sf::Sprite square;
            sf::Sprite piece;
            square.setTexture(boardTexture);
            square.setTextureRect(sf::IntRect(isWhite ? 0 : 60, 0, 60, 60));
            square.scale(3.33f, 3.33f);
            square.setPosition(200 * file, 200 * rank);
            boardSprites[file][rank] = square;

            piece.setTexture(texture);
            piece.scale(0.6f, 0.6f);
            piece.setPosition(200 * file, 200 * rank);
            pieceSprites[file][rank] = piece;
        }
    }
    resetSquareColor();
}

void BoardUI::resetSquarePositions() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            boardSprites[file][rank].setPosition(200 * file, 200 * rank);
            pieceSprites[file][rank].setPosition(200 * file, 200 * rank);
        }
    }

    if (!lastMadeMove.isInvalid()) {
        highLightMove(lastMadeMove);
    }
}

void BoardUI::setPerspective(bool whitePov) {
    whiteIsBottom = whitePov;
    resetSquarePositions();
}

void BoardUI::resetSquareColor(bool highlight) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            setSquareColor(sf::Vector2<int>(file, rank), colors.lightColors.normal, colors.darkColors.normal);
        }
    }
    if (highlight) {
        if (!lastMadeMove.isInvalid()) {
            highLightMove(lastMadeMove);
        }
    }
}

void BoardUI::setSquareColor(sf::Vector2<int> coords, sf::Color lightColor, sf::Color darkColor) {
    auto isLight = (coords.x + coords.y) % 2 != 0;
    boardSprites[coords.x][coords.y].setColor(isLight ? lightColor : darkColor);
}

sf::Vector2<int> BoardUI::positionFromCoords(int file, int rank) {
    return {file * 200, rank * 200};
}

sf::Vector2<int> BoardUI::positionFromCoords(sf::Vector2<int> coords) {
    return positionFromCoords(coords.x, coords.y);
}

BoardUI::BoardUI() {
    if (!texture.loadFromFile("assets/pieces.png")) {
        spdlog::error("error loading pieces");
    }
    if (!boardTexture.loadFromFile("assets/board.png")) {
        spdlog::error("error loading board");
    }

    createBoardUI();
}

void BoardUI::dragPiece(sf::Vector2<int> piecePos, sf::Vector2f mousePos) {
    pieceSprites[piecePos.x][piecePos.y].setPosition(mousePos);
}
