//
//  gameObject.cpp
//  project-3
//
//  Created by Elliot Fouts on 5/15/20.
//  Copyright © 2020 CS32. All rights reserved.
//

#include "gameObject.h"
#include "Actor.h"
#include "utilities.h"
#include "globals.h"

void GameObject::generateRandomCoordinates() {
    m_column = randInt(0, MAXCOLUMNS - 1);
    m_row = randInt(0, MAXROWS - 1);
}

void ScrollTeleportation::useScroll(Player* player) {
    return;
}

void ScrollArmor::useScroll(Player* player) {
    int armorUpgrade = randInt(1, 3);
    player -> changeArmorPoints(armorUpgrade);
}

void ScrollStrength::useScroll(Player* player) {
    int strengthUpgrade = randInt(1, 3);
    player -> changeStrengthPoints(strengthUpgrade);
}

void ScrollHealth::useScroll(Player* player) {
    int healthUpgrade = randInt(3, 8);
    player -> changeMaxHitPoints(healthUpgrade);
}

void ScrollDexterity::useScroll(Player* player) {
    player -> changeDexterityPoints(1);
}
