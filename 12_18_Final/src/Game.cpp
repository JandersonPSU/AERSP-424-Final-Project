#include "Game.hpp"
#include "Player.hpp"


Game::Game() : blitzMode(false) {
    srand((unsigned)time(0));   // Seed the random number generator
    player1 = new Jenkins();    // Default player1 to Jenkins
    player2 = new Ironsides();  // Default player2 to Ironsides
}

Game::~Game() {
    delete player1;             // Clean up player1
    delete player2;             // Clean up player2
}

void Game::displayRules() {
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

void Game::selectMap(vector<vector<char>>& grid) {
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

void Game::start() {
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

bool Game::isValidPlacement(Player& player, int x, int y, int length, char direction) {
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

void Game::printGrid(const vector<vector<char>>& grid) {
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

void Game::selectMode() {
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

void Game::selectCaptain(Player*& player) {
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

void Game::generateShatteredSea(vector<vector<char>>& grid) {
    // Randomly scatter islands across the grid
    int numIslands = rand() % 15 + 5; // Between 5 and 19 islands
    for (int i = 0; i < numIslands; ++i) {
        int x = rand() % GRID_SIZE;   // Random row
        int y = rand() % GRID_SIZE;   // Random column
        grid[x][y] = ISLAND;          // Place island
    }
}
