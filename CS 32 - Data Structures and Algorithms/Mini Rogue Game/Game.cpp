// Game.cpp
#include <iostream>
#include "Game.h"
#include "utilities.h"
#include "Dungeon.h"
#include "Actor.h"
using namespace std;

Game::Game(int goblinSmellDistance):m_goblinSmellDistance(goblinSmellDistance), m_isGameOver(false)
{
    m_dungeon = new Dungeon(MAXROWS, MAXCOLUMNS, goblinSmellDistance);
}

Game::~Game() {
    delete m_dungeon;
    m_dungeon = nullptr;
}

void Game::play()
{
    Player* player = m_dungeon -> getPlayer();
    while (!m_isGameOver) {
        m_isGameOver = playLevel();
        // make sure player has not died in the level
        if (player -> getHitPoints() < 0) break;
        m_dungeon -> incrementLevel();
    }
    cout << "Press q to exit game." << endl;
    char character = getCharacter();
    while (character != 'q')
        character = getCharacter();
    return;
}

bool Game::playLevel() {
    Player* player = m_dungeon -> getPlayer();
    m_dungeon -> generateDungeon();
    m_dungeon -> display();
    bool isLevelOver = false;
    while(player -> getHitPoints() > 0 && !isLevelOver && !m_isGameOver) {
        isLevelOver = takePlayerTurn();
        if (m_isGameOver || isLevelOver) break; // will occur if player has quit or if player has advanced to next level
        m_dungeon -> moveMonsters();
        m_isGameOver = m_dungeon -> checkForGoldenIdol();
        m_dungeon -> display();
    }
    if (player -> getHitPoints() <= 0 || m_isGameOver)
        return true;
    else
        return false;
}

bool Game::takePlayerTurn() {
    
    char currentMove = getCharacter();
    // call move if key pressed was a valid move
    if (currentMove == ARROW_UP || currentMove == ARROW_DOWN || currentMove == ARROW_LEFT || currentMove == ARROW_RIGHT)
        m_dungeon -> movePlayer(currentMove);
    else if (currentMove == 'g')
        m_dungeon -> pickUpObject();
    else if (currentMove == 'i') {
        m_dungeon -> openInventory();
        getCharacter();
    }
    else if (currentMove == 'r')
        m_dungeon -> openScrollInventory();
    else if (currentMove == 'w')
        m_dungeon -> openWeaponInventory();
    else if (currentMove == 'c')
        m_dungeon -> cheat();
    else if (currentMove == 'q')
        m_isGameOver = true;
    else if (currentMove == '>') {
        bool IsOnStaircase = m_dungeon -> moveToNextLevel();
        // depending on this return value, a new level will/will not be generated
        return (IsOnStaircase) ? true : false;
    } else if (currentMove == '?') {
        // this allowed me to set a breakpoint and catch infinite loops. If you set a breakpoint during runtime, getCharacter() will infinitely return the '?' character thus ruining the game..............no idea what to do about this
        int doNothing;
    }
    return false;
}
