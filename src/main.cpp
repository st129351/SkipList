//Lichkovaha Daniil
//st129351@student.spbu.ru
//SkipList
#include <iostream>
#include "include/SkipList.h"

int main() {
    SkipList<int> lst;

    for (int i = 0; i <= 10; i++) {
        lst.insert(i);
        if (i % 2 == 0) {
            lst.erase(i);
        }
    }
    std::cout << lst << std::endl;
    std::cout << lst.size() << std::endl;
    return 0;
}