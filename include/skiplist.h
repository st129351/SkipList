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
        reference operator*() const {
            if (!ptr) throw std::out_of_range("Null iterator, without dereferencing!");
            return ptr->value;
        }

        // ++iter
        Iterator& operator++() {
            ptr = ptr->next[0];
            return *this;
        }
        // iter++
        Iterator operator++(int) {
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
     * @param val the value for insert
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

    /**
     * @brief delete an item
     * @param val the value for erase
     * @return true if success, false if not found
     */
    bool erase(const T& val) {
        std::vector<Node*> update(mx_lvl + 1, nullptr);
        Node* curr = head;

        // search
        for (int i = curr_mx_lvl; i >= 0; i--) {
            while (curr->next[i] != tail && curr->next[i]->value < val) {
                curr = curr->next[i]; // last curr on the zero lvl
            }
            update[i] = curr;
        }
        curr = curr->next[0]; 

        // if an item isn't exist
        if (curr == tail || curr->value != val) {
            return false;
        }

        // updating
        for (int i = 0; i <= curr->lvl; i++) {
            update[i]->next[i] = curr->next[i];
            curr->next[i]->prev[i] = update[i];
        }

        // updating current max level
        while (curr_mx_lvl > 0 && head->next[curr_mx_lvl] == tail) {
            curr_mx_lvl--;
        }

        delete curr; // ptr on deleted item
        all_size--;
        return true;
    }

    /**
     * @brief search an item
     * @param val value for search
     * @return the iterator on found item or end() if not found
     */
    Iterator find(const T& val) const {
        Node* curr = head;
        
        for (int i = curr_mx_lvl; i >= 0; i--) {
            while (curr->next[i] != tail && curr->next[i]->value < val) {
                curr = curr->next[i];
            }
        }

        curr = curr->next[0];
        if (curr != tail && curr->value == val) {
            return Iterator(curr);
        }
        return end(); // ptr on tail, mark of end of skiplist
    }

    /**
     * @brief iterator on the end of the skiplist
     * @return iterator on item after last item (after tail->prev[0], so tail)
     */
    Iterator end() const { // const - not change fields of class SkipList
        return Iterator(tail); // in STl end() is next after the last with norm value
    }

    /**
     * @brief iterator on the start of the skiplist
     * @return iterator on 1st item (0 is head) on 0 level 
     */
    Iterator begin() const {
        return Iterator(head->next[0]);
    }

    /**
     * @brief clear the skiplist
    */
    void clear() {
        Node* curr = head->next[0];
        while (curr != tail) {
            Node* temp = curr;
            curr = curr->next[0];
            delete temp;
        }
        for (int i = 0; i <= mx_lvl; i++) {
            head->next[i] = tail;
            tail->prev[i] = head;
        }

        curr_mx_lvl = 0;
        all_size = 0;
    } // enough delete on 0 lvl, because in skiplist all values (items) in the only one instance

    /**
     * @brief checking for emptiness
     * @return true if skiplist is empty, else false
     */
    bool empty() const {
        return all_size == 0;
    }

    /**
     * @brief quantity of items
     * @return quantity of items in the skiplist, without head and tail
     */
    size_t size() const {
        return all_size;
    }

    /**
     * @brief the operator for the conclusion
     * @param os the conclusion stream
     * @param lst thre list for conclusion
     * @return the conclusion stream
     */
    friend std::ostream& operator<<(std::ostream& os, const SkipList& lst) { // binary operator (ostream + SkipList class)
        os << "[";
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            os << *it;
            if (std::next(it) != lst.end()) { // next(it) to avoid changing the iterator as ++it
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    /**
     * @brief oterator for comparison
     * @param other the another lst for comparison
     * @return true if this skiplist == other skiplist, otherwise false
     */
    bool operator==(const SkipList& other) const {
        if (all_size != other.all_size) {
            return false;
        }

        auto it1 = begin();
        auto it2 = other.begin();

        while (it1 != end() && it2 != other.end()) {
            if (*it1 != *it2) {
                return false;
            }
            ++it1;
            ++it2;
        }
        
        return true;
    }

    /**
     * @brief operator not equal
     * @param other another skiplist
     * @return true if not equal, else false
     */
    bool operator!=(const SkipList& other) const {
        return !(*this == other);
    }
};

#endif