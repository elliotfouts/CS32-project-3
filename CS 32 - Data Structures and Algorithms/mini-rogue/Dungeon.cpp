// dependencies
#include "Dungeon.h"
#include "utilities.h"
#include "Actor.h"
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
/// constructors and destructors
////////////////////////////////////////////////////////////////////////////////

Dungeon::Dungeon(int rows, int columns, int goblinSmellDistance)
:M_ROWS(rows), M_COLUMNS(columns), m_level(0), m_goblinSmellDistance(goblinSmellDistance)
{
    m_player = new Player();
    // allocate memory for grid that will contain actors and gameObjects
    m_displayGrid = new char*[M_ROWS];
    for(int i = 0; i < M_ROWS; ++i)
        m_displayGrid[i] = new char[M_COLUMNS];
    
}

Dungeon::~Dungeon() {
    // deallocate memory for player
    delete m_player;
    m_player = nullptr;
    // deallocate memory for grid
    for (int i = 0; i < M_ROWS; i++) {
        delete [] m_displayGrid[i];
    }
    delete [] m_displayGrid;
    m_displayGrid = nullptr;
    // deallocate memory for monsters
    for (int i = 0; i < m_monsterVector.size(); i++)
        delete m_monsterVector[i];
    // deallocare memory for game objects
    for (int i = 0; i < m_objectVector.size(); i++)
        delete m_objectVector[i];
}

////////////////////////////////////////////////////////////////////////////////
/// building the dungeon
////////////////////////////////////////////////////////////////////////////////

void Dungeon::generateDungeon() {
    // initially fill the dungeon up completely with # signs
    for (int i = 0; i < M_ROWS; i++) {
        for (int j = 0; j < M_COLUMNS; j++)
            m_dungeonGrid[i][j] = '#';
    }
    // put rooms in the dungeon
    int numberOfRooms = generateRooms(randInt(1, 10), randInt(1, 10), randInt(15, 20), randInt(4, 6));
    // ensure that there are at least 4 rooms in the dungeon
    while (numberOfRooms < 4) {
        for (int i = 0; i < M_ROWS; i++) {
            for (int j = 0; j < M_COLUMNS; j++)
                m_dungeonGrid[i][j] = '#';
        }
        numberOfRooms = generateRooms(randInt(1, 10), randInt(1, 10), randInt(15, 20), randInt(4, 6));
    }
    // generate random coordinates for stairwell / goldenIdol
    int staircaseRow = randInt(0, MAXROWS - 1);
    int staircaseColumn = randInt(0, MAXCOLUMNS - 1);
    while (m_dungeonGrid[staircaseRow][staircaseColumn] != ' ') {
        staircaseRow = randInt(0, MAXROWS - 1);
        staircaseColumn = randInt(0, MAXCOLUMNS - 1);
    }
    // if level is less than four then add a staircase, otherwise add a golden idol
    if (m_level < 4)
        m_dungeonGrid[staircaseRow][staircaseColumn] = '>';
    else {
        GameObject* theGoldenIdol = new GoldenIdol();
        theGoldenIdol -> setRow(staircaseRow);
        theGoldenIdol -> setColumn(staircaseColumn);
        m_objectVector.push_back(theGoldenIdol);
    }
    // generate vector of gameObjects
    generateObjectVector();
    // generate vector of monsters
    generateMonsterVector();
    // generate starting coordinates for player
    generatePlayerCoordinates();
}

int Dungeon::generateRooms(int column, int row, int width, int height) {
    // base case (checks if the room to be generated will go off the right of the grid, off the bottom, off the top, or off the left)
    if (MAXROWS - height - row - 1 < 0 || MAXCOLUMNS - width - column - 1 < 0 || column < 1 || row < 1) {
        return 0;
    }
    // make sure rooms are not overlapping
    for (int i = row; i < height + row + 1; i++) {
        for (int j = column; j < width + column + 1; j++)
            if (m_dungeonGrid[i][j]==' ')
                return 0;
    }
    // actually generate the room described by the parameters
    for (int i = row; i < height + row; i++) {
        for (int j = column; j < width + column; j++) {
            m_dungeonGrid[i][j] = ' ';
        }
    }
    int numberOfRooms = 1;
    // recursive call to generate room below
    int belowCorridorColumn = randInt(column, column + width - 1);
    int belowCorridorLength = randInt(2, 4);
    int belowCorridorRow = row + height + belowCorridorLength;
    int belowRoomHeight = randInt(4, 7);
    int belowRoomWidth = randInt(10, 25);
    int isValidRoomBelow = generateRooms(belowCorridorColumn, belowCorridorRow, belowRoomWidth, belowRoomHeight);
    numberOfRooms+=isValidRoomBelow;
    // if new room(s) have been created, build a corridor between them
    if (isValidRoomBelow) {
        for (int i = 0; i < belowCorridorLength; i++) {
            m_dungeonGrid[row + height + i][belowCorridorColumn] = ' ';
        }
    }
    // recursive call to generate room to the right
    int rightCorridorRow = randInt(row, row + height - 1);
    int rightCorridorLength = randInt(3, 8);
    int rightRoomHeight = randInt(5, 10);
    int rightRoomWidth = randInt(10, 25);
    int isValidRoomRight = generateRooms(column + width + rightCorridorLength, rightCorridorRow, rightRoomWidth, rightRoomHeight);
    numberOfRooms += isValidRoomRight;
    // if new room(s) have been created, build a corridor between them
    if (isValidRoomRight) {
        for (int i = 0; i < rightCorridorLength; i++) {
            m_dungeonGrid[rightCorridorRow][column + width + i] = ' ';
        }
    }
    // recursive call to generate room above
    int aboveCorridorLength = randInt(5, 6);
    int aboveCorridorColumn = randInt(column, column + width - 1);
    int aboveRoomRow = row - aboveCorridorLength;
    int aboveRoomHeight = randInt(3, 6);
    int aboveRoomWidth = randInt(10, 25);
    int isValidRoomAbove = generateRooms(aboveCorridorColumn, aboveRoomRow, aboveRoomWidth, aboveRoomHeight);
    numberOfRooms += isValidRoomAbove;
    // if new room(s) have been created, build a corridor between them
    if (isValidRoomAbove) {
        for (int i = 0; i < aboveCorridorLength; i++) {
            m_dungeonGrid[row - i][aboveCorridorColumn] = ' ';
        }
    }
        return numberOfRooms;
}

void Dungeon::generatePlayerCoordinates() {
    m_player -> generateRandomCoordinates();
    char charAtCoordinate = m_dungeonGrid[m_player -> getRow()][m_player -> getColumn()];
    // ensure that player is not placed on a wall or on another Actor
    while (charAtCoordinate != ' ' && charAtCoordinate != ')' && charAtCoordinate != '?') {
        m_player -> generateRandomCoordinates();
        charAtCoordinate = m_dungeonGrid[m_player -> getRow()][m_player -> getColumn()];
    }
}

void Dungeon::generateMonsterVector() {
    int numberOfMonsters = randInt(2, 5*(m_level+1)+1);
    for (int i = 0; i < numberOfMonsters; i++)
        m_monsterVector.push_back(generateRandomMonster());
}

Monster* Dungeon::generateRandomMonster(){
    Monster* randomMonster = nullptr;
    int randomChoice;
    // based on the current level, only choose between the monsters appropriate for that level
    switch(m_level) {
        case 0:
        case 1:
            randomChoice = randInt(1, 2);
            break;
        case 2:
            randomChoice = randInt(1, 3);
            break;
        default:
            randomChoice = randInt(1, 4);
    }
    // allocated new memory for the appropriate, randomly chosen monster
    switch (randomChoice) {
        case 1:
            randomMonster = new Snakewoman();
            break;
        case 2:
            randomMonster = new Goblin(m_goblinSmellDistance); 
            break;
        case 3:
            randomMonster = new Bogeyman();
            break;
        case 4:
            randomMonster = new Dragon();
            break;
            
    }
    generateMonsterCoordinates(randomMonster);
    return randomMonster;
}

void Dungeon::generateMonsterCoordinates(Monster* monster) {
    monster -> generateRandomCoordinates();
    char charAtCoordinate = m_dungeonGrid[monster -> getRow()][monster -> getColumn()];
    // ensure that the monster is not placed on the player, the stairwell, the golden idol, or a wall
    while ((charAtCoordinate != ')' && charAtCoordinate != '?' && charAtCoordinate != ' ') || (monster -> getRow() == m_player -> getRow() && monster -> getColumn() == m_player -> getColumn())) {
        monster -> generateRandomCoordinates();
        charAtCoordinate = m_dungeonGrid[monster -> getRow()][monster -> getColumn()];
    }
}

void Dungeon::generateObjectVector() {
    // populate object vector with either 2 or three objects (decided randomly at runtime)
    if (trueWithProbability(0.5)) {
        m_objectVector.push_back(chooseRandomObject());
        m_objectVector.push_back(chooseRandomObject());
    } else {
        m_objectVector.push_back(chooseRandomObject());
        m_objectVector.push_back(chooseRandomObject());
        m_objectVector.push_back(chooseRandomObject());
    }
    
    // generate random coordinates for each object
    for (int i = 0; i < m_objectVector.size(); i++) {
        m_objectVector[i] -> generateRandomCoordinates();
        int currentRow = m_objectVector[i] -> getRow();
        int currentColumn = m_objectVector[i] -> getColumn();
        char charAtCoordinate = m_dungeonGrid[currentRow][currentColumn];
        // ensure that object is placed on an open space
        while(charAtCoordinate != ' ') {
            m_objectVector[i] -> generateRandomCoordinates();
            currentRow = m_objectVector[i] -> getRow();
            currentColumn = m_objectVector[i] -> getColumn();
            charAtCoordinate = m_dungeonGrid[currentRow][currentColumn];
        }
    }
}

GameObject* Dungeon::chooseRandomObject() {
    int randomChoice = randInt(1, 7);
    GameObject* randomObject = nullptr;
    // randomly choose between the allowed objects
    switch (randomChoice) {
        case 1:
            randomObject = new Mace();
            break;
        case 2:
            randomObject = new ShortSword();
            break;
        case 3:
            randomObject = new LongSword();
            break;
        case 4:
            randomObject = new ScrollArmor();
            break;
        case 5:
            randomObject = new ScrollStrength();
            break;
        case 6:
            randomObject = new ScrollHealth();
            break;
        case 7:
            randomObject = new ScrollDexterity();
            break;
    }
    return randomObject;
}


////////////////////////////////////////////////////////////////////////////////
/// displaying the dungeon
////////////////////////////////////////////////////////////////////////////////

void Dungeon::display() {
    placeInDungeon();
    printDungeon();
}

void Dungeon::placeInDungeon() {
    // copies over m_dungeon grid into the grid for tracking actors and game objects
    for (int i = 0; i < M_ROWS; i++)
        for (int j = 0; j < M_COLUMNS; j++)
            m_displayGrid[i][j] = m_dungeonGrid[i][j];
    
    // places gameObjects in display grid
    for (int i = 0; i < m_objectVector.size(); i++)
        m_displayGrid[m_objectVector[i]->getRow()][m_objectVector[i]->getColumn()] = m_objectVector[i] -> getRepresentation();
    //places monsters in display grid
    for (int i = 0; i < m_monsterVector.size(); i++)
        m_displayGrid[m_monsterVector[i]->getRow()][m_monsterVector[i]->getColumn()] = m_monsterVector[i] -> getRepresentation();
    // places Player in display grid
    m_displayGrid[m_player->getRow()][m_player->getColumn()] = '@';
}

void Dungeon::printDungeon() {
    // prints out the display grid
    clearScreen();
    for (int i = 0; i < M_ROWS; i++) {
        for (int j = 0; j < M_COLUMNS; j++)
            cout << m_displayGrid[i][j];
        cout << endl;
    }
    // prints out player stats
    cout << "Dungeon Level: " << m_level;
    cout << ", Hit Points: " << m_player -> getHitPoints();
    cout << ", Armor: " << m_player -> getArmorPoints();
    cout << ", Strength: " << m_player -> getStrengthPoints();
    cout << ", Dexterity: " << m_player -> getDexterityPoints();
    cout << endl << endl;
    // prints out game messages
    for (int i = 0; i < m_messages.size(); i++) {
        cout << m_messages[i] << endl;
    }
    m_messages.clear();
}

////////////////////////////////////////////////////////////////////////////////
/// player moves
////////////////////////////////////////////////////////////////////////////////

void Dungeon::movePlayer(const char direction) {
    bool gainHitPoint = trueWithProbability(0.1);
    if (gainHitPoint)
        m_player -> changeHitPoints(1);
    if (m_player -> getSleepTime() > 0)
        m_player -> setSleepTime(m_player -> getSleepTime() - 1);
    else {
        // based on direction:
        //   - check if player can move in that direction
        //   - if so, update coordinates
        //   - if monster is in the way, attack it
        //   - if wall is in the way, do nothing
        switch (direction) {
            case ARROW_UP: {
                char positionAbove = m_displayGrid[m_player -> getRow() - 1][m_player -> getColumn()];
                if ( positionAbove == ' ' || positionAbove == '?' || positionAbove == ')' || positionAbove == '>' || positionAbove == '&')
                    m_player -> setRow(m_player -> getRow() - 1);
                else if (positionAbove == 'S' || positionAbove == 'B' || positionAbove == 'D' || positionAbove == 'G')
                    playerAttack('N');
                break;
            }
            case ARROW_LEFT: {
                char positionLeft = m_displayGrid[m_player -> getRow()][m_player -> getColumn() - 1];
                if ( positionLeft == ' '|| positionLeft == '?' || positionLeft == ')' || positionLeft == '>'|| positionLeft == '&')
                    m_player -> setColumn(m_player -> getColumn() - 1);
                else if (positionLeft == 'S' || positionLeft == 'B' || positionLeft == 'D' || positionLeft == 'G')
                    playerAttack('W');
                break;
            }
            case ARROW_RIGHT: {
                char positionRight = m_displayGrid[m_player -> getRow()][m_player -> getColumn() + 1];
                if ( positionRight == ' '|| positionRight == '?' || positionRight == ')' || positionRight == '>' || positionRight == '&')
                    m_player -> setColumn(m_player -> getColumn() + 1);
                else if (positionRight == 'S' || positionRight == 'B' || positionRight == 'D' || positionRight == 'G')
                    playerAttack('E');
                break;
            }
            case ARROW_DOWN: {
                char positionBelow = m_displayGrid[m_player -> getRow() + 1][m_player -> getColumn()];
                if ( positionBelow == ' '|| positionBelow == '?' || positionBelow == ')' || positionBelow == '>'|| positionBelow == '&')
                    m_player -> setRow(m_player -> getRow() + 1);
                else if (positionBelow == 'S' || positionBelow == 'B' || positionBelow == 'D' || positionBelow == 'G')
                    playerAttack('S');
                break;
            }
        }
        placeInDungeon();
    }
}

void Dungeon::playerAttack(char direction) {
    int rowCoordinate = 0;
    int columnCoordinate = 0;
    // based on direction, save coordinates of the monster which player is attacking
    switch (direction) {
        case 'N':{
            rowCoordinate = m_player -> getRow() - 1;
            columnCoordinate = m_player -> getColumn();
            break;
        }
        case 'S': {
            rowCoordinate = m_player -> getRow() + 1;
            columnCoordinate = m_player -> getColumn();
            break;
        }
        case 'E': {
            rowCoordinate = m_player -> getRow();
            columnCoordinate = m_player -> getColumn() + 1;
            break;
        }
        case 'W': {
            rowCoordinate = m_player -> getRow();
            columnCoordinate = m_player -> getColumn() - 1;
            break;
        }
    }
    // find the monster that the player is attacking
    for (int i = 0; i < m_monsterVector.size(); i++) {
        // attack the correct monster
        if (m_monsterVector[i] -> getRow() == rowCoordinate && m_monsterVector[i]->getColumn() == columnCoordinate) {
            std::string message = m_player -> attack(m_monsterVector[i]);
            // if a monster is dead, remove it from the dungeon
            if (m_monsterVector[i]->getHitPoints() <= 0) {
                GameObject* droppedItem = m_monsterVector[i] -> die();
                if (droppedItem != nullptr) {
                    droppedItem -> setRow(m_monsterVector[i]->getRow());
                    droppedItem -> setColumn(m_monsterVector[i]->getColumn());
                    if (m_dungeonGrid[droppedItem -> getRow()][droppedItem->getColumn()] == ' ')
                        m_objectVector.push_back(droppedItem);
                    else
                        delete droppedItem;
                }
                delete m_monsterVector[i];
                m_monsterVector.erase(m_monsterVector.begin() + i);
            }
            m_messages.push_back(message);
        }
    }
}

void Dungeon::pickUpObject() {
    int rowOfPlayer = m_player -> getRow();
    int columnOfPlayer = m_player -> getColumn();
    // loop through m_objectVector
    for (int i = 0; i < m_objectVector.size(); i++) {
        int rowOfObject = m_objectVector[i] -> getRow();
        int columnOfObject = m_objectVector[i] -> getColumn();
        // if the player is at the same coordinates as one of the objects
        if (rowOfObject == rowOfPlayer && columnOfObject == columnOfPlayer) {
            // add that object to the inventory
            std::string message = m_player -> addToInventory(m_objectVector[i]);
            if (m_objectVector[i] -> getRepresentation() == '&')
                message += "\nCongratulations, you won!";
            m_messages.push_back(message);
            // remove that object from the dungeon
            m_objectVector.erase(m_objectVector.begin()+i);
        }
    }
}

void Dungeon::openInventory() {
    clearScreen();
    cout << "Inventory:" << endl;
    m_player -> printInventory();
}

void Dungeon::openScrollInventory() {
    openInventory();
    char scrollChoice = getCharacter();
    std::string scrollMessage = m_player -> readScroll(scrollChoice, this);
    if (scrollMessage != "You can't read a ") // makes sure not to print anything if an invalid key was pressed
        m_messages.push_back(scrollMessage);
}

void Dungeon::openWeaponInventory() {
    openInventory();
    char weaponChoice = getCharacter();
    std::string weaponSelected = m_player -> findWeapon(weaponChoice);
    if (weaponSelected == "mace" || weaponSelected == "short sword" || weaponSelected == "long sword" || weaponSelected == "magic axe" || weaponSelected == "magic fangs of sleep")
        m_messages.push_back("You are wielding " + weaponSelected);
    else if (weaponSelected == "") // checking if they clicked an invalid key
        return;
    else
        m_messages.push_back("You can't wield " + weaponSelected);
}

bool Dungeon::moveToNextLevel() {
    // get character where the player is
    char charAtCoordinate = m_dungeonGrid[m_player -> getRow()][m_player -> getColumn()];
    // if the player is on the stairwell
    if (charAtCoordinate == '>') {
        for (int i = 0; i < m_objectVector.size(); i++) // empty object vector for next level
            delete m_objectVector[i];
        m_objectVector.clear();
        for (int i = 0; i < m_monsterVector.size(); i++) // empty monster vector for next level
            delete m_monsterVector[i];
        m_monsterVector.clear();
        return true;
    } else {
        return false;
    }
}

void Dungeon::cheat() {
    m_player -> setMaxHitPoints(50);
    m_player -> setHitPoints(50);
    m_player -> setStrengthPoints(9);
}

////////////////////////////////////////////////////////////////////////////////
/// playing the dungeon
////////////////////////////////////////////////////////////////////////////////

bool Dungeon::checkForGoldenIdol() {
    return m_player -> hasGoldenIdol();
}

void Dungeon::moveMonsters() {
    for (int i = 0; i < m_monsterVector.size(); i++) {
        // makes sure sleeping monsters don't move
        if (m_monsterVector[i] -> getSleepTime() > 0)
            m_monsterVector[i] -> setSleepTime(m_monsterVector[i]->getSleepTime() - 1);
        else {
            std::string message = m_monsterVector[i] -> move(m_displayGrid, m_player);
            // if monster does not move, nothing should be printed
            if (message != "")
                m_messages.push_back(message);
            // updates display grid so future monster moves have the most up-to-date grid
            placeInDungeon();
        }
    }
}
