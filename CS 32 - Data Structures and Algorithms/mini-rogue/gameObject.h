//
//  gameObject.hpp
//  project-3
//
//  Created by Elliot Fouts on 5/15/20.
//  Copyright © 2020 CS32. All rights reserved.
//

#ifndef INCLUDED_GAME_OBJECT
#define INCLUDED_GAME_OBJECT

#include <string>

class GameObject {
public:
    GameObject(std::string name, char representation):m_representation(representation), m_name(name){};
    virtual ~GameObject(){};
    void generateRandomCoordinates();
    int getRow(){return m_row;}
    int getColumn(){return m_column;}
    void setRow(int row){m_row = row;}
    void setColumn(int column){m_column = column;}
    char getRepresentation(){return m_representation;}
    std::string getName(){return m_name;}
private:
    char m_representation;
    int m_row;
    int m_column;
    std::string m_name;
};

////////////////////////////////////////////////////////////////////////////////
/// golden idol
////////////////////////////////////////////////////////////////////////////////

class GoldenIdol: public GameObject {
public:
    GoldenIdol():GameObject("the golden idol", '&') {}
};


////////////////////////////////////////////////////////////////////////////////
/// weapons
////////////////////////////////////////////////////////////////////////////////


// base class ///////////////////////
class Weapon: public GameObject {
public:
    Weapon(std::string name, std::string action, int dexterity, int damage)
    :GameObject(name, ')'), m_actionString(action), m_dexterityBonus(dexterity), m_damageAmount(damage){}
    std::string getActionString() {return m_actionString;}
    int getDexterityBonus(){return m_dexterityBonus;}
    int getDamageAmount(){return m_damageAmount;}
private:
    std::string m_actionString;
    int m_dexterityBonus;
    int m_damageAmount;
};

// sub classes ///////////////////////

class Mace: public Weapon {
public:
    Mace():Weapon("mace", " swings mace at ", 0, 2){}
};

class ShortSword: public Weapon {
public:
    ShortSword():Weapon("short sword", " slashes short sword at ", 0, 2){}
};

class LongSword: public Weapon {
public:
    LongSword():Weapon("long sword", " swings long sword at ", 2, 4){}
};

class MagicAxe: public Weapon {
public:
    MagicAxe():Weapon("magic axe", " chops magic axe at ", 5, 5){}
};

class MagicFangs: public Weapon {
public:
    MagicFangs():Weapon("magic fangs of sleep", " strikes magic fangs at ", 3, 2){}
};

////////////////////////////////////////////////////////////////////////////////
/// scrolls
////////////////////////////////////////////////////////////////////////////////

class Player; // forward declaration of player in order to mutate private player members

// base class ///////////////////////
class Scroll: public GameObject {
public:
    Scroll(std::string name, std::string message):GameObject(name, '?'), m_message("You read the scroll called " + name + '\n' + message){};
    virtual void useScroll(Player* player) = 0;
    std::string getMessage(){return m_message;};
private:
    std::string m_message;
};

// sub classes //////////////////////

class ScrollTeleportation: public Scroll {
public:
    ScrollTeleportation():Scroll("scroll of teleportation", "You feel your body wrenched in space and time."){}
    virtual void useScroll(Player* player);
};

class ScrollArmor: public Scroll {
public:
    ScrollArmor():Scroll("scroll of improve armor", "Your armor glows blue."){}
    virtual void useScroll(Player* player);
};

class ScrollStrength: public Scroll {
public:
    ScrollStrength():Scroll("scroll of raise strength", "Your muscles bulge."){}
    virtual void useScroll(Player* player);
};

class ScrollHealth: public Scroll {
public:
    ScrollHealth():Scroll("scroll of enhance health", "You feel your heart beating stronger."){}
    virtual void useScroll(Player* player);
};

class ScrollDexterity: public Scroll {
public:
    ScrollDexterity():Scroll("scroll of enhance dexterity", "You feel like less of a klutz."){}
    virtual void useScroll(Player* player);
};

#endif /* INCLUDED_GAME_OBJECT */
