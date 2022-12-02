//
// Created by Арман Чукин on 02.12.2022.
//

#ifndef CHESS_SDL_COLORS_H
#define CHESS_SDL_COLORS_H

#include <SFML/Graphics/Color.hpp>

struct SquareColors {
    sf::Color legal;
    sf::Color selected;
    sf::Color moveFromHighlight;
    sf::Color moveToHighlight;
    sf::Color normal;
};

class Colors {
public:
    SquareColors lightColors;
    SquareColors darkColors;

    Colors();
};


#endif //CHESS_SDL_COLORS_H
