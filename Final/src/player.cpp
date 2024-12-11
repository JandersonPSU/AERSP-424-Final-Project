// player.cpp
#include "Player.h"
#include "Game.h"
#include <iostream>
#include <limits>
using namespace std;

// Player Class Implementation
Player::Player(string name) 
    : name(name), grid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)),
      guessGrid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)), usedPowerUp(false) {}

void Player::placeShips(Game& game) 
{
    cout << name << ", place your ships on the grid." << endl;
    game.printGrid(grid);
    for (int i = 0; i < shipLengths.size(); ++i) 
    {
        int length = shipLengths[i];
        bool shipPlaced = false;
        while (!shipPlaced) 
        {
            int x, y;
            char direction;
            cout << "Enter starting coordinates to place ship " << i + 1 
                 << " of length " << length << " (row, column), and direction (h/v/d): " << endl;
            while (true) 
            {
                cin >> x >> y >> direction;
                if (cin.fail()) 
                { 
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    cout << "Invalid input. Please try again." << endl;
                } 
                else 
                {
                    break;
                }
            }

            if (game.isValidPlacement(*this, x, y, length, direction)) 
            {
                for (int j = 0; j < length; ++j) 
                {
                    if (direction == 'h') grid[x][y + j] = SHIP;
                    else if (direction == 'v') grid[x + j][y] = SHIP;
                    else if (direction == 'd') grid[x + j][y + j] = SHIP;
                }
                shipPlaced = true;
                game.printGrid(grid);
            } 
            else 
            {
                cout << "Invalid position or overlap. Try again." << endl;
            }
        }
    }
}

bool Player::takeTurn(Player& opponent) 
{
    int x, y;
    cout << name << ", enter coordinates to attack (row and column): " << endl;
    while (true) 
    {
        cin >> x >> y;
        if (cin.fail()) 
        { 
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter valid integers." << endl;
        } 
        else 
        {
            break;
        }
    }

    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) 
    {
        cout << "Invalid coordinates. Try again." << endl;
        return false;
    }

    if (opponent.grid[x][y] == SHIP) 
    {
        cout << "It's a hit!" << endl;
        opponent.grid[x][y] = HIT;
        guessGrid[x][y] = HIT;
        return true;
    } 
    else if (opponent.grid[x][y] == WATER) 
    {
        cout << "You missed." << endl;
        opponent.grid[x][y] = MISS;
        guessGrid[x][y] = MISS;
        return true;
    } 
    else 
    {
        cout << "Position already attacked. Try again." << endl;
        return false;
    }
}

bool Player::allShipsSunk() const 
{
    for (const auto& row : grid) 
    {
        for (char cell : row) 
        {
            if (cell == SHIP) return false;
        }
    }
    return true;
}

// Jenkins Class Implementation
Jenkins::Jenkins() : Player("Jenkins") {}

bool Jenkins::usePowerUp(Game& game, Player& opponent) 
{
    if (usedPowerUp) 
    {
        cout << name << ", you already used your power-up." << endl;
        return false;
    }
    usedPowerUp = true;
    cout << name << " is using the power-up!" << endl;

    int x, y;
    cout << "Enter the center coordinates for a 1-radius search (row and column): " << endl;
    while (true) 
    {
        cin >> x >> y;
        if (cin.fail()) 
        { 
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Try again." << endl;
        } 
        else 
        {
            break;
        }
    }

    for (int i = -1; i <= 1; ++i) 
    {
        for (int j = -1; j <= 1; ++j) 
        {
            int newX = x + i;
            int newY = y + j;
            if (newX >= 0 && newX < GRID_SIZE && newY >= 0 && newY < GRID_SIZE) 
            {
                if (opponent.grid[newX][newY] == SHIP) 
                {
                    cout << "Hit at (" << newX << ", " << newY << ")!" << endl;
                    opponent.grid[newX][newY] = HIT;
                    guessGrid[newX][newY] = HIT;
                } 
                else if (opponent.grid[newX][newY] == WATER) 
                {
                    cout << "Miss at (" << newX << ", " << newY << ")" << endl;
                    guessGrid[newX][newY] = MISS;
                }
            }
        }
    }
    return true;
}

// Ironsides Class Implementation
Ironsides::Ironsides() : Player("Ironsides") {}

bool Ironsides::usePowerUp(Game& game, Player& opponent) 
{
    if (usedPowerUp) 
    {
        cout << name << ", you already used your power-up." << endl;
        return false;
    }
    usedPowerUp = true;
    cout << name << " is using the power-up!" << endl;

    char choice;
    int index;
    cout << "Enter 'r' for row or 'c' for column, followed by the index (0-" << GRID_SIZE - 1 << "): " << endl;
    cin >> choice >> index;

    if (choice == 'r' && index >= 0 && index < GRID_SIZE) 
    {
        for (int j = 0; j < GRID_SIZE; ++j) 
        {
            if (opponent.grid[index][j] == SHIP) 
            {
                cout << "Hit at (" << index << ", " << j << ")!" << endl;
                opponent.grid[index][j] = HIT;
                guessGrid[index][j] = HIT;
            } 
            else if (opponent.grid[index][j] == WATER) 
            {
                cout << "Miss at (" << index << ", " << j << ")" << endl;
                guessGrid[index][j] = MISS;
            }
        }
    } 
    else if (choice == 'c' && index >= 0 && index < GRID_SIZE) 
    {
        for (int i = 0; i < GRID_SIZE; ++i) 
        {
            if (opponent.grid[i][index] == SHIP) 
            {
                cout << "Hit at (" << i << ", " << index << ")!" << endl;
                opponent.grid[i][index] = HIT;
                guessGrid[i][index] = HIT;
            } 
            else if (opponent.grid[i][index] == WATER) 
            {
                cout << "Miss at (" << i << ", " << index << ")" << endl;
                guessGrid[i][index] = MISS;
            }
        }
    } 
    else 
    {
        cout << "Invalid input." << endl;
        usedPowerUp = false;
        return false;
    }
    return true;
}

// Steven Class Implementation
Steven::Steven() : Player("Steven"), powercounter(1) {}

bool Steven::usePowerUp(Game& game, Player& opponent) 
{
    if (powercounter > 3) 
    {
        usedPowerUp = true;
    }
    if (usedPowerUp) 
    {
        cout << name << ", you already used your power-up." << endl;
        return false;
    }

    cout << name << " is using the power-up! You have three extra attacks this turn." << endl;
    for (int i = 0; i < 3; ++i) 
    {
        cout << "Attack " << (3 - i) << " remaining." << endl;
        takeTurn(opponent);
    }
    ++powercounter;
    return true;
}
