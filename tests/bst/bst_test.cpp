#define CATCH_CONFIG_MAIN

#include "../../src/bst/bst_interface.h"
#include "../../src/bst/cartesian_bst.h"
#include "../catch/catch.hpp"

template <class TreePtr, class Iterable>
void CheckTreeContent(const TreePtr ptr, const Iterable& arr) {
    auto lit = ptr->begin();
    auto rit = arr.begin();
    while (lit != ptr->end() && rit != arr.end()) {
        CHECK(*lit++ == *rit++);
    }
    CHECK(lit == ptr->end());
    CHECK(rit == arr.end());
}

TEST_CASE("Test BST creates") {
    std::vector<char> values = {'a', 'b', 'c', 'd', 'e'};
    std::shared_ptr<IBST<char>> tree;
    REQUIRE_NOTHROW(tree = std::make_shared<CartesianBST<char>>(values.begin(), values.end()));
    SECTION("Iterators work") {
        std::vector<char> result;
        for (auto elem : *tree) {
            result.emplace_back(elem);
        }
        REQUIRE(values == result);
    }
    SECTION("Values are same") {
        CheckTreeContent(tree, values);
    }
}

TEST_CASE("Test simple merge") {
    std::vector<int> lhs_vals = {1, 2, 3};
    std::vector<int> rhs_vals = {4, 5, 6};
    std::shared_ptr<IBST<int>> lhs_tree =
        std::make_shared<CartesianBST<int>>(lhs_vals.begin(), lhs_vals.end());
    std::shared_ptr<IBST<int>> rhs_tree =
        std::make_shared<CartesianBST<int>>(rhs_vals.begin(), rhs_vals.end());
    lhs_tree->merge(rhs_tree);
    CheckTreeContent(lhs_tree, std::vector<int>({1, 2, 3, 4, 5, 6}));
}

TEST_CASE("Test merge same") {
    std::vector<int> lhs_vals = {2, 2};
    std::vector<int> rhs_vals = {2, 2, 2, 2};
    std::shared_ptr<IBST<int>> lhs_tree =
        std::make_shared<CartesianBST<int>>(lhs_vals.begin(), lhs_vals.end());
    std::shared_ptr<IBST<int>> rhs_tree =
        std::make_shared<CartesianBST<int>>(rhs_vals.begin(), rhs_vals.end());
    lhs_tree->merge(rhs_tree);
    CheckTreeContent(lhs_tree, std::vector<int>({2, 2, 2, 2, 2, 2}));
}

TEST_CASE("Test simple split") {
    std::vector<int> vals = {1, 2, 3, 4, 5, 6};
    std::shared_ptr<IBST<int>> tree = std::make_shared<CartesianBST<int>>(vals.begin(), vals.end());
    auto it = tree->begin();
    ++it, ++it;
    auto other_tree = tree->split(it);
    CheckTreeContent(tree, std::vector<int>({1, 2, 3}));
    CheckTreeContent(other_tree, std::vector<int>({4, 5, 6}));
}

TEST_CASE("Test split same") {
    std::vector<int> vals = {5, 5, 5, 5, 5, 5};
    std::shared_ptr<IBST<int>> tree = std::make_shared<CartesianBST<int>>(vals.begin(), vals.end());
    auto it = tree->begin();
    ++it, ++it, ++it;
    auto other_tree = tree->split(it);
    CheckTreeContent(tree, std::vector<int>({5, 5, 5, 5}));
    CheckTreeContent(other_tree, std::vector<int>({5, 5}));
}

TEST_CASE("Test split and merge") {
    std::vector<int> lhs_vals = {1, 2};
    std::vector<int> rhs_vals = {2, 3, 4, 5};
    std::shared_ptr<IBST<int>> lhs_tree =
        std::make_shared<CartesianBST<int>>(lhs_vals.begin(), lhs_vals.end());
    std::shared_ptr<IBST<int>> rhs_tree =
        std::make_shared<CartesianBST<int>>(rhs_vals.begin(), rhs_vals.end());
    auto it = rhs_tree->begin();
    ++it;
    auto other_tree = rhs_tree->split(it);
    SECTION("Merge works consecutive") {
        lhs_tree->merge(rhs_tree);
        CheckTreeContent(lhs_tree, std::vector<int>({1, 2, 2, 3}));
        CheckTreeContent(other_tree, std::vector<int>({4, 5}));
    }
    SECTION("Merge works always") {
        lhs_tree->merge(other_tree);
        CheckTreeContent(lhs_tree, std::vector<int>({1, 2, 4, 5}));
        CheckTreeContent(rhs_tree, std::vector<int>({2, 3}));
    }
}