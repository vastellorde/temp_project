#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>
#include "src/board.h"
#include "src/ui/piece_ui.h"
#include "src/board_representation.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 1600), "My window", sf::Style::Titlebar | sf::Style::Close);
    sf::Texture texture;
    sf::Texture boardTexture;
    sf::Sprite boardSprites[8][8];
    sf::Sprite pieceSprites[8][8];
    Board board;
    board.LoadPosition();
    PieceUi pieceUI;
    if (!texture.loadFromFile("assets/pieces.png")) {
        spdlog::error("error loading image: {}" , "assets/pieces.png");
        return -1;
    }
    if (!boardTexture.loadFromFile("assets/board.png")) {
        spdlog::error("cannot load board texture");
        return -1;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bool isWhite = (i + j) % 2 != 0;
            sf::Sprite square;
            sf::Sprite piece;
            square.setTexture(boardTexture);
            square.setTextureRect(sf::IntRect(isWhite ? 0 : 60, 0, 60, 60));
            square.scale(3.33f, 3.33f);
            square.setPosition(200 * j, 200 * i);
            boardSprites[j][i] = square;

            piece.setTexture(texture);
            int p = board.square[BoardRepresentation::indexFromCoords(j, i)];
            spdlog::info("board piece: {}", p);
            sf::Vector2<int> coords = pieceUI.getSprite(p);
            if (coords.x != -1 && coords.y != -1) {
                piece.setTextureRect(sf::IntRect(coords.x, coords.y, 334, 334));
            } else {
                piece.setTextureRect(sf::IntRect(0, 0, 0, 0));
            }
            piece.scale(0.6f, 0.6f);
            piece.setPosition(200 * j, 200 * i);
            pieceSprites[j][i] = piece;
        }
    }
    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                auto position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (auto& pieceSprite : pieceSprites) {
                    for (auto& p : pieceSprite) {
                        auto bounds = p.getGlobalBounds();
                        if (bounds.contains(position)) {
                            p.setPosition(100, 100);
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        for (int rank = 0; rank < 8; rank++) {
            for (auto & boardSprite : boardSprites) {
                window.draw(boardSprite[rank]);
            }
        }

        for (int rank = 0; rank < 8; rank++) {
            for (auto & pieceSprite: pieceSprites) {
                window.draw(pieceSprite[rank]);
            }
        }

        window.display();
    }

    return 0;
}
