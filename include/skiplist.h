//Lichkovaha Daniil
//st129351@student.spbu.ru
//SkipList
#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <initializer_list>
#include "WhatLevel.h"

/**
 * @brief skip list class (container)
 * @tparam using T type for template
 */
template <typename T>
class SkipList {
    // template requirements (using type_traits)
    static_assert(std::is_copy_constructible_v<T>, "'T' must support coppying");
    static_assert(std::is_copy_assignable_v<T>, "'T' must support assignment");
    static_assert(std::is_default_constructible_v<T>, "'T' must support default constructor");

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

    // aliases
    Node* head; // ptr on head node
    Node* tail; // ptr on tail node
    int mx_lvl; // max possible level
    int curr_mx_lvl; // current __//__
    size_t all_size;

public:
    /**
     * @brief iterator realization (analogy - smart ptr)
     * */
    class Iterator {
    private:
        Node* ptr;
    public:
        
        // determine
        using iterator_category = std::forward_iterator_tag; // only increase (++), not --
        using value_type = T;
        using difference_type = std::ptrdiff_t; // dist between 2 ptrs, type (ptrdiff) as size_t
        using pointer = T*;
        using reference = T&;
        using size_type = size_t;

        // if call without args p = nullptr
        Iterator(Node* p = nullptr) : ptr(p) {}
        
        // dereference (for values)
        reference operator*() {
            if (!ptr) throw std::out_of_range("Null iterator, without dereferencing!");
            return ptr->value;
        }
        // with const
        const reference operator*() const {
            if (!ptr) throw std::out_of_range("Null iterator, without dereferencing!");
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

        // return ptr on value (for methods)
        pointer operator->() {
            if (!ptr) throw std::out_of_range("Null iterator, without accessing!");
            return &ptr->value;
        }
    };
    
    using const_iterator = Iterator; // const-methods already
 
    /**
     * @brief Default constructor
     * @param max_lvl - max level in list, default 16
     */
    explicit SkipList(int max_lvl = 16) : mx_lvl(max_lvl), curr_mx_lvl(0), all_size(0) {
        head = new Node(T(), mx_lvl); // head and tail - zero values and mx_lvls to the right and left
        tail = new Node(T(), mx_lvl);

        for (int i = 0; i <= mx_lvl; i++) {
            head->next[i] = tail;
            tail->prev[i] = head;
            /*
            lvls:
            [2]:      node
                     /   \
            [1]: head  -  tail
                     \   /
            [0]:      node
            hard picture, i belive i am right...            
            */
        }
    }
    /*explicit for avoid SkipList<int> lst = 1; where 1 is obgect of SkipList
    correct SL<int> lst(1), where 1 is arg */

    /**
     * @brief constructor from init list
     * @param init initial list
     * @param max_lvl max level in list
     */
    SkipList(std::initializer_list<T> init, int max_lvl = 16) : SkipList(max_lvl) {
        for (const auto& val : init) {
            insert(val); // insert next
        }
    }

    /**
     * @brief Copying constructor
     * @param other - other SkipList for copying
     */
    SkipList(const SkipList& other) : mx_lvl(other.mx_lvl), curr_mx_lvl(0), all_size(0) {
        head = new Node(T(), mx_lvl);
        tail = new Node(T(), mx_lvl);

        for (int i = 0; i <= mx_lvl; i++) {
            head->next[i] = tail;
            tail->prev[i] = head;
        }

        for (const auto& val : other) {
            insert(val);
        }
    }

    /**
     * @brief assignment operator
     * @param other - Other Skiplist for copy
     * @return reference on current object
     */
    SkipList& operator=(const SkipList& other) {
        if (this != &other) {
            this->clear();
            for (const auto& val : other) {
                insert(val);
            }
        }
        return *this;
    }

    /**
     * @brief destructor 
     */
    ~SkipList() {
        this->clear();
        delete head;
        delete tail;
    }

    /**
     * @brief insert an item
     * @param val value for insert
     * @return iterator on inserted item
     */
    Iterator insert(const T& val) {
        std::vector<Node*> update(mx_lvl+1, head); // vector of the places for insert
        Node* curr = head;

        // search place for insert
        for (int i = curr_mx_lvl; i >= 0; i--) {
            while (curr->next[i] != tail && curr->next[i]->value < val) {
                curr = curr->next[i]; // last curr on the zero lvl
            }
            update[i] = curr;
        }

        // if need exclude copy:
        
        // curr = curr->next[0];
        // // if item already exist in the 0 lvl
        // if (curr != tail && curr->value == val) {
        //     return Iterator(curr); // initiate ptr in iter
        // }
        int new_lvl = WhatLvl(mx_lvl);
        if (new_lvl > curr_mx_lvl) {
            for (int i = curr_mx_lvl + 1; i <= new_lvl; i++) {
                update[i] = head;
            }
            curr_mx_lvl = new_lvl;
        }
        // create a new node
        Node* new_node = new Node(val, new_lvl);

        // updating connections
        for (int i = 0; i <= new_lvl; i++) {
            new_node->next[i] = update[i]->next[i];
            new_node->prev[i] = update[i]; // because update[i] - place, after which the insertion is needed
            update[i]->next[i]->prev[i] = new_node;
            update[i]->next[i] = new_node;
        }
        all_size++;
        return Iterator(new_node);
    }
};

#endif