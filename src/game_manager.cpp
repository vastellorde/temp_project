//
// Created by Арман Чукин on 02.12.2022.
//

#include "game_manager.h"

void GameManager::handleEvent() {
    sf::Event event{};
    if (m_window.pollEvent(event)) {
        auto position = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
        if (event.type == sf::Event::Closed)
            m_window.close();
        if (event.type == sf::Event::MouseButtonPressed) {
            if (gameState == InputState::NONE) {
                spdlog::info("handlePieceSelection");
                handlePieceSelection(position);
            } else if (gameState == InputState::DRAGGING_PIECE) {
                spdlog::info("handleDragMove");
                handleDragMove(position);
            } else if (gameState == InputState::PIECE_SELECTED) {
                spdlog::info("handlePiecePlacement");
                handlePiecePlacement(position);
            }
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            if (gameState == InputState::DRAGGING_PIECE) {
                handlePiecePlacement(position);
            }
        }
    }
}

void GameManager::cancelPieceSelection() {
    if (gameState != InputState::NONE) {
        gameState = InputState::NONE;
        m_boardUI.deselectSquare(selectedPieceSquare);
        m_boardUI.resetPiecePosition(selectedPieceSquare);
    }
}

void GameManager::tryMakeMove(sf::Vector2<int> startCoords, sf::Vector2<int> targetCoords) {
    int startIndex = BoardRepresentation::indexFromCoords(startCoords.x, startCoords.y);
    int targetIndex = BoardRepresentation::indexFromCoords(targetCoords.x, targetCoords.y);
    bool moveIsLegal = false;
    Move chosenMove{};

    auto legalMoves = MoveGenerator().generateMoves(&m_board);

    spdlog::info("legal moves size: {}", legalMoves.size());

    for (auto legalMove : legalMoves) {
        spdlog::info("legal moves {}", legalMove.isInvalid());
        spdlog::info("start square {}", legalMove.getStartSquare());
        spdlog::info("target square {}", legalMove.getTargetSquare());
        spdlog::info("start index {}", startIndex);
        spdlog::info("target index {}", targetIndex);
        if (legalMove.getStartSquare() == startIndex && legalMove.getTargetSquare() == targetIndex) {
            spdlog::info("check move");
            moveIsLegal = true;
            chosenMove = legalMove;
            spdlog::info("is legal move: {}", moveIsLegal);
            break;
        }
    }

    if (moveIsLegal) {
        m_board.makeMove(chosenMove);
        m_boardUI.onMoveMade(m_board,chosenMove);
        gameState = InputState::NONE;
    } else {
        cancelPieceSelection();
    }
}

void GameManager::handlePieceSelection(sf::Vector2f mousePosition) {
    if (m_boardUI.tryGetSquareUnderMouse(mousePosition, &selectedPieceSquare)) {
        int index = BoardRepresentation::indexFromCoords(selectedPieceSquare);
        spdlog::info("piece founded: {}", index);
        spdlog::info("piece type on board: {}", m_board.square[index]);
        if (Piece::isColor(m_board.square[index], m_board.colorToMove)) {
            spdlog::info("piece have correct color to move");
            m_boardUI.highlightLegalMoves(m_board, selectedPieceSquare);
            m_boardUI.selectSquare(selectedPieceSquare);
            gameState = InputState::DRAGGING_PIECE;
        }
    }
}


bool GameManager::isActive() {
    return m_window.isOpen();
}

void GameManager::update() {
    m_window.clear(sf::Color::Black);

    for (int rank = 0; rank < 8; rank++) {
        for (auto & boardSprite : m_boardUI.boardSprites) {
            m_window.draw(boardSprite[rank]);
        }
    }

    for (int rank = 0; rank < 8; rank++) {
        for (auto & pieceSprite: m_boardUI.pieceSprites) {
            m_window.draw(pieceSprite[rank]);
        }
    }
}

void GameManager::render() {
    m_window.display();
}

void GameManager::handlePiecePlacement(sf::Vector2f mousePosition) {
    sf::Vector2<int> targetSquare;
    if (m_boardUI.tryGetSquareUnderMouse(mousePosition, &targetSquare)) {
        spdlog::info("[handlePiecePlacement] square found");
        if (targetSquare == selectedPieceSquare) {
            spdlog::info("[handlePiecePlacement] target square equals to selected");
            m_boardUI.resetPiecePosition(selectedPieceSquare);
            if (gameState == InputState::DRAGGING_PIECE) {
                spdlog::info("[handlePiecePlacement] set piece selected");
                gameState = InputState::PIECE_SELECTED;
            } else {
                spdlog::info("[handlePiecePlacement] deselect square");
                gameState = InputState::NONE;
                m_boardUI.deselectSquare(selectedPieceSquare);
            }
        } else {
            spdlog::info("[handlePiecePlacement] target square not equals to selected");
            int const targetIndex = BoardRepresentation::indexFromCoords(targetSquare);
            if (Piece::isColor(m_board.square[targetIndex], m_board.colorToMove) && m_board.square[targetIndex] != 0) {
                spdlog::info("[handlePiecePlacement] reselect piece");
                cancelPieceSelection();
                handlePieceSelection(mousePosition);
            } else {
                spdlog::info("[handlePiecePlacement] try make move");
                tryMakeMove(selectedPieceSquare, targetSquare);
            }
        }
    } else {
        cancelPieceSelection();
    }
}

void GameManager::handleDragMove(sf::Vector2f mousePosition) {
    m_boardUI.dragPiece(selectedPieceSquare, mousePosition);
}
