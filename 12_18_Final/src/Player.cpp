#include "Player.hpp"
#include "Game.hpp"
#include "EventLogger.hpp"

Player::Player(string name)
    : name(name), grid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)),
      guessGrid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)), usedPowerUp(false) {}

void Player::placeShips(Game& game) {
    // Prompt the player to place each ship
    cout << name << ", place your ships on the grid." << endl;
    game.printGrid(grid);
    for (int i = 0; i < (int)shipLengths.size(); ++i) 
    {
        int length = shipLengths[i];
        bool shipPlaced = false;
        while (!shipPlaced) 
        {
            int x, y; // Starting cords
            char direction; // Placement Direction
            cout << "Enter starting coordinates to place ship " << i + 1 << " of length " << length
                 << " (row and column), and direction (h/v/d): " << endl;
            while (!(cin >> x >> y >> direction)) 
            {
                // Validate input for coordinates and direction
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                cout << "Invalid input. Please enter two integers and a character."<<endl;
            }

            // Check if placement is valid using Game's method
            if (game.isValidPlacement(*this, x, y, length, direction)) 
            {
                event("placed ship", name, x, y, direction);
                // Place ship based on direction
                if (direction == 'h') // Horizontal Placement
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        grid[x][y + j] = SHIP; // Placing to the right of the slected point
                    }
                } 
                
                else if (direction == 'v')  // Vertical Placement
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        grid[x + j][y] = SHIP; // Placing below a certian point
                    }
                } 
                
                else if (direction == 'd')  // Diagonal Placement
                {
                    for (int j = 0; j < length; ++j) 
                    {
                        grid[x + j][y + j] = SHIP; //Using linear slope formula to place ship down and to the left of the code
                    }
                }
                
                shipPlaced = true;               // Ship successfully placed
                game.printGrid(grid);            // Show updated grid
            } 
            
            else
            {
                cout << "Invalid position or already occupied. Try again." << endl;
            }
        }
    }
}

bool Player::allShipsSunk() const {
    // Check grid for any 'S' cells left
    for (const auto& row : grid) 
    {
        for (char cell : row) 
        {
            if (cell == SHIP) 
            {
                return false; // Found a ship cell, so not all sunk
            }
        }
    }

    return true; // No ship cells found
}

bool Player::takeTurn(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) {
    // Prompt player for attack coordinates
    int x, y;
    event("chose attack", name);
    cout << name << ", enter coordinates to attack (row and column): " << endl;
    if (!game.timedInput(x, blitzMode, startTime)) return true; // If time up, end turn
    if (!game.timedInput(y, blitzMode, startTime)) return true; // If time up, end turn

    // Validate coordinates
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) 
    {
        cout << "Invalid coordinates. Try again." << endl;
        return false; // Let them try again this turn
    }

    // Check what is at that coordinate on the opponent's grid
    if (opponent.grid[x][y] == SHIP) // You scored a hit
    {
        cout << "It's a hit!" << endl;
        opponent.grid[x][y] = HIT;
        guessGrid[x][y] = HIT;
        event("successful hit", name, x, y);
        return true; // Turn completes successfully
    } 
    
    else if (opponent.grid[x][y] == WATER)  // You missed the shot
    {
        cout << "You missed." << endl;
        opponent.grid[x][y] = MISS;
        guessGrid[x][y] = MISS;
        event("unsuccessful hit", name, x, y);
        return true; // Turn completes with a miss
    } 
    
    else 
    {
        // Already attacked cell (HIT, MISS, or ISLAND)
        cout << "You already attacked this position. Try again." << endl;
        return false; // Must re-enter coordinates
    }
}

Jenkins::Jenkins() : Player("Jenkins") {}

bool Jenkins::usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) {
    if (usedPowerUp) 
    {
        cout << name << ", you have already used your power-up." << endl;
        return false; // Can't use power-up twice
    }

    usedPowerUp = true;
    cout << name << " is using their power-up!" << endl;
    event("using power-up radius search", name);

    int x, y;
    cout << "Enter the center coordinates to search in a 1 radius area (row and column): " << endl;
    event("searching in a 1 radius area", name);
    if (!game.timedInput(x, blitzMode, startTime)) return true; // If time out, turn ends
    if (!game.timedInput(y, blitzMode, startTime)) return true; // If time out, turn ends

    // Check a 3x3 block centered at (x,y)
    
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
    return true; // Power-up used
}

Ironsides::Ironsides() : Player("Ironsides") {}

bool Ironsides::usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) {
    if (usedPowerUp) 
    {
        cout << name << ", you have already used your power-up." << endl;
        return false;
    }

    usedPowerUp = true;
    cout << name << " is using their power-up!" << endl;
    event("using power-up row or colum search");

    char choice;
    int index;
    cout << "Enter 'r' to search an entire row or 'c' to search an entire column: " << endl; // Allows the player to choose between attacking a row or column
    if (!game.timedInput(choice, blitzMode, startTime)) return true;
    cout << "Enter the index of the row or column to search (0 to " << GRID_SIZE - 1 << "): " << endl;
    if (!game.timedInput(index, blitzMode, startTime)) return true;

    // Perform row scan
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
    
    // Perform column scan
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
        // Invalid choice or index
        cout << "Invalid choice or index." << endl;
        usedPowerUp = false; // Let user try again another turn
        return false;
    }
    
    return true; // Power-up used
}

Steven::Steven() : Player("Steven"), powercounter(1) {}

bool Steven::usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) {
    if (powercounter > 3) 
    {
        usedPowerUp = true; // If used more than three times do not allow to be used again
    }

    if (usedPowerUp) 
    {
        cout << name << ", you have already used your power-up." << endl; // Indicating that the power up is completely used up
        return false;
    }

    event("using power-up 3 attacks", name);
    cout << name << " is using their power-up!" << endl;
    cout << "Three attacks remaining" << endl;
    takeTurn(game, opponent, blitzMode, startTime); // 1st attack
    cout << "Two attacks remaining" << endl;
    takeTurn(game, opponent, blitzMode, startTime); // 2nd attack
    cout << "One attack remaining" << endl;
    takeTurn(game, opponent, blitzMode, startTime); // 3rd attack
    cout << "You can use this powerup " << 3 - powercounter << " more times." << endl;
    powercounter++; // Updating the number of times the power has been used.
    return true; // Power-up used
}
