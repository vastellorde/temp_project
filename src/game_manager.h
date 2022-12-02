//
// Created by Арман Чукин on 02.12.2022.
//

#ifndef CHESS_SDL_GAME_MANAGER_H
#define CHESS_SDL_GAME_MANAGER_H


#include <SFML/Window/Event.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "board.h"
#include "ui/board_ui.h"

enum class InputState {
    NONE,
    PIECE_SELECTED,
    DRAGGING_PIECE,
};

class GameManager {
public:
    explicit GameManager() {
        m_board.LoadPosition();
        m_boardUI.updatePosition(m_board);
        m_boardUI.resetSquareColor();
    }

    std::vector<Move> gameMoves;

    void update();
    void render();
    void handleEvent();

    bool isActive();

private:
    sf::RenderWindow m_window{sf::VideoMode(1600, 1600), "My window", sf::Style::Titlebar | sf::Style::Close};
    Board m_board{};
    BoardUI m_boardUI{};
    MoveGenerator moveGenerator{};
    sf::Vector2<int> selectedPieceSquare;
    InputState gameState = InputState::NONE;

    void handlePieceSelection(sf::Vector2f mousePosition);
    void cancelPieceSelection();

    void handlePiecePlacement(sf::Vector2f mousePosition);
    void handleDragMove(sf::Vector2f mousePosition);

    void tryMakeMove(sf::Vector2<int> startCoords, sf::Vector2<int> targetCoords);
};


#endif //CHESS_SDL_GAME_MANAGER_H
