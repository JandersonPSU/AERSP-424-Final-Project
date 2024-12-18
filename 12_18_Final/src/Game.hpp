#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "Player.hpp"
#include "EventLogger.hpp"


using namespace std;

const int GRID_SIZE = 10;
const char WATER = '~';
const char ISLAND = 'I';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = 'O';
const int BLITZ_TIME_LIMIT = 10;

class Player;

class Game {
public:
    Player* player1;
    Player* player2;
    bool blitzMode;

    Game();
    ~Game();

    void displayRules();
    void selectMap(vector<vector<char>>& grid);
    void start();
    bool isValidPlacement(Player& player, int x, int y, int length, char direction);
    void printGrid(const vector<vector<char>>& grid);
    void selectMode();
    void selectCaptain(Player*& player);

    template<typename T>
    bool timedInput(T &var, bool blitz, chrono::steady_clock::time_point startTime);

private:
    void generateShatteredSea(vector<vector<char>>& grid);
};

template<typename T>
bool Game::timedInput(T &var, bool blitz, chrono::steady_clock::time_point startTime) {
    if (!blitz) {
        while (!(cin >> var)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again." << endl;
        }
        return true;
    }

    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();

    if (elapsedTime >= BLITZ_TIME_LIMIT) {
        cout << "Time's up! Switching turns." << endl;
        return false;
    }

    if (!(cin >> var)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        currentTime = chrono::steady_clock::now();
        elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();

        if (elapsedTime >= BLITZ_TIME_LIMIT) {
            cout << "Time's up! Switching turns." << endl;
            return false;
        }
        cout << "Invalid input. Try again:" << endl;
        return timedInput(var, blitz, startTime);
    }

    currentTime = chrono::steady_clock::now();
    elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();

    if (elapsedTime >= BLITZ_TIME_LIMIT) {
        cout << "Time's up! Switching turns." << endl;
        return false;
    }

    return true;
}

#endif
