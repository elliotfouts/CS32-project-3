#ifndef INCLUDED_DUNGEON
#define INCLUDED_DUNGEON
// dependencies ///////////
#include <vector>
#include "globals.h"
#include "gameObject.h"
class Player;
class Monster;

class Dungeon {
public:
    // constructors and destructors ////////////////////
    Dungeon(int rows, int columns, int goblinSmellDistance);
    ~Dungeon();
    
    // playing the dungeon ////////////////////
    void incrementLevel(){m_level++;};
    void moveMonsters();
    bool checkForGoldenIdol();
    
    // building the dungeon ////////////////////
    void generateDungeon();
    int generateRooms(int x, int y, int width, int height);
    void generateObjectVector();
    void generateMonsterVector();
    void generatePlayerCoordinates();
    void generateMonsterCoordinates(Monster* monster);
    GameObject* chooseRandomObject();
    Monster* generateRandomMonster();
    
    // displaying the dungeon //////////////////
    void display();
    void placeInDungeon();
    void printDungeon();
    
    // player moves ////////////////////
    Player* getPlayer() {return m_player;}
    void movePlayer(const char direction);
    void pickUpObject();
    void openInventory();
    void openScrollInventory();
    void openWeaponInventory();
    bool moveToNextLevel();
    void playerAttack(char direction);
    void cheat();
    
private:
    char m_dungeonGrid[MAXROWS][MAXCOLUMNS];
    char** m_displayGrid;
    const int M_ROWS;
    const int M_COLUMNS;
    int m_level;
    int m_goblinSmellDistance;
    Player* m_player;
    std::vector<GameObject*> m_objectVector;
    std::vector<Monster*> m_monsterVector;
    std::vector<std::string> m_messages;
};

#endif /* INCLUDED_DUNGEON */
