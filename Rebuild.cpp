// Remaking the main file so that is uses more classes

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;

const int GRID_SIZE = 10;
const char WATER = '~';
const char ISLAND = 'I';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = 'O';

class Player 
{
public:
    string name;
    vector<vector<char>> grid;
    vector<vector<char>> guessGrid;
    vector<int> shipLengths;
    bool usedPowerUp;

    Player(string name) : name(name), grid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)),
                          guessGrid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)), usedPowerUp(false) {}

    void placeShips(Game& game);
    bool takeTurn(Player& opponent);
    bool allShipsSunk() const;
};

class Game {
public:
    Player player1;
    Player player2;

    Game() : player1("Player 1"), player2("Player 2") {
        srand(time(0)); // Seed rand
    }

    void displayRules();
    void selectMap(vector<vector<char>>& grid);
    void start();
    bool isValidPlacement(Player& player, int x, int y, int length, char direction);
    void printGrid(const vector<vector<char>>& grid);

private:
    void generateShatteredSea(vector<vector<char>>& grid);
    void selectCaptain(Player& player);
};

void Game::displayRules() 
{
    cout << "Welcome to Battleship! Here are the rules:" << endl;
    cout << "1. Players take turns to place their ships on the grid." << endl;
    cout << "2. Each player has a set of ships to place, with different lengths." << endl;
    cout << "3. Players take turns attacking the opponent's grid, trying to hit ships." << endl;
    cout << "4. The first player to sink all opponent ships wins the game." << endl;
    cout << endl;
}