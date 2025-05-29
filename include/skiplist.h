//Lichkovaha Daniil
//st129351@student.spbu.ru
//SkipList
#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <vector>
#include "WhatLevel.h"

template <typename T>
class SkipList {

private:
    // realization nodes
    struct Node {
        T value;

        // vectors for pointers on prev and next nodes
        std::vector<Node*> prev;
        std::vector<Node*> next;
        // the level of the nodes
        int lvl;

        Node(const T& v, int l) : value(v), lvl(l) {
            prev.resize(lvl + 1, nullptr); // how many connections to the left
            next.resize(lvl + 1, nullptr); // how many to the right
        }
    };

    Node *head;
    Node* tail;
    int mx_lvl;
    int curr_mx_lvl;
    float prob;

public:
    // iterator realization (analogy - smart ptr)
    class Iterator {
    private:
        Node* ptr;
    public:
        // if call without args p = nullptr
        Iterator(Node* p = nullptr) : ptr(p) {}
        
        // dereference 
        T& operator*() {
            return ptr->value;
        }
        // with const
        const T& operator*() const {
            return ptr->value;
        }

        // ++iter
        Iterator& operator++() {
            ptr = ptr->next[0];
            return *this;
        }
        // iter++
        Iterator& operator++(int) {
            Iterator iter = *this;
            ++(*this);
            return iter;
        }

        bool operator==(const Iterator& other) const { // last const - curr iter is const now
            return ptr == other.ptr;
        }
        bool operator!=(const Iterator& other) const {
            return ptr != other.ptr;
        }

        Node* GetNode() {
            return ptr;
        }
    };
    
    // SkipList()
};

#endif