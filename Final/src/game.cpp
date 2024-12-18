// game.cpp
#include "Game.h"
#include "Player.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

Game::Game() : blitzMode(false) 
{
    srand(time(0)); // Seed rand
    player1 = new Jenkins(); // Default to Jenkins
    player2 = new Ironsides(); // Default to Ironsides
}

Game::~Game() 
{
    delete player1;
    delete player2;
}

void Game::displayRules() 
{
    string BattleshipTitle=R"( 
    ____        __  __  __          __    _     
   / __ )____ _/ /_/ /_/ /__  _____/ /_  (_)___ 
  / __  / __ `/ __/ __/ / _ \/ ___/ __ \/ / __ \
 / /_/ / /_/ / /_/ /_/ /  __(__  ) / / / / /_/ /
/_____ /\__,_/\__/\__/_/\___/____/_/ /_/_/ .___/ 
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
    int choice;
    cout <<"Choose your captain:" << endl;
    cout << "1. Old Man Jenkins (5 ships: lengths 1, 2, 3, 4, 5, power-up: search 1 radius around a point)" << endl;
    cout << "2. Old Ironsides (5 ships: lengths 2, 2, 2, 4, 5, power-up: search one entire row or column)" << endl;
    cout << "3. Threeven Steven (5 Ships: lengths 3, 3, 3, 3, 3, power-up: search three points in one turn, power is available to use three times)" << endl;
    
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
    } 
    else if (choice == 2) 
    {
        player = new Ironsides();
        player->shipLengths = {2, 2, 2, 4, 5};
    } 
    else if (choice == 3) 
    {
        player = new Steven();
        player->shipLengths = {3, 3, 3, 3, 3};
    } 
    else 
    {
        cout << "Invalid choice, please pick pick again (1, 2, or 3)." << endl;
        selectCaptain(player);
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
    this_thread::sleep_for(chrono::seconds(1));
    cout << string(50, '\n');
    cout << "Ships placed plase switch players" << endl;
    cout << string(5, '\n');
    this_thread::sleep_for(chrono::seconds(1));

    player2->placeShips(*this);
    this_thread::sleep_for(chrono::seconds(1));
    cout << string(50, '\n');
    cout << "Ships placed plase switch players" << endl;
    cout << string(5, '\n');
    this_thread::sleep_for(chrono::seconds(1));

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
                cout << "Invalid input. Please enter a character."<<endl;
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
            this_thread::sleep_for(chrono::seconds(20));
            gameOver = true;
        } 
        
        else 
        {
            this_thread::sleep_for(chrono::seconds(1));
            cout << string(50, '\n');
            cout << "Switching turns. Please hand device to other player..." << endl;
            cout << string(5, '\n');
            this_thread::sleep_for(chrono::seconds(1));
            swap(currentPlayer, opponentPlayer);

        }
    }
}
