//Lichkovaha Daniil
//st129351@student.spbu.ru
//SkipList
#include 'WhatLevel.h'

int WhatLvl(int mx_lvl) {
    int lvl = 0;
    bool c_flip = rand() % 2; // coin flip for 0 or 1 result
    while (c_flip == 1 && lvl < mx_lvl) {
        lvl++;
    }
    return lvl;
}