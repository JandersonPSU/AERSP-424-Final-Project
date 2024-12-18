#ifndef PLAYER_H
#define PLAYER_H

#include "Constants.h"
#include <vector>
#include <string>

class Game;

class Player {
public:
    std::string name;
    std::vector<std::vector<char>> grid;
    std::vector<std::vector<char>> guessGrid;
    std::vector<int> shipLengths;
    bool usedPowerUp;

    Player(std::string name);
    virtual ~Player() = default;

    void placeShips(Game& game);
    bool takeTurn(Player& opponent);
    bool allShipsSunk() const;
    virtual bool usePowerUp(Game& game, Player& opponent) = 0;
};

class Jenkins : public Player {
public:
    Jenkins();
    bool usePowerUp(Game& game, Player& opponent) override;
};

class Ironsides : public Player {
public:
    Ironsides();
    bool usePowerUp(Game& game, Player& opponent) override;
};

class Steven : public Player {
public:
    Steven();
    int powercounter;
    bool usePowerUp(Game& game, Player& opponent) override;
};

#endif // PLAYER_H
