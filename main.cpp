#include "src/game_manager.h"


int main() {
    GameManager gameManager{};
    while (gameManager.isActive()) {
        gameManager.handleEvent();
        gameManager.update();
        gameManager.render();
    }
    return 0;
}
