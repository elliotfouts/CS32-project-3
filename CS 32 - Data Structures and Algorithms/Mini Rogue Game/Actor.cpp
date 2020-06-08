#include <string>
#include <iostream>
#include <queue>
#include <utility>
#include "Actor.h"
#include "utilities.h"
#include "globals.h"
#include "gameObject.h"
#include "Dungeon.h"
#include <cstdlib>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
/// BASE CLASS : Actor
////////////////////////////////////////////////////////////////////////////////

Actor::Actor(std::string name, int hitPoints, int maxHitPoints, int armorPoints, int strengthPoints, int dexterityPoints, Weapon* initialWeapon)
:m_name(name), m_hitPoints(hitPoints), m_maxHitPoints(maxHitPoints), m_armorPoints(armorPoints), m_strengthPoints(strengthPoints), m_dexterityPoints(dexterityPoints), m_currentWeapon(initialWeapon), m_sleepTime(0)
{}

Actor::~Actor() {
    delete m_currentWeapon;
}

void Actor::generateRandomCoordinates() {
    m_column = randInt(0, MAXCOLUMNS - 1);
    m_row = randInt(0, MAXROWS - 1);
}

void Actor::changeWeapon(Weapon* newWeapon) {
    m_currentWeapon = newWeapon;
}

std::string Actor::attack(Actor *other) {
    int attackerPoints = getDexterityPoints() + m_currentWeapon -> getDexterityBonus();
    int defenderPoints = other -> getDexterityPoints() + other -> getArmorPoints();
    std::string message;
    if (randInt(1, attackerPoints) >= randInt(1, defenderPoints)) {
        bool isDead = inflictDamage(other);
        message = getName() + m_currentWeapon -> getActionString() + other -> getName() + " and hits";
        if (isDead)
            message += ", dealing a final blow.";
        else if (m_currentWeapon -> getName() == "magic fangs of sleep") {
            bool isAsleep = useMagicFangs(other);
            if (isAsleep)
                message += ", putting " + other -> getName() + " to sleep";
        }
    } else {
        message = getName() + m_currentWeapon -> getActionString() + other -> getName() + " and misses";
    }
    return message;
}

bool Actor::inflictDamage(Actor *other) {
    int damagePoints = randInt(0, getStrengthPoints() + m_currentWeapon -> getDamageAmount() - 1);
    other -> changeHitPoints(-1 * damagePoints);
    if (other -> getHitPoints() > 0)
        return false;
    else
        return true;
}

bool Actor::useMagicFangs(Actor* other) {
    bool hasPutToSleep = trueWithProbability(0.2);
    if (hasPutToSleep) {
        int newSleepTime = randInt(2, 6);
        if (newSleepTime > other -> getSleepTime())
            other -> setSleepTime(newSleepTime);
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
/// SUBCLASS : Player
////////////////////////////////////////////////////////////////////////////////

Player::Player():Actor("Player", 20, 20, 2, 2, 2, new ShortSword())
{
    generateRandomCoordinates();
    addToInventory(getCurrentWeapon());
}

Player::~Player() {
    for (int i = 0; i < m_inventory.size(); i++) {
        delete m_inventory[i];
        m_inventory[i] = nullptr;
    }
    setCurrentWeapon(nullptr);
}

std::string Player::addToInventory(GameObject* newItem){
    // ensure that inventory size does not exceed 25
    if (m_inventory.size() <= 24) {
        m_inventory.push_back(newItem);
        std::string objectName = newItem -> getName();
         if (objectName == "mace" || objectName == "short sword" || objectName == "long sword" || objectName == "magic axe" || objectName == "magic fangs of sleep" || objectName == "the golden idol")
             return "You pick up " + objectName;
        else
            return "You pick up a scroll called " + objectName;
    } else
        return "Your knapsack is full; you can't pick that up.";
}

void Player::printInventory() {
    char alphabeticalIndex = 'a';
    for (int i = 0; i < m_inventory.size(); i++) {
        // prints each item in the inventory
        std::cout << alphabeticalIndex << ". " << m_inventory[i] -> getName() << endl;
        // increments letter to print out in alphabetical order
        alphabeticalIndex++;
    }
}

std::string Player::readScroll(char scrollChoice, Dungeon* dungeon) {
    char alphabeticalIndex = 'a';
    std::string objectName;
    for (int i = 0; i < m_inventory.size(); i++) {
        if (scrollChoice == alphabeticalIndex) {
            objectName = m_inventory[i] -> getName();
            // dynamic cast to be able to use the useScroll method
            Scroll* scrollToRead = dynamic_cast<Scroll*>(m_inventory[i]);
            // checks if item selected is a scroll
            if (scrollToRead != nullptr) {
                // i didn't want to pass the dungeon pointer to the useScroll method so i took care of the teleportation scroll at this level
                if (scrollToRead -> getName() == "scroll of teleportation")
                    dungeon -> generatePlayerCoordinates();
                else
                    scrollToRead -> useScroll(this);
                std::string scrollMessage = scrollToRead -> getMessage();
                // deallocate memory from used scroll
                delete m_inventory[i];
                m_inventory.erase(m_inventory.begin()+i);
                return scrollMessage;
            }
        }
        // increments letter to print out in alphabetical order
        alphabeticalIndex++;
    }
    // will be returned if they did not select a scroll
    std::string message = "You can't read a " + objectName;
    return message;
}

std::string Player::findWeapon(char weaponChoice) {
    char alphabeticalIndex = 'a';
    std::string objectName;
    for (int i = 0; i < m_inventory.size(); i++) {
        // increments until the user choice matches the alphabetical index i am creating
        if (weaponChoice == alphabeticalIndex) {
            objectName = m_inventory[i] -> getName();
            // dynamic cast to make sure that i am not passing in a nonWeapon to the changeWeapon function
            Weapon* weaponToChoose = dynamic_cast<Weapon*>(m_inventory[i]);
            if (weaponToChoose != nullptr && weaponToChoose != NULL) {
                changeWeapon(weaponToChoose);
                return objectName;
            }
        }
        // increments letter to print out in alphabetical order
        alphabeticalIndex++;
    }
    return objectName;
}

bool Player::hasGoldenIdol() {
    for (int i = 0; i < m_inventory.size(); i++) {
        if (m_inventory[i] -> getRepresentation() == '&')
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// SUBCLASS: Monster
////////////////////////////////////////////////////////////////////////////////

Monster::Monster(std::string name, int hitPoints, int armorPoints, int strengthPoints, int dexterityPoints, char representation, Weapon* initalWeapon)
:Actor(name, hitPoints, hitPoints, armorPoints, strengthPoints, dexterityPoints, initalWeapon), m_representation(representation){}

std::string Monster::getDumbMove(Player *player, int distance) {
    int verticalDistance = player -> getRow() - getRow();
    int horizontalDistance = player -> getColumn() - getColumn();
    // checks if player is within the smell range
    if (abs(verticalDistance) + abs(horizontalDistance) <= distance) {
        std::string direction = "";
        if (verticalDistance < 0)
            direction += "north";
        else if (verticalDistance > 0)
            direction += "south";
        if (horizontalDistance < 0)
            direction += "west";
        else if (horizontalDistance > 0)
            direction += "east";
        // this will be the general direction of the player
        return direction;
    } else
        return "not found"; // if player is not within smelling distance
}

std::string Monster::moveMonster(std::string direction, char** dungeon, Player* player) {
    std::string message = "";
    // if the character in the direction of player is :
    //    - player: attack player
    //    - open spot (or spot occupied by a gameObject/staircase): move toward player
    //    - wall/other monster: do nothing
    if (direction == "north") {
        char northSpot = dungeon[getRow() - 1][getColumn()];
        if (northSpot == ' ' || northSpot == ')' || northSpot == '?' || northSpot == '>' || northSpot == '&')
            setRow(getRow()-1);
        else if (northSpot == '@')
            message = attack(player);
    } else if (direction == "south") {
        char southSpot = dungeon[getRow() + 1][getColumn()];
        if (southSpot == ' ' || southSpot == ')' || southSpot == '?' || southSpot == '>' || southSpot == '&')
            setRow(getRow() + 1);
        else if (southSpot == '@')
            message = attack(player);
    } else if (direction == "east") {
        char eastSpot = dungeon[getRow()][getColumn() + 1];
        if (eastSpot == ' ' || eastSpot == ')' || eastSpot == '?' || eastSpot == '>' || eastSpot == '&')
            setColumn(getColumn() + 1);
        else if (eastSpot == '@')
            message = attack(player);
    } else if (direction == "west") {
        char westSpot = dungeon[getRow()][getColumn() - 1];
        if (westSpot == ' ' || westSpot == ')' || westSpot == '?' || westSpot == '>' || westSpot == '&')
            setColumn(getColumn() - 1);
        else if (westSpot == '@')
            message = attack(player);
    } else if (direction == "northwest") {
        char northSpot = dungeon[getRow() - 1][getColumn()];
        char westSpot = dungeon[getRow()][getColumn() - 1];
        if (northSpot == '@')
            message = attack(player);
        else if (westSpot == '@')
            message = attack(player);
        else if (northSpot == ' ' || northSpot == ')' || northSpot == '?' || northSpot == '>' || northSpot == '&')
            setRow(getRow()-1);
        else if (westSpot == ' ' || westSpot == ')' || westSpot == '?' || westSpot == '>' || westSpot == '&')
            setColumn(getColumn() - 1);
    } else if (direction == "southwest") {
        char westSpot = dungeon[getRow()][getColumn() - 1];
        char southSpot = dungeon[getRow() + 1][getColumn()];
        if (southSpot == '@')
            message = attack(player);
        else if (westSpot == '@')
            message = attack(player);
        else if (southSpot == ' ' || southSpot == ')' || southSpot == '?' || southSpot == '>' || southSpot == '&')
            setRow(getRow() + 1);
        else if (westSpot == ' ' || westSpot == ')' || westSpot == '?' || westSpot == '>' || westSpot == '&')
            setColumn(getColumn() - 1);
    } else if (direction == "northeast") {
        char northSpot = dungeon[getRow() - 1][getColumn()];
        char eastSpot = dungeon[getRow()][getColumn() + 1];
        if (northSpot == '@')
            message = attack(player);
        else if (eastSpot == '@')
            message = attack(player);
        else if (northSpot == ' ' || northSpot == ')' || northSpot == '?' || northSpot == '>' || northSpot == '&')
            setRow(getRow()-1);
        else if (eastSpot == ' ' || eastSpot == ')' || eastSpot == '?' || eastSpot == '>' || eastSpot == '&')
            setColumn(getColumn() + 1);
    } else if (direction == "southeast") {
        char southSpot = dungeon[getRow() + 1][getColumn()];
        char eastSpot = dungeon[getRow()][getColumn() + 1];
        if (southSpot == '@')
            message = attack(player);
        else if (eastSpot == '@')
            message = attack(player);
        else if (eastSpot == ' ' || eastSpot == ')' || eastSpot == '?' || eastSpot == '>' || eastSpot == '&')
            setColumn(getColumn() + 1);
        else if (southSpot == ' ' || southSpot == ')' || southSpot == '?' || southSpot == '>' || southSpot == '&')
            setRow(getRow() + 1);
    }
    return message;
}

////////////////////////////////////////////////////////////////////////////////
/// SUBCLASS OF MONSTER: Bogeyman
////////////////////////////////////////////////////////////////////////////////

Bogeyman::Bogeyman()
:Monster("the Bogeyman", randInt(5, 10), 2, randInt(2, 3), randInt(2, 3), 'B', new ShortSword())
{}

std::string Bogeyman::move(char** dungeonGrid, Player* player) {
    std::string direction = getDumbMove(player, 5);
    if (direction == "not found")
        return "";
    else
        return moveMonster(direction, dungeonGrid, player);;
}

GameObject* Bogeyman::die() {
    bool dropAxe = trueWithProbability(0.1);
    if (dropAxe)
        return new MagicAxe();
    else
        return nullptr;
}


////////////////////////////////////////////////////////////////////////////////
/// SUBCLASS OF MONSTER: Snakewoman
////////////////////////////////////////////////////////////////////////////////

Snakewoman::Snakewoman()
:Monster("the Snakewoman", randInt(3, 6), 3, 2, 3, 'S', new MagicFangs())
{}

std::string Snakewoman::move(char** dungeonGrid, Player* player) {
    std::string direction = getDumbMove(player, 3);
    if (direction == "not found")
        return "";
    else
        return moveMonster(direction, dungeonGrid, player);
}

GameObject* Snakewoman::die() {
    bool dropMagicFangs = trueWithProbability(1.0/3.0);
    if (dropMagicFangs)
        return new MagicFangs();
    else
        return nullptr;
}


////////////////////////////////////////////////////////////////////////////////
/// SUBCLASS OF MONSTER: Dragon
////////////////////////////////////////////////////////////////////////////////

Dragon::Dragon()
:Monster("the Dragon", randInt(20, 25), 4, 4, 4, 'D',new LongSword())
{}

std::string Dragon::move(char** dungeonGrid, Player* player) {
    // allows dragon to gain hitPoints with random probability
    bool gainHitPoint = trueWithProbability(0.1);
    if (gainHitPoint)
        changeHitPoints(1);
    char northSpot = dungeonGrid[getRow() - 1][getColumn()];
    char eastSpot = dungeonGrid[getRow()][getColumn() + 1];
    char southSpot = dungeonGrid[getRow() + 1][getColumn()];
    char westSpot = dungeonGrid[getRow()][getColumn() - 1];
    // checks if player is in any adjacent square and attacks if so
    if (northSpot == '@')
        return attack(player);
    else if (eastSpot == '@')
        return attack(player);
    else if (southSpot == '@')
        return attack(player);
    else if(westSpot == '@')
        return attack(player);
    else
        return "";
}

GameObject* Dragon::die() {
    // chooses randomly between the 5 different scroll types 
    int randomChoice = randInt(1, 5);
    switch (randomChoice) {
        case 1:
            return new ScrollArmor();
        case 2:
            return new ScrollHealth();
        case 3:
            return new ScrollStrength();
        case 4:
            return new ScrollDexterity();
        default:
            return new ScrollTeleportation;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// SUBCLASS OF MONSTER: Goblin
////////////////////////////////////////////////////////////////////////////////

Goblin::Goblin(int smellDistance)
:Monster("the Goblin", randInt(15, 20), 1, 3, 1, 'G', new ShortSword()), m_smellDistance(smellDistance)
{}

std::string Goblin::move(char** dungeonGrid, Player* player) {
    // generate a copy of dungeonGrid to keep track of spots visited
    char ** goblinGrid = new char*[MAXROWS];
    for(int i = 0; i < MAXROWS; ++i)
        goblinGrid[i] = new char[MAXCOLUMNS];
    for (int i = 0; i < MAXROWS; i++)
        for (int j = 0; j < MAXCOLUMNS; j++)
            goblinGrid[i][j] = dungeonGrid[i][j];
    // call recursive goblin function
    std::queue<int> pathDistanceQueue;
    pathDistanceQueue.push(0);
    std::queue<std::pair<int, int> > coordinateQueue;
    std::pair<int, int> playerCoordindates = {player -> getRow(), player -> getColumn()};
    coordinateQueue.push(playerCoordindates);
    bool canFindPlayer = goblinRecursion(goblinGrid, &pathDistanceQueue, &coordinateQueue);
    std::string message = "";
    if (canFindPlayer) {
        std::pair<int, int> nextMove = coordinateQueue.front();
        if (getRow() - 1 == nextMove.first)
            message = moveMonster("north", dungeonGrid, player);
        else if (getColumn() + 1 == nextMove.second)
            message = moveMonster("east", dungeonGrid, player);
        else if (getRow() + 1 == nextMove.first)
            message = moveMonster("south", dungeonGrid, player);
        else if (getColumn() - 1 == nextMove.second)
            message = moveMonster("west", dungeonGrid, player);
    }
    // deallocate memory for the goblin grid
    for (int i = 0; i < MAXROWS; i++)
        delete [] goblinGrid[i];
    delete [] goblinGrid;
    return message;
}

bool Goblin::goblinRecursion (char** dungeonGrid, std::queue<int>* pathDistanceQueue, std::queue<std::pair<int, int> >* coordinateQueue) {
    // base case if player is out of range or is no path exists
    if (coordinateQueue -> empty() || pathDistanceQueue -> front() > m_smellDistance)
        return false;
    else {
        std::pair<int, int> currentSpot = coordinateQueue -> front();
        char charAtCurrentSpot = dungeonGrid[currentSpot.first][currentSpot.second];
        // if current spot is invalid then move to next coordinate in the queue
        if (charAtCurrentSpot == '#' || charAtCurrentSpot == 'G' || charAtCurrentSpot == 'S' || charAtCurrentSpot == 'D' || charAtCurrentSpot == 'B' || charAtCurrentSpot == '.') {
            coordinateQueue -> pop();
            pathDistanceQueue -> pop();
            return goblinRecursion(dungeonGrid, pathDistanceQueue, coordinateQueue);
        }
        // base case if player has been found
        else if (checkNeighborsForEndPoint(currentSpot.first, currentSpot.second))
            return true;
        else {
            // mark current spot as visited
            dungeonGrid[currentSpot.first][currentSpot.second] = '.';
            std::pair<int, int> northNeighbor = {currentSpot.first - 1, currentSpot.second};
            std::pair<int, int> southNeighbor = {currentSpot.first + 1, currentSpot.second};
            std::pair<int, int> eastNeighbor = {currentSpot.first, currentSpot.second + 1};
            std::pair<int, int> westNeighbor = {currentSpot.first, currentSpot.second - 1};
            // add all neighbors to coordindate queue
            coordinateQueue -> push(northNeighbor);
            coordinateQueue -> push(southNeighbor);
            coordinateQueue -> push(eastNeighbor);
            coordinateQueue -> push(westNeighbor);
            // push corresponding path distances onto the queue
            pathDistanceQueue -> push(pathDistanceQueue -> front() + 1);
            pathDistanceQueue -> push(pathDistanceQueue -> front() + 1);
            pathDistanceQueue -> push(pathDistanceQueue -> front() + 1);
            pathDistanceQueue -> push(pathDistanceQueue -> front() + 1);
            // remove current spot from both queues
            coordinateQueue -> pop();
            pathDistanceQueue -> pop();
            //recursive call
            return goblinRecursion(dungeonGrid, pathDistanceQueue, coordinateQueue);
        }
    }
}

bool Goblin::checkNeighborsForEndPoint(int row, int column) {
    // check north neighbor
    if (row - 1 == getRow() && column == getColumn())
        return true;
    // east neighbor
    else if (row == getRow() && column + 1 == getColumn())
        return true;
    // south neighbor
    else if (row + 1 == getRow() && column == getColumn())
        return true;
    // west neighbor
    else if (row == getRow() && column - 1 == getColumn())
        return true;
    else
        return false;
}

GameObject* Goblin::die(){
    bool willDropItem = trueWithProbability(1.0/3.0);
    if (willDropItem) {
        bool willDropMagicAxe = trueWithProbability(0.5);
        if (willDropMagicAxe)
            return new MagicAxe;
        else
            return new MagicFangs;
    }
    return nullptr;
    
}
