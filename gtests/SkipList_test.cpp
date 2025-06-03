//Lichkovaha Daniil
//st129351@student.spbu.ru
//SkipList
#include <gtest/gtest.h>
#include "skiplist.h"

class SkipListTest : public ::testing::Test {
protected:
    void SetUp() override {
        list1.insert(1);
        list1.insert(3);
        list1.insert(2);
    }

    SkipList<int> emptyList;
    SkipList<int> list1;
};

TEST_F(SkipListTest, DefaultConstructor) {
    EXPECT_TRUE(emptyList.empty());
    EXPECT_EQ(emptyList.size(), 0);
    EXPECT_EQ(emptyList.begin(), emptyList.end());
}

TEST_F(SkipListTest, InitializerListConstructor) {
    SkipList<int> list = {5, 3, 1, 4, 2};
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 5);
    
    int expected = 1;
    for (auto val : list) {
        EXPECT_EQ(val, expected++);
    }
}

TEST_F(SkipListTest, CopyConstructor) {
    SkipList<int> copy(list1);
    EXPECT_EQ(list1, copy);
    EXPECT_EQ(list1.size(), copy.size());
}

TEST_F(SkipListTest, AssignmentOperator) {
    SkipList<int> copy;
    copy = list1;
    EXPECT_EQ(list1, copy);
    EXPECT_EQ(list1.size(), copy.size());
}

TEST_F(SkipListTest, Insert) {
    auto it = emptyList.insert(5);
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(emptyList.size(), 1);
    EXPECT_FALSE(emptyList.empty());
    
    it = emptyList.insert(3);
    EXPECT_EQ(*it, 3);
    
    it = emptyList.insert(7);
    EXPECT_EQ(*it, 7);
    
    // Check order
    std::vector<int> expected = {3, 5, 7};
    int i = 0;
    for (auto val : emptyList) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST_F(SkipListTest, Erase) {
    EXPECT_TRUE(list1.erase(2));
    EXPECT_EQ(list1.size(), 2);
    EXPECT_EQ(list1.find(2), list1.end());
    
    EXPECT_FALSE(list1.erase(10)); // non-existent element
    EXPECT_EQ(list1.size(), 2);
    
    EXPECT_TRUE(list1.erase(1));
    EXPECT_TRUE(list1.erase(3));
    EXPECT_TRUE(list1.empty());
}

TEST_F(SkipListTest, Find) {
    auto it = list1.find(2);
    EXPECT_NE(it, list1.end());
    EXPECT_EQ(*it, 2);
    
    it = list1.find(5);
    EXPECT_EQ(it, list1.end());
}

TEST_F(SkipListTest, Clear) {
    list1.clear();
    EXPECT_TRUE(list1.empty());
    EXPECT_EQ(list1.size(), 0);
    EXPECT_EQ(list1.begin(), list1.end());
}

TEST_F(SkipListTest, Iterators) {
    int sum = 0;
    for (auto it = list1.begin(); it != list1.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
    
    sum = 0;
    for (int val : list1) {
        sum += val;
    }
    EXPECT_EQ(sum, 6);
    
    auto it = list1.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it, 3);
}

TEST_F(SkipListTest, Equality) {
    SkipList<int> list2 = {1, 2, 3};
    SkipList<int> list3 = {1, 2, 4};
    
    EXPECT_EQ(list1, list2);
    EXPECT_NE(list1, list3);
}

TEST_F(SkipListTest, OutputOperator) {
    std::ostringstream oss;
    oss << list1;
    EXPECT_EQ(oss.str(), "[1, 2, 3]");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}