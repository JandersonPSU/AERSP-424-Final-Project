#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include <vector>
#include <chrono>
#include "Game.hpp"
#include "EventLogger.hpp"

using namespace std;

class Game; // Forward declaration

class Player {
public:
    string name;
    vector<vector<char>> grid;
    vector<vector<char>> guessGrid;
    vector<int> shipLengths;
    bool usedPowerUp;

    Player(string name);
    virtual ~Player() = default;

    void placeShips(Game& game);
    bool allShipsSunk() const;
    virtual bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) = 0;
    bool takeTurn(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime);
};

class Jenkins : public Player {
public:
    Jenkins();
    bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) override;
};

class Ironsides : public Player {
public:
    Ironsides();
    bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) override;
};

class Steven : public Player {
public:
    Steven();
    int powercounter;
    bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) override;
};

#endif
