#ifndef INCLUDED_ACTOR
#define INCLUDED_ACTOR

#include <string>
#include <vector>
#include "utilities.h"
#include <utility>
#include <queue>
#include <tuple>
// forward declarations
class GameObject;
class Weapon;
class Dungeon;

////////////////////////////////////////////////////////////////////////////////
/// BASE CLASS : Actor
////////////////////////////////////////////////////////////////////////////////

class Actor {
public:
    // constructor and destructor
    Actor(std::string name, int hitPoints, int maxHitPoints, int armorPoints, int strengthPoints, int dexterityPoints, Weapon* initalWeapon);
    virtual ~Actor();
    
    // utility functions ///////////////////////////////////
    void generateRandomCoordinates();
    
    // attacking methods ///////////////////////////////////
    std::string attack(Actor* other);
    bool inflictDamage(Actor* other);
    bool useMagicFangs(Actor* other);
    
    // getters and setters //////////////////////////////////
    Weapon* getCurrentWeapon(){return m_currentWeapon;}
    void setCurrentWeapon(Weapon* newWeapon){m_currentWeapon = newWeapon;}
    std::string getName() {return m_name;}
    void changeWeapon(Weapon* newWeapon);
    int getColumn() {return m_column;}
    void setColumn(int column) {m_column = column;}
    int getRow() {return m_row;}
    void setRow(int row) {m_row = row;}
    int getHitPoints() {return m_hitPoints;}
    void changeHitPoints(int change) {
        // ensure m_hitPoints does not exceed maximum
        if (m_hitPoints + change < m_maxHitPoints) m_hitPoints+=change;
        else m_hitPoints = m_maxHitPoints;
    }
    void setHitPoints(int newHitPoints) {
        m_hitPoints = newHitPoints;
    }
    void changeMaxHitPoints(int change) {m_maxHitPoints+=change;}
    void setMaxHitPoints(int newMaxHitPoints) {
        m_maxHitPoints = newMaxHitPoints;
    }
    int getArmorPoints(){return m_armorPoints;}
    void changeArmorPoints(int change){
        // ensure m_armorPoints does not exceed maximum
        if (m_armorPoints + change < 99) m_armorPoints+=change;
        else m_armorPoints = 99;
    }
    int getStrengthPoints(){return m_strengthPoints;}
    void changeStrengthPoints(int change){
        // ensure m_strengthPoints does not exceed maximum
        if (m_strengthPoints + change < 99) m_strengthPoints+=change;
        else m_strengthPoints = 99;
    }
    void setStrengthPoints(int newStrength) {
        m_strengthPoints = newStrength;
    }
    int getDexterityPoints() {return m_dexterityPoints;}
    void changeDexterityPoints(int change){
        // ensure m_dexterityPoints does not exceed maximum
        if (m_dexterityPoints + change < 99) m_dexterityPoints+=change;
        else m_dexterityPoints = 99;
    }
    int getSleepTime(){return m_sleepTime;}
    void setSleepTime(int change){m_sleepTime=change;}
private:
    std::string m_name;
    int m_column;
    int m_row;
    int m_hitPoints;
    int m_maxHitPoints;
    int m_armorPoints;
    int m_strengthPoints;
    int m_dexterityPoints;
    Weapon* m_currentWeapon;
    int m_sleepTime;
};

////////////////////////////////////////////////////////////////////////////////
/// Player
////////////////////////////////////////////////////////////////////////////////

class Player : public Actor {
public:
    Player();
    ~Player();
    
    // utility
    bool hasGoldenIdol();
    
    // moves ///////////////////////////////////
    void printInventory();
    std::string addToInventory(GameObject* newItem);
    std::string readScroll(char scrollChoice, Dungeon* dungeon);
    std::string findWeapon(char weaponChoice);
private:
    std::vector<GameObject*> m_inventory;
};

////////////////////////////////////////////////////////////////////////////////
/// Monsters
////////////////////////////////////////////////////////////////////////////////

/// base class /////////////////////////////////////////////////////////////////////////////
class Monster : public Actor {
public:
    Monster(std::string name, int hitPoints, int armorPoints, int strengthPoints, int dexterityPoints, char representation, Weapon* initalWeapon);
    
    // utility
    char getRepresentation(){return m_representation;}
    virtual GameObject* die() = 0;
    
    
    // moves /////////////////////////////////////////////////
    virtual std::string move(char** dungeonGrid, Player* player) = 0;
    std::string getDumbMove(Player* player, int distance);
    std::string moveMonster(std::string direction, char** dungeon, Player* player);
private:
    char m_representation;
};

/// sub classes /////////////////////////////////////////////////////////////////////////////

class Bogeyman : public Monster {
public:
    Bogeyman();
    virtual std::string move(char** dungeonGrid, Player* player);
    virtual GameObject* die();
};

class Snakewoman : public Monster {
public:
    Snakewoman();
    virtual std::string move(char** dungeonGrid, Player* player);
    virtual GameObject* die();
};

class Dragon : public Monster {
public:
    Dragon();
    virtual std::string move(char** dungeonGrid, Player* player);
    virtual GameObject* die();
};

class Goblin : public Monster {
public:
    Goblin(int smellDistance);
    virtual std::string move(char** dungeonGrid, Player* player);
    virtual GameObject* die();
//    bool goblinRecursion(char** dungeonGrid, int currentRow, int currentColumn, int smellDistance, int pathLength);
    bool goblinRecursion (char** dungeonGrid, std::queue<int>* pathDistanceQueue, std::queue<std::pair<int, int> >* coordinateQueue);
    bool checkNeighborsForEndPoint(int row, int column);
private:
    int m_smellDistance;
    int m_pathLength;
    std::pair<int, int> m_nextMove;
};



#endif /* INCLUDED_ACTOR */
