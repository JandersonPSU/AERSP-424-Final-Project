// This is the work of Jack Anderson
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int GRID_SIZE = 10;
const char WATER = '~';
const char ISLAND = 'I';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = 'O';

struct Player 
{
    string name;
    vector<vector<char>> grid;
    vector<vector<char>> guessGrid;
    vector<int> shipLengths;
    bool usedPowerUp;
    Player(string name) : name(name), grid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)), guessGrid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)), usedPowerUp(false) {}
};

void printGrid(const vector<vector<char>>& grid) 
{
    for (const auto& row : grid) 
    {
        for (char cell : row) 
        {
            cout << cell << " ";
        }
        cout << endl;
    }
}

void generateShatteredSea(vector<vector<char>>& grid) 
{
    srand(time(0)); // Use the current time to seed rand
    int numIslands = rand() % 15 + 5; // Random number of islands between 5 and 20
    
  for (int i = 0; i < numIslands; ++i) 
    {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        grid[x][y] = ISLAND;
    }
}

void selectMap(vector<vector<char>>& grid) 
{
    int choice;
    cout << "Select a map:" << endl;
    cout << "1. The Open Seas (All water)" << endl;
    cout << "2. The Shattered Sea (Random islands)" << endl;
    cin >> choice;

    if (choice == 1) 
    {
        // The Open Seas - All water, no changes needed
        cout << "You have selected the map 'The Open Seas'" << endl;
    } 
    else if (choice == 2) 
    {
        generateShatteredSea(grid);
        cout << "You have selected the map 'The Shattered Sea'" << endl;
    } 
    else 
    {
        cout << "Invalid choice. Defaulting to The Open Seas." << endl;
    }
}

void placeShips(Player& player) 
{
    cout << player.name << ", place your ships on the grid." << endl;
    
    for (int i = 0; i < player.shipLengths.size(); ++i) 
    {
        int length = player.shipLengths[i];
        bool shipPlaced = false;
        while (!shipPlaced) 
        {
            int x, y;
            char direction;
            cout << "Enter starting coordinates to place ship " << i + 1 << " of length " << length << " (row and column), and direction (h for horizontal, v for vertical, d for diagonal): " << endl;
            cin >> x >> y >> direction;
            bool validPlacement = true;
            if (direction == 'h') 
            {
                if (y + length > GRID_SIZE) 
                {
                    validPlacement = false;
                } 
                
                else 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        if (player.grid[x][y + j] != WATER) 
                        {
                            validPlacement = false;
                            break;
                        }
                    }
                }
                
                if (validPlacement) 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        player.grid[x][y + j] = SHIP;
                    }
                }
            } 
            
            else if (direction == 'v') 
            {
                if (x + length > GRID_SIZE) 
                {
                    validPlacement = false;
                } 
                
                else 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        if (player.grid[x + j][y] != WATER) 
                        {
                            validPlacement = false;
                            break;
                        }
                    }
                }
                if (validPlacement) 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        player.grid[x + j][y] = SHIP;
                    }
                }
            } 
            
            else if (direction == 'd') 
            {
                if (x + length > GRID_SIZE || y + length > GRID_SIZE) 
                {
                    validPlacement = false;
                } 
                
                else 
                {
                    
                    for (int j = 0; j < length; ++j) 
                    {
                        if (player.grid[x + j][y + j] != WATER) 
                        
                        {
                            validPlacement = false;
                            break;
                        }
                    }
                }
                
                if (validPlacement) 
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        player.grid[x + j][y + j] = SHIP;
                    }
                }
            } 
            
            else 
            {
                validPlacement = false;
            }

            if (validPlacement) 
            {
                shipPlaced = true;
                printGrid(player.grid);
            } 
            
            else 
            {
                cout << "Invalid position or already occupied. Try again." << endl;
            }
        }
    }
}

void selectCaptain(Player& player) 
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

bool takeTurn(Player& attacker, Player& defender) 
{
    int x, y;
    cout << attacker.name << ", enter coordinates to attack (row and column): " << endl;
    cin >> x >> y;
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) 
    {
        cout << "Invalid coordinates. Try again." << endl;
        return false;
    }
    if (defender.grid[x][y] == SHIP) 
    {
        cout << "It's a hit!" << endl;
        defender.grid[x][y] = HIT;
        attacker.guessGrid[x][y] = HIT;
        return true;
    } 
    else if (defender.grid[x][y] == WATER) 
    {
        cout << "You missed." << endl;
        defender.grid[x][y] = MISS;
        attacker.guessGrid[x][y] = MISS;
        return true;
    } 
    else 
    {
        cout << "You already attacked this position. Try again." << endl;
        return false;
    }
}

void usePowerUp(Player& player, Player& opponent) 
{
    if (player.usedPowerUp) 
    {
        cout << player.name << ", you have already used your power-up." << endl;
        return;
    }

    player.usedPowerUp = true;
    cout << player.name << " is using their power-up!" << endl;

    if (player.shipLengths == vector<int>{1, 2, 3, 4, 5}) // Old Man Jenkins
    { 
        int x, y;
        cout << "Enter the center coordinates to search in a 1 radius area (row and column): " << endl;
        cin >> x >> y;
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
                        cout << "Hit found at (" << newX << ", " << newY << ")!" << endl;
                        opponent.grid[newX][newY] = HIT;
                        player.guessGrid[newX][newY] = HIT;
                    } 
                    else if (opponent.grid[newX][newY] == WATER) 
                    {
                        cout << "Miss at (" << newX << ", " << newY << ")" << endl;
                        player.guessGrid[newX][newY] = MISS;
                    }
                }
            }
        }
    } 
    else if (player.shipLengths == vector<int>{2, 2, 2, 4, 5}) // Old Ironsides
    { 
        char choice;
        int index;
        cout << "Enter 'r' to search an entire row or 'c' to search an entire column: " << endl;
        cin >> choice;
        cout << "Enter the index of the row or column to search (0 to " << GRID_SIZE - 1 << "): " << endl;
        cin >> index;
        if (choice == 'r' && index >= 0 && index < GRID_SIZE) 
        {
            for (int j = 0; j < GRID_SIZE; ++j) 
            {
                if (opponent.grid[index][j] == SHIP) 
                {
                    cout << "Hit found at (" << index << ", " << j << ")!";
                    opponent.grid[index][j] = HIT;
                    player.guessGrid[index][j] = HIT;
                } 
                else if (opponent.grid[index][j] == WATER) 
                {
                    cout << "Miss at (" << index << ", " << j << ")";
                    player.guessGrid[index][j] = MISS;
                }
            }
        } 
        
        else if (choice == 'c' && index >= 0 && index < GRID_SIZE) 
        {
            for (int i = 0; i < GRID_SIZE; ++i) 
            {
                if (opponent.grid[i][index] == SHIP) 
                {
                    cout << "Hit found at (" << i << ", " << index << ")!";
                    opponent.grid[i][index] = HIT;
                    player.guessGrid[i][index] = HIT;
                } 
                else if (opponent.grid[i][index] == WATER) 
                {
                    cout << "Miss at (" << i << ", " << index << ")";
                    player.guessGrid[i][index] = MISS;
                }
            }
        } 
        else 
        {
            cout << "Invalid choice or index." << endl;
            player.usedPowerUp = false; // Allow retry if input was invalid
        }
    }
}

bool allShipsSunk(const Player& player) 
{
    for (const auto& row : player.grid) 
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

int main() 
{
    Player player1("Player 1");
    Player player2("Player 2");

    // Select the map for both players
    selectMap(player1.grid);
    printGrid(player1.grid);
    player2.grid = player1.grid;

    selectCaptain(player1);
    selectCaptain(player2);

    placeShips(player1);
    placeShips(player2);

    bool gameOver = false;
    Player* currentPlayer = &player1;
    Player* opponentPlayer = &player2;

    while (!gameOver) 
    {
        cout << currentPlayer->name << "'s turn:" << endl;
        printGrid(currentPlayer->guessGrid);

        char action;
        cout << "Enter 'a' to attack or 'p' to use your power-up: " << endl;
        cin >> action;

        if (action == 'a') 
        {
            if (takeTurn(*currentPlayer, *opponentPlayer)) 
            {
                if (allShipsSunk(*opponentPlayer)) 
                {
                    cout << currentPlayer->name << " wins! All opponent ships have been sunk." << endl;
                    gameOver = true;
                } 
                else 
                {
                    swap(currentPlayer, opponentPlayer);
                }
            }
        } 
        else if (action == 'p') 
        {
            usePowerUp(*currentPlayer, *opponentPlayer);
        } 
        else 
        {
            cout << "Invalid action. Try again." << endl;
        }
    }

    return 0;
}
