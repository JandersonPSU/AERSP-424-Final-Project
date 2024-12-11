#ifndef GAME_H
#define GAME_H

#include "Constants.h"
#include <vector>
#include <string>

class Player;

class Game {
public:
    Player* player1;
    Player* player2;
    bool blitzMode;

    Game();
    ~Game();

    void displayRules();
    void selectMap(std::vector<std::vector<char>>& grid);
    void start();
    bool isValidPlacement(Player& player, int x, int y, int length, char direction);
    void printGrid(const std::vector<std::vector<char>>& grid);
    void selectMode();
    void selectCaptain(Player*& player);

private:
    void generateShatteredSea(std::vector<std::vector<char>>& grid);
};

#endif // GAME_H
