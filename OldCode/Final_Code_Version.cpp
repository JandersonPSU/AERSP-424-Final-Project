// Blitz Mode Debug Stage 4

#include <iostream>   // For input/output using std::cin and std::cout
#include <vector>     // For using std::vector to represent the grids
#include <thread>     // For std::this_thread::sleep_for
#include <chrono>     // For std::chrono::seconds to handle timing
#include <cstdlib>    // For rand(), srand()
#include <ctime>      // For time() seed used by srand()
#include <limits>     // For std::numeric_limits used in input validation
#include <fstream>    // For ofstream

using namespace std;  // Use the standard namespace for convenience

// Constants defining the grid and various cell states
const int GRID_SIZE = 10; // The board is a 10x10 grid
const char WATER = '~';   // Represents a water cell
const char ISLAND = 'I';  // Represents an island cell
const char SHIP = 'S';    // Represents a ship cell
const char HIT = 'X';     // Represents a hit cell
const char MISS = 'O';    // Represents a miss cell
const int BLITZ_TIME_LIMIT = 10; // In blitz mode, each turn is limited to 10 seconds

// creating a funciton which will allow for logging of game events, with potential to record the message, player name, x and y coords and ship direction
ofstream logfile;
void event(const string& message, const string& name = "Console", int x = -1, int y = -1, char direction = ' ') 
{
    // makes sure it only creates the fie once or else it will keep making new files each move
    if (!logfile.is_open())
    {
        // get the current hour, min, second
        time_t tnow = time(0);
        tm* tinfo = localtime(&tnow);

        // get a timestamp
        char timestamp[9];  
        strftime(timestamp, sizeof(timestamp), "%H_%M_%S", tinfo); 

        // name the file based off the timestamp so it doesnt overwrite previous files (unless you happen to make the file at the same time a different day which is unlikely)
        string filename = "GameLog_" + string(timestamp) + ".txt";  // Add underscore for clearer filename

        // append the file
        logfile.open(filename, ios::app);
    }

    // now getting times and timestamp for the time an event occurs to be printed in the file next to the event
    time_t tnow = time(0);
    tm* tinfo = localtime(&tnow);
    char timestamp[9];  
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tinfo); 

    // print the timestamp in the file
    logfile << "[" << timestamp << "] " << name << ": ";

    // see if the message parameter is satisfied or not so it can print a blank message
    if (!message.empty()) {
        logfile << message;
    }

    // checks to see if an x or y is provided
    if (x != -1 && y != -1) 
    {
        logfile << " (" << x << ", " << y << ")";
    }

    // checks to see if a direciton is provided
    if (direction != ' ') 
    {
        logfile << " Direction: " << direction;
    }
    logfile << endl;
}

class Player; // Forward declaration of Player so we can use Player* in Game

// The Game class handles the overall flow: map selection, mode selection, turn-taking, win condition
class Game 
{
public:
    Player* player1;            // Pointer to first player's chosen captain
    Player* player2;            // Pointer to second player's chosen captain
    bool blitzMode;             // True if Blitz mode is selected, else Classic mode

    Game();                     // Constructor: sets defaults and seeds random
    ~Game();                    // Destructor: deletes players

    void displayRules();                                      // Print the game rules
    void selectMap(vector<vector<char>>& grid);                // Let user pick a map (Open Seas or Shattered Sea)
    void start();                                              // Start the main game loop
    bool isValidPlacement(Player& player, int x, int y, int length, char direction); // Check if a ship can be placed
    void printGrid(const vector<vector<char>>& grid);          // Print a given grid
    void selectMode();                                         // Choose between Classic or Blitz mode
    void selectCaptain(Player*& player);                       // Choose the captain for a player

    // Template function for time-limited input in blitz mode
    template<typename T>
    bool timedInput(T &var, bool blitz, chrono::steady_clock::time_point startTime) 
    {
        // If not blitz mode, just read input until valid
        if(!blitz) 
        {
            while (!(cin >> var)) 
            {                            // Attempt to read input
                cin.clear();                                   // Clear error flags
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << "Invalid input. Try again." << endl;   // Prompt user again
            }
            return true;                                       // Input was successful
        }

        // In blitz mode, check remaining time before reading input
        auto currentTime = chrono::steady_clock::now();        // Get current time
        auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count(); // Calculate elapsed seconds
        if (elapsedTime >= BLITZ_TIME_LIMIT) // If time already up
        {                 
            cout << "Time's up! Switching turns." << endl;     // Notify and return false
            return false;
        }

        // Try to read input
        if (!(cin >> var))  // If failed to read valid input
        {                                   
            cin.clear();                                       // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid line
            currentTime = chrono::steady_clock::now();         // Check time again
            elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
            if (elapsedTime >= BLITZ_TIME_LIMIT) {             // If time expired now
                cout << "Time's up! Switching turns." << endl; // Notify and fail
                return false;
            }
            cout << "Invalid input. Try again:" << endl;       // Prompt again
            return timedInput(var, blitz, startTime);          // Recursive call to attempt input again
        }

        // Check time once more after successful input
        currentTime = chrono::steady_clock::now();
        elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
        if (elapsedTime >= BLITZ_TIME_LIMIT) // If time expired now
        {                 
            cout << "Time's up! Switching turns." << endl;     // Notify and fail
            return false;
        }
        return true;                                           // All good: input successful and time not expired
    }

private:
    void generateShatteredSea(vector<vector<char>>& grid);      // Create a random map with islands
};

// The Player class is abstract, each specific captain extends it with a unique power-up
class Player 
{
public:
    string name;                   // Player or captain name
    vector<vector<char>> grid;     // Player's own board
    vector<vector<char>> guessGrid;// Player's view of opponent's board
    vector<int> shipLengths;       // The lengths of the ships this captain places
    bool usedPowerUp;              // Tracks if this player's special ability is used

    Player(string name) 
      : name(name), grid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)),
        guessGrid(GRID_SIZE, vector<char>(GRID_SIZE, WATER)), usedPowerUp(false) {} // Initialize fields

    void placeShips(Game& game);   // Let player place all their ships
    bool allShipsSunk() const;     // Check if all ships are destroyed
    virtual bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime)=0; // Must be overridden
    bool takeTurn(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime); // Perform an attack
};

// Jenkins: a specific type of captain with a certain power-up ability
class Jenkins : public Player 
{
public:
    Jenkins() : Player("Jenkins") {} // Constructor sets name
    bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) override; // Jenkins's power-up
};

// Ironsides: another captain type with a different power-up ability
class Ironsides : public Player 
{
public:
    Ironsides() : Player("Ironsides") {} // Constructor sets name
    bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) override; // Ironsides's power-up
};

// Steven: another captain type with a unique triple-attack power-up
class Steven : public Player 
{
public:
    Steven() : Player("Steven") {} // Constructor sets name
    int powercounter=1;            // Count how many times Steven used power-up
    bool usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) override; // Steven's power-up
};

// ========== Game Method Implementations ==========

Game::Game() : blitzMode(false) // Initially not in blitz mode
{
    srand((unsigned)time(0));   // Seed the random number generator
    player1 = new Jenkins();    // Default player1 to Jenkins
    player2 = new Ironsides();  // Default player2 to Ironsides
}

Game::~Game()
{
    delete player1;             // Clean up player1
    delete player2;             // Clean up player2
}

void Game::displayRules() 
{
    // Print ASCII art and rules summary
    string BattleshipTitle=R"( 
    ____        __  __  __          __    _     
   / __ )____ _/ /_/ /_/ /__  _____/ /_  (_)___ 
  / __  / __ / __/ __/ / _ \/ ___/ __ \/ / __ \
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
    cout << "6. In Blitz Battleship mode, players have only 10 seconds per turn." << endl;
    cout << endl;
}

void Game::generateShatteredSea(vector<vector<char>>& grid) 
{
    // Randomly scatter islands across the grid
    int numIslands = rand() % 15 + 5; // Between 5 and 19 islands
    for (int i = 0; i < numIslands; ++i) {
        int x = rand() % GRID_SIZE;   // Random row
        int y = rand() % GRID_SIZE;   // Random column
        grid[x][y] = ISLAND;          // Place island
    }
}

void Game::selectMap(vector<vector<char>>& grid) 
{
    // User chooses the map: Open Seas or Shattered Sea
    int choice;
    do 
    {
        cout << "Select a map:" << endl;
        cout << "1. The Open Seas (All water)" << endl;
        cout << "2. The Shattered Sea (Random islands)" << endl;
        
        while (!(cin >> choice)) 
        {                          // Validate input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter an integer."<<endl;
        }
        
        if (choice == 1) 
        {
            cout << "You have selected the map 'The Open Seas'" << endl;
            event("the Open Seas map was chosen");
        } 
        
        else if (choice == 2) 
        {
            generateShatteredSea(grid);                      // Generate islands
            cout << "You have selected the map 'The Shattered Sea'" << endl;
            event("the Shattered Sea map was chosen");
        } 
        
        else 
        {
            cout << "Invalid choice. Please select again." << endl;
        }
    } 

    while (choice != 1 && choice != 2);

    // Show the chosen map
    cout << "Here is the selected map:" << endl;
    printGrid(grid);
    cout << endl;
}

void Game::selectMode() 
{
    // Let the user pick game mode: Classic or Blitz
    int choice;
    do 
    {
        cout << "Select game mode:" << endl;
        cout << "1. Classic Battleship" << endl;
        cout << "2. Blitz Battleship" << endl;
        
        while (!(cin >> choice)) // Validating input 
        {                  
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter an integer."<<endl;
        }
        
        if (choice == 1) 
        {
            blitzMode = false;                      // Player has selected Classic mode
            cout << "You have selected 'Classic Battleship' mode." << endl;
            event("classic mode was selected");
        } 
        
        else if (choice == 2) 
        {
            blitzMode = true;                       //  Player has selected Blitz mode
            cout << "You have selected 'Blitz Battleship' mode." << endl;
            event("blitz mode was selected");
        } 
        
        else 
        {
            cout << "Invalid choice. Please select again." << endl;
        }
    
    }
    
    while (choice != 1 && choice != 2); // Making sure that a gamemode is selected before continuing along

    cout << endl;
}

bool Game::isValidPlacement(Player& player, int x, int y, int length, char direction) 
{
    // Check if we can place a ship of 'length' starting at (x,y) in the given direction
    if (direction == 'h') 
    {
        // Horizontal placement
        if (y + length > GRID_SIZE) return false;   // Goes off right edge
        for (int j = 0; j < length; ++j) 
        {
            if (player.grid[x][y + j] != WATER) return false; // Occupied cell
        }
    } 
    
    else if (direction == 'v') 
    {
        // Vertical placement
        if (x + length > GRID_SIZE) return false;   // Goes off bottom edge
        for (int j = 0; j < length; ++j) 
        {
            if (player.grid[x + j][y] != WATER) return false; // Occupied cell
        }
    } 
    else if (direction == 'd') 
    {
        // Diagonal placement
        if (x + length > GRID_SIZE || y + length > GRID_SIZE) return false; // Goes off edge
        for (int j = 0; j < length; ++j) 
        {
            if (player.grid[x + j][y + j] != WATER) return false; // Occupied cell
        }
    } 
    
    else 
    {
        return false; // Invalid direction character
    }
    
    return true; // Placement is valid
}

void Game::selectCaptain(Player*& player) 
{
    // Delete old player and prompt user to pick a new captain
    delete player; // Memory Clearing
    bool check=true;
    int choice;
    cout <<"Choose your captain:" << endl;
    cout << "1. Old Man Jenkins (5 ships: 1,2,3,4,5)" << endl;
    cout << "2. Old Ironsides (5 ships: 2,2,2,4,5)" << endl;
    cout << "3. Threeven Steven (5 ships: 3,3,3,3,3)" << endl;
        
    while (check)
    {
        
        while (!(cin >> choice))  // Validate input
        {                   
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter an integer."<<endl;
        }
        
        if (choice == 1)  // Pick Jenkins
        {
            player = new Jenkins();                  // Assigns current player to have the jenkins class
            player->shipLengths = {1, 2, 3, 4, 5};   // Assigns the current player jenkins fleet
            check=false;
        } 
        
        else if (choice == 2) // Pick Ironsides
        {
            player = new Ironsides();                // Assigns current player to have the ironsides class
            player->shipLengths = {2, 2, 2, 4, 5};   // Assigns the current player ironsides fleet
            check=false;
        } 
        
        else if (choice == 3) 
        {
            player = new Steven();                   // Pick Steven
            player->shipLengths = {3, 3, 3, 3, 3};   // Assigns the current player jenkins fleet
            check=false;
        } 
        
        else 
        {
            cout << "Invalid choice, please pick again (1, 2, or 3)." << endl;
        }
    }
    
    cout << player->name << " has been chosen as captain!" << endl;
    event("chosen captain", player->name);
}

void Game::printGrid(const vector<vector<char>>& grid) 
{
    // Print the grid with formatting and indices
    string spaces =R"(         )"; // GUI Visual Formatting
    string bottomline=R"(________________________________________)"; // GUI Visual Formatting
    cout<<bottomline<<endl;
    cout << "  "<<spaces;
    
    for (int i = 0; i < GRID_SIZE; ++i) 
    {
        cout << i << " ";                           // Print column indices
    }
    cout << endl;

    for (int i = 0; i < (int)grid.size(); ++i) 
    {
        cout <<spaces<< i << " ";                   // Print row index
        for (char cell : grid[i]) 
        {
            cout << cell << " ";                    // Print cell symbol
        }
        cout << endl;
    }
    
    cout<<bottomline<<endl;
}

void Game::start()  // The main game sequence: from setup to the turn-taking loop until someone wins
{
    event("game initialized");

    displayRules(); // Show rules first                                // Show rules first
    

    // Players share the same map layout: first choose map for player1
    selectMap(player1->grid);
    player2->grid = player1->grid;  // Copy map to player2              

    // Choose captains for both players
    selectCaptain(player1);
    selectCaptain(player2);

    // Choose game mode (Classic or Blitz)
    selectMode();

    // Player 1 places ships
    player1->placeShips(*this);
    
    // Handles the screen wipe after player 2 has finished placing their ships
    cout << "Ships placed please switch players" << endl;
    this_thread::sleep_for(chrono::seconds(5)); // Gives time to hand over laptop
    cout << string(100, '\n');
    this_thread::sleep_for(chrono::seconds(5));

    // Player 2 places ships
    player2->placeShips(*this);

    // Handles the screen wipe after player 2 has finished placing their ships
    cout << "Ships placed please switch players" << endl;
    this_thread::sleep_for(chrono::seconds(5)); // Gives time to hand over laptop
    cout << string(100, '\n');
    this_thread::sleep_for(chrono::seconds(5));

    // Main game loop: take turns until one player wins
    bool gameOver = false;
    Player* currentPlayer = player1;
    Player* opponentPlayer = player2;

    while (!gameOver) 
    {
        auto startTime = chrono::steady_clock::now(); // Track turn start time for blitz
        bool turnComplete = false;

        // ASCII banners for opponent and self
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
        cout<<yourself<<endl;                      // Printing player's own grid
        printGrid(currentPlayer->grid);
        cout << opponent<<endl;                    // Printing player's guess grid of opponent
        printGrid(currentPlayer->guessGrid);

        while (!turnComplete) 
        {
            // Check time limit for blitz mode
            auto currentTime = chrono::steady_clock::now();
            auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
            if (blitzMode && elapsedTime >= BLITZ_TIME_LIMIT) 
            {
                // If out of time, switch turns
                cout << "Time's up! Switching turns." << endl; 
                event("time limit reached, switching");
                this_thread::sleep_for(chrono::seconds(5));
                cout << string(100, '\n');
                turnComplete = true;
                break;
            }

            cout << "Enter 'a' to attack or 'p' to use your power-up: " << endl;
            
            char action;
            if (!timedInput(action, blitzMode, startTime)) 
            {
                // If time runs out during input, end turn
                turnComplete = true;
                break;
            }

            if (action == 'a') 
            {
                // Attack action
                if (!currentPlayer->takeTurn(*this, *opponentPlayer, blitzMode, startTime)) 
                {
                    // If attack invalid or repeated, retry unless time out
                    currentTime = chrono::steady_clock::now(); //Blitz time tracking
                    elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count(); //Blitz time tracking
                    if (blitzMode && elapsedTime >= BLITZ_TIME_LIMIT) 
                    {
                        // If out of time, switch turns
                        cout << "Time's up! Switching turns." << endl;
                        event("time limit reached, switching"); 
                        this_thread::sleep_for(chrono::seconds(5));
                        cout << string(100, '\n');
                        turnComplete = true;
                    }
                } 
                
                else 
                {
                    // Successful attack ends turn
                    turnComplete = true;
                }
            } 
            
            else if (action == 'p') 
            {
                // Power-up action
                if (!currentPlayer->usePowerUp(*this, *opponentPlayer, blitzMode, startTime)) 
                {
                    // If power-up failed (invalid or already used), check time and maybe retry if time permits
                    currentTime = chrono::steady_clock::now();
                    elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
                    if (blitzMode && elapsedTime >= BLITZ_TIME_LIMIT) 
                    {
                         // If out of time, switch turns
                        cout << "Time's up! Switching turns." << endl;
                        event("time limit reached, switching");
                        cout << string(50, '\n');
                        this_thread::sleep_for(chrono::seconds(5));
                        turnComplete = true;
                    }
                } 
                
                else 
                {
                    // Successful power-up ends turn
                    turnComplete = true;
                }
            } 
            
            else 
            {
                // Invalid action input
                cout << "Invalid action. Try again." << endl;
            }

            // Final time check if turn not complete
            if (blitzMode && !turnComplete) 
            {
                currentTime = chrono::steady_clock::now();
                elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= BLITZ_TIME_LIMIT) 
                {

                     // If out of time, switch turns
                    cout << "Time's up! Switching turns." << endl;
                    event("time limit reached, switching");
                    this_thread::sleep_for(chrono::seconds(5));
                    cout << string(100, '\n');
                    turnComplete = true;
                }
            }
        }

        // After turn completes, print updated guess grid
        cout<<"Updated Grid:"<<endl;
        cout << opponent<<endl;
        printGrid(currentPlayer->guessGrid);

        // Check if opponent is defeated
        if (opponentPlayer->allShipsSunk()) 
        {
            // Announcing the winner 
            cout << currentPlayer->name << " wins! All opponent ships have been sunk." << endl;
            event("has won the game", currentPlayer->name);
            event("game is now terminated");
            this_thread::sleep_for(chrono::seconds(20));
            gameOver = true;
        } 
        
        else if (!gameOver) 
        {
            // If not game over, switch turns
            cout << "Switching turns. Please hand device to other player..." << endl;
            this_thread::sleep_for(chrono::seconds(5));
            cout << string(100, '\n');
            this_thread::sleep_for(chrono::seconds(5));
            swap(currentPlayer, opponentPlayer);
        }
    
    }

}

void Player::placeShips(Game& game) 
{
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

bool Player::allShipsSunk() const // used to determine if the game has ended
{
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

bool Player::takeTurn(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) 
{
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

// Jenkins's power-up: search a 1-radius area around a chosen point
bool Jenkins::usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) 
{
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

// Ironsides's power-up: search an entire row or column
bool Ironsides::usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) 
{
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

// Steven's power-up: three consecutive attacks in one turn
bool Steven::usePowerUp(Game& game, Player& opponent, bool blitzMode, chrono::steady_clock::time_point startTime) 
{
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

int main() 
{
    // Entry point: create a game object and start it
    Game game;
    game.start();
    return 0;
}
