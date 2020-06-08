#ifndef GAME_INCLUDED
#define GAME_INCLUDED

// You may add data members and other member functions to this class.
class Dungeon;
class Player;
class Game
{
public:
    Game(int goblinSmellDistance);
    ~Game();
    void play();
    bool playLevel();
    bool takePlayerTurn();
private:
    int m_level;
    int m_goblinSmellDistance;
    bool m_isGameOver;
    Dungeon* m_dungeon;
};

#endif // GAME_INCLUDED
