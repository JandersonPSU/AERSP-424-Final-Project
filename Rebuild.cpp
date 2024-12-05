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

class Game; // Forward declaration to deal with an issues where the class game doesnt load

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

class Game 
{
public:

    Player player1;
    Player player2;

    Game() : player1("Player 1"), player2("Player 2") 
    {
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

void Game::generateShatteredSea(vector<vector<char>>& grid) 
{
    int numIslands = rand() % 15 + 5; // Random number of islands between 5 and 20
    for (int i = 0; i < numIslands; ++i) 
    {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        grid[x][y] = ISLAND;
    }
}

void Game::selectMap(vector<vector<char>>& grid) 
{
    int choice;
    do {
        cout << "Select a map:" << endl;
        cout << "1. The Open Seas (All water)" << endl;
        cout << "2. The Shattered Sea (Random islands)" << endl;
        cin >> choice;
        if (choice == 1) 
        {
            cout << "You have selected the map 'The Open Seas'" << endl;
        } 
        else if (choice == 2) 
        {
            generateShatteredSea(grid);
            cout << "You have selected the map 'The Shattered Sea'" << endl;
        } 
        else 
        {
            cout << "Invalid choice. Please select again." << endl;
        }
    } 
    while (choice != 1 && choice != 2);

    // Display the selected map to the players
    cout << "Here is the selected map:" << endl;
    printGrid(grid);
    cout << endl;
}


bool Game::isValidPlacement(Player& player, int x, int y, int length, char direction) 
{
    if (direction == 'h') 
    {
        if (y + length > GRID_SIZE) return false;
        
        for (int j = 0; j < length; ++j) 
        {
            if (player.grid[x][y + j] != WATER) return false;
        }

    } 
    else if (direction == 'v') 
    {
        if (x + length > GRID_SIZE) return false;
        
        for (int j = 0; j < length; ++j) 
        {
            if (player.grid[x + j][y] != WATER) return false;
        }
    } 
    
    else if (direction == 'd') 
    {
        if (x + length > GRID_SIZE || y + length > GRID_SIZE) return false;
        
        for (int j = 0; j < length; ++j) 
        {
            if (player.grid[x + j][y + j] != WATER) return false;
        }
    } 
    else 
    {
        return false; // Invalid direction
    }
    
    return true;
}

void Game::selectCaptain(Player& player) 
{
    int choice;
    cout << player.name << ", choose your captain:" << endl;
    cout << "1. Old Man Jenkins (5 ships: lengths 1, 2, 3, 4, 5, power-up: search 1 radius around a point)" << endl;
    cout << "2. Old Ironsides (5 ships: lengths 2, 2, 2, 4, 5, power-up: search one entire row or column)" << endl;
    cin >> choice;

    if (choice == 1) 
    {
        player.shipLengths = {1, 2, 3, 4, 5};
    } 
    else if (choice == 2) 
    {
        player.shipLengths = {2, 2, 2, 4, 5};
    } 
    else 
    {
        cout << "Invalid choice. Defaulting to Old Man Jenkins." << endl;
        player.shipLengths = {1, 2, 3, 4, 5};
    }
}

void Game::printGrid(const vector<vector<char>>& grid) 
{
    cout << "  ";
    for (int i = 0; i < GRID_SIZE; ++i) 
    {
        cout << i << " ";
    }
    cout << endl;

    for (int i = 0; i < grid.size(); ++i) 
    {
        cout << i << " ";
        for (char cell : grid[i]) 
        {
            cout << cell << " ";
        }
        cout << endl;
    }
}

void Player::placeShips(Game& game) 
{
    cout << name << ", place your ships on the grid." << endl;
    for (int i = 0; i < shipLengths.size(); ++i) 
    {
        int length = shipLengths[i];
        bool shipPlaced = false;
        while (!shipPlaced) 
        {
            int x, y;
            char direction;
            cout << "Enter starting coordinates to place ship " << i + 1 << " of length " << length
                 << " (row and column), and direction (h for horizontal, v for vertical, d for diagonal): " << endl;
            cin >> x >> y >> direction;

            if (game.isValidPlacement(*this, x, y, length, direction)) 
            {
                if (direction == 'h') 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        grid[x][y + j] = SHIP;
                    }
                } 
                else if (direction == 'v') 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        grid[x + j][y] = SHIP;
                    }
                } 
                else if (direction == 'd') 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        grid[x + j][y + j] = SHIP;
                    }
                }
                shipPlaced = true;
                game.printGrid(grid);
            } 
            else 
            {
                cout << "Invalid position or already occupied. Try again." << endl;
            }
        }
    }
}

bool Player::takeTurn(Player& opponent) 
{
    int x, y;
    cout << name << ", enter coordinates to attack (row and column): " << endl;
    cin >> x >> y;

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
        cout << "You already attacked this position. Try again." << endl;
        return false;
    }
}

bool Player::allShipsSunk() const 
{
    for (const auto& row : grid) 
    {
        for (char cell : row) 
        {
            if (cell == SHIP) 
            {
                return false;
            }
        }
    }
    
    return true;
}

void Game::start() 
{
    displayRules();

    // Select the map for both players
    selectMap(player1.grid);
    player2.grid = player1.grid; // Same map for both players

    // Select captains
    selectCaptain(player1);
    selectCaptain(player2);

    // Place ships
    player1.placeShips(*this);
    cout << string(50, '\n');
    cout << "Ships placed plase switch players" << endl;
    cout << string(5, '\n');
    this_thread::sleep_for(chrono::seconds(5));

    player2.placeShips(*this);
    cout << string(50, '\n');
    cout << "Ships placed plase switch players" << endl;
    cout << string(5, '\n');
    this_thread::sleep_for(chrono::seconds(5));

    // Main game loop
    bool gameOver = false;
    Player* currentPlayer = &player1;
    Player* opponentPlayer = &player2;

    while (!gameOver) 
    {
        cout << currentPlayer->name << "'s turn:" << endl;
        printGrid(currentPlayer->guessGrid);

        if (currentPlayer->takeTurn(*opponentPlayer)) 
        {
            if (opponentPlayer->allShipsSunk()) 
            {
                cout << currentPlayer->name << " wins! All opponent ships have been sunk." << endl;
                gameOver = true;
            } 
            else 
            {
                cout << string(50, '\n');
                cout << "Switching turns. Please hand device to other player..." << endl;
                cout << string(5, '\n');
                this_thread::sleep_for(chrono::seconds(5));
                swap(currentPlayer, opponentPlayer);

            }
        }
    }
}

int main() // I wanted to see how small I could make the main function
{
    Game game;
    game.start();
    return 0;
}