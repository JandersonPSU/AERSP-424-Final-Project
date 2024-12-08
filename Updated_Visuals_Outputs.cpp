// Blitz Mode Debug Stage 3
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

const int GRID_SIZE = 10;
const char WATER = '~';
const char ISLAND = 'I';
const char SHIP = 'S';
const char HIT = 'X';
const char MISS = 'O';

class Game; // Forward declaration

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
    virtual bool usePowerUp(Game& game, Player& opponent)=0;
};
class Jenkins : public Player {
public:
    Jenkins() : Player("Jenkins") {}
    bool usePowerUp(Game& game, Player& opponent) override {
        
        if (usedPowerUp) 
        {
            cout << name << ", you have already used your power-up." << endl;
            return false;
        }

        usedPowerUp = true;
        cout << name << " is using their power-up!" << endl;
        //powerup start definition
        int x, y;
        cout << "Enter the center coordinates to search in a 1 radius area (row and column): " << endl;
        while (true) 
        {
            cin >> x >> y ;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
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
                        cout << "Hit found at (" << newX << ", " << newY << ")!" << endl;
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
};

class Ironsides : public Player {
public:
    Ironsides() : Player("Ironsides") {}
    bool usePowerUp(Game& game, Player& opponent) override {
        if (usedPowerUp) 
        {
            cout << name << ", you have already used your power-up." << endl;
            return false;
        }

        usedPowerUp = true;
        cout << name << " is using their power-up!" << endl;
        // Powerup Start Definition
        char choice;
        int index;

        cout << "Enter 'r' to search an entire row or 'c' to search an entire column: " << endl;
        while (true) 
        {
            cin >> choice ;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
            } 
            else 
            {
                break; 
            }
        }
        cout << "Enter the index of the row or column to search (0 to " << GRID_SIZE - 1 << "): " << endl;
        while (true) 
        {
            cin >> index ;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
            } 
            else 
            {
                break; 
            }
        }
        
        if (choice == 'r' && index >= 0 && index < GRID_SIZE) 
        {
            for (int j = 0; j < GRID_SIZE; ++j) 
            {
                if (opponent.grid[index][j] == SHIP) 
                {
                    cout << "Hit found at (" << index << ", " << j << ")!" << endl;
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
                    cout << "Hit found at (" << i << ", " << index << ")!" << endl;
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
            cout << "Invalid choice or index." << endl;
            usedPowerUp = false; // Allow retry if input was invalid
            return false;
        }
        return true;
    }
};

class Steven : public Player {
public:
    Steven() : Player("Steven") {}
    int powercounter=1;
    bool usePowerUp(Game& game, Player& opponent) override {
        if (powercounter>3)
        {
            usedPowerUp = true;
        }
    
        if (usedPowerUp) 
        {
            cout << name << ", you have already used your power-up." << endl;
            return false;
        }

        cout << name << " is using their power-up!" << endl;
        cout<< "Three attacks remaining"<<endl;
        takeTurn(opponent);
        cout << "Two attacks remaining"<<endl;
        takeTurn(opponent);
        cout << "One attacks remaining"<<endl;
        takeTurn(opponent);
        cout<< "You can use this powerup "<<3-powercounter<<" more times."<<endl;
        powercounter++;
        return true;
    }

};
class Game 
{
public:
    Player* player1;
    Player* player2;
    bool blitzMode;

    Game() : blitzMode(false) 
    {
        srand(time(0)); // Seed rand
        player1 = new Jenkins(); // Default to Jenkins
        player2 = new Ironsides(); // Default to Ironsides
    }

    ~Game(){
        delete player1;
        delete player2;
    }

    void displayRules();
    void selectMap(vector<vector<char>>& grid);
    void start();
    bool isValidPlacement(Player& player, int x, int y, int length, char direction);
    void printGrid(const vector<vector<char>>& grid);
    void selectMode();
    void selectCaptain(Player*& player);

private:
    void generateShatteredSea(vector<vector<char>>& grid);
    
};

void Game::displayRules() 
{
    string BattleshipTitle=R"( 
    ____        __  __  __          __    _     
   / __ )____ _/ /_/ /_/ /__  _____/ /_  (_)___ 
  / __  / __ `/ __/ __/ / _ \/ ___/ __ \/ / __ \
 / /_/ / /_/ / /_/ /_/ /  __(__  ) / / / / /_/ /
/_____/\__,_/\__/\__/_/\___/____/_/ /_/_/ .___/ 
                                       /_/        )";
    cout << BattleshipTitle<<endl;
    cout << "Welcome to Battleship! Here are the rules:" << endl;
    cout << "1. Players take turns to place their ships on the grid." << endl;
    cout << "2. Each player has a set of ships to place, with different lengths." << endl;
    cout << "3. Players take turns attacking the opponent's grid, trying to hit ships." << endl;
    cout << "4. Each captain has a special power-up that can be used once per match." << endl;
    cout << "5. The first player to sink all opponent ships wins the game." << endl;
    cout << "6. In Blitz Battleship mode, players have only 15 seconds per turn." << endl;
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
    
    do 
    {
        cout << "Select a map:" << endl;
        cout << "1. The Open Seas (All water)" << endl;
        cout << "2. The Shattered Sea (Random islands)" << endl;
        while (true) 
        {
            cin >> choice;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
            } 
            else 
            {
                break; 
            }
        }
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

void Game::selectMode() 
{
    int choice;
    do 
    {
        cout << "Select game mode:" << endl;
        cout << "1. Classic Battleship" << endl;
        cout << "2. Blitz Battleship" << endl;
        while (true) 
        {
            cin >> choice;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
            } 
            else 
            {
                break; 
            }
        }
        if (choice == 1) 
        {
            blitzMode = false;
            cout << "You have selected 'Classic Battleship' mode." << endl;
        } 
        
        else if (choice == 2) 
        {
            blitzMode = true;
            cout << "You have selected 'Blitz Battleship' mode." << endl;
        } 
        
        else 
        {
            cout << "Invalid choice. Please select again." << endl;
        }
    } 
    
    while (choice != 1 && choice != 2);
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

void Game::selectCaptain(Player*& player) 
{
    delete player;
    bool check=true;
    int choice;
    cout <<"Choose your captain:" << endl;
    cout << "1. Old Man Jenkins (5 ships: lengths 1, 2, 3, 4, 5, power-up: search 1 radius around a point)" << endl;
    cout << "2. Old Ironsides (5 ships: lengths 2, 2, 2, 4, 5, power-up: search one entire row or column)" << endl;
    cout << "3. Threeven Steven (5 Ships: lengths 3, 3, 3, 3, 3, power-up: search three points in one turn, power is available to use three times)" << endl;
        
    // Checking input
    while (check){
        while (true) 
        {
            cin >> choice;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
            } 
            else 
            {
                break; 
            }
        }
        if (choice == 1) 
        {
            player = new Jenkins();
            player->shipLengths = {1, 2, 3, 4, 5};
            check=false;
        } 
        else if (choice == 2) 
        {
            player = new Ironsides();
            player->shipLengths = {2, 2, 2, 4, 5};
            check=false;
        } 
        else if (choice == 3) 
        {
            player = new Steven();
            player->shipLengths = {3, 3, 3, 3, 3};
            check=false;
        } 
        else 
        {
            cout << "Invalid choice, please pick pick again (1, 2, or 3)." << endl;
        }
    }
    cout << player->name << " has been chosen as captain!" << endl;
}

void Game::printGrid(const vector<vector<char>>& grid) 
{
    string spaces =R"(         )";
    string bottomline=R"(________________________________________)";
    cout<<bottomline<<endl;
    cout << "  "<<spaces;
    
    for (int i = 0; i < GRID_SIZE; ++i) 
    {
        cout << i << " ";
    }
    cout << endl;

    for (int i = 0; i < grid.size(); ++i) 
    {
        cout <<spaces<< i << " ";
        for (char cell : grid[i]) 
        {
            cout << cell << " ";
        }
        cout << endl;
    }
    cout<<bottomline<<endl;
}

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
            cout << "Enter starting coordinates to place ship " << i + 1 << " of length " << length
                 << " (row and column), and direction (h for horizontal, v for vertical, d for diagonal): " << endl;
            while (true) 
            {
                cin >> x >> y >> direction;
                if (cin.fail()) 
                { 
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    cout << "Invalid input. Please enter an integer."<<endl;
                } 
                else 
                {
                    break; 
                }
            }

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
    while (true) 
        {
            cin >> x >> y;
            if (cin.fail()) 
            { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
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
    selectMap(player1->grid);
    player2->grid = player1->grid; // Same map for both players

    // Select captains
    selectCaptain(player1);
    selectCaptain(player2);

    // Select game mode
    selectMode();

    // Place ships
    
    player1->placeShips(*this);
    cout << string(50, '\n');
    cout << "Ships placed plase switch players" << endl;
    cout << string(5, '\n');
    this_thread::sleep_for(chrono::seconds(5));

    player2->placeShips(*this);
    cout << string(50, '\n');
    cout << "Ships placed plase switch players" << endl;
    cout << string(5, '\n');
    this_thread::sleep_for(chrono::seconds(5));

    // Main game loop
    bool gameOver = false;
    Player* currentPlayer = player1;
    Player* opponentPlayer = player2;

    while (!gameOver) 
    {
        auto startTime = chrono::steady_clock::now();
        bool turnComplete = false;
        string opponent=R"( 
 ________________________________________
|  __   __   __   __        ___      ___ | 
| /  \ |__) |__) /  \ |\ | |__  |\ |  |  |
| \__/ |    |    \__/ | \| |___ | \|  |  |
|________________________________________|
        )";
        string yourself=R"(
 ________________________________________
|                  __       	         |
|             \ / /  \ |  |              |
|              |  \__/ \__/              |
|________________________________________|
        )";

        cout << currentPlayer->name << "'s turn:" << endl;
        cout<<yourself<<endl;
        printGrid(currentPlayer->grid);
        cout << opponent<<endl;
        printGrid(currentPlayer->guessGrid);

        while (!turnComplete) 
        {
            char action;
            cout << "Enter 'a' to attack or 'p' to use your power-up: " << endl;
            while (true) {
            cin >> action;
            if (cin.fail()) { 
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter an integer."<<endl;
            } else {
                break; 
            }
            }

            if (action == 'a') 
            {
                turnComplete = currentPlayer->takeTurn(*opponentPlayer);
            } 
            
            else if (action == 'p') 
            {
                turnComplete=currentPlayer->usePowerUp(*this, *opponentPlayer);
            } 
            
            else 
            {
                cout << "Invalid action. Try again." << endl;
            }

            if (blitzMode) 
            {
                auto currentTime = chrono::steady_clock::now();
                auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= 15) 
                {
                    cout << "Time's up! Switching turns." << endl; // Add "Wipe Screen Here"
                    cout << string(50, '\n');
                    this_thread::sleep_for(chrono::seconds(5));
                    turnComplete = true;
                }
            }
        }
        cout<<"Updated Grid:"<<endl;
        cout << opponent<<endl;
        printGrid(currentPlayer->guessGrid);

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

int main() 
{
    Game game;
    game.start();
    return 0;
}
