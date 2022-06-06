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

TEST_CASE("Test empty") {
    std::vector<char> values = {};
    std::shared_ptr<IBST<char>> tree;
    REQUIRE_NOTHROW(tree = std::make_shared<CartesianBST<char>>(values.begin(), values.end()));
    SECTION("Empty iterators are ok") {
        REQUIRE_NOTHROW(tree->begin());
        REQUIRE_NOTHROW(tree->end());
        CHECK(tree->empty());
        CHECK(tree->begin() == tree->end());
        CHECK_THROWS_AS(--tree->begin(), std::runtime_error);
        CHECK_THROWS_AS(--tree->end(), std::runtime_error);
        CHECK_THROWS_AS(++tree->begin(), std::runtime_error);
        CHECK_THROWS_AS(++tree->end(), std::runtime_error);
    }
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

TEST_CASE("Test one element") {
    std::vector<char> values = {42};
    std::shared_ptr<IBST<char>> tree;
    REQUIRE_NOTHROW(tree = std::make_shared<CartesianBST<char>>(values.begin(), values.end()));
    SECTION("One element iterators are ok") {
        REQUIRE_NOTHROW(tree->begin());
        REQUIRE_NOTHROW(tree->end());
        REQUIRE_NOTHROW(++tree->begin());
        REQUIRE_NOTHROW(tree->begin()++);
        REQUIRE_NOTHROW(--tree->end());
        REQUIRE_NOTHROW(tree->end()--);

        CHECK(!tree->empty());
        CHECK(tree->begin() != tree->end());
        CHECK(++tree->begin() == tree->end());
        CHECK(tree->begin()++ != tree->end());
        CHECK(tree->begin() == --tree->end());
        CHECK(tree->begin() != tree->end()--);
        CHECK(++tree->begin() != --tree->end());
        CHECK_THROWS_AS(--tree->begin(), std::runtime_error);
        CHECK_THROWS_AS(++tree->end(), std::runtime_error);

        CHECK(--(++tree->begin()) == tree->begin());
        CHECK(++(--tree->end()) == tree->end());
    }
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

TEST_CASE("Test merge empty") {
    std::vector<int> lhs_vals = {2, 2};
    std::vector<int> rhs_vals = {};
    std::shared_ptr<IBST<int>> lhs_tree =
        std::make_shared<CartesianBST<int>>(lhs_vals.begin(), lhs_vals.end());
    std::shared_ptr<IBST<int>> rhs_tree =
        std::make_shared<CartesianBST<int>>(rhs_vals.begin(), rhs_vals.end());
    SECTION("Empty on rhs") {
        lhs_tree->merge(rhs_tree);
        CheckTreeContent(lhs_tree, std::vector<int>({2, 2}));
    }
    SECTION("Empty on lhs") {
        rhs_tree->merge(lhs_tree);
        CheckTreeContent(rhs_tree, std::vector<int>({2, 2}));
    }
    SECTION("Empty on both") {
        std::shared_ptr<IBST<int>> empty_tree =
            std::make_shared<CartesianBST<int>>(rhs_vals.begin(), rhs_vals.end());
        rhs_tree->merge(empty_tree);
        CheckTreeContent(rhs_tree, std::vector<int>({}));
    }
}

TEST_CASE("Test simple split") {
    std::vector<int> vals = {1, 2, 3, 4, 5, 6};
    std::shared_ptr<IBST<int>> tree = std::make_shared<CartesianBST<int>>(vals.begin(), vals.end());
    auto it = tree->begin();
    ++it, ++it;
    auto tree_pair = it.split();
    CheckTreeContent(tree_pair.first, std::vector<int>({1, 2, 3}));
    CheckTreeContent(tree_pair.second, std::vector<int>({4, 5, 6}));
}

TEST_CASE("Test split same") {
    std::vector<int> vals = {5, 5, 5, 5, 5, 5};
    std::shared_ptr<IBST<int>> tree = std::make_shared<CartesianBST<int>>(vals.begin(), vals.end());
    auto it = tree->begin();
    ++it, ++it, ++it;
    auto tree_pair = it.split();
    CheckTreeContent(tree_pair.first, std::vector<int>({5, 5, 5, 5}));
    CheckTreeContent(tree_pair.second, std::vector<int>({5, 5}));
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
    auto tree_pair = it.split();
    SECTION("Merge works consecutive") {
        lhs_tree->merge(tree_pair.first);
        CheckTreeContent(lhs_tree, std::vector<int>({1, 2, 2, 3}));
        CheckTreeContent(tree_pair.second, std::vector<int>({4, 5}));
    }
    SECTION("Merge works always") {
        lhs_tree->merge(tree_pair.second);
        CheckTreeContent(lhs_tree, std::vector<int>({1, 2, 4, 5}));
        CheckTreeContent(tree_pair.first, std::vector<int>({2, 3}));
    }
}

TEST_CASE("Test iterators coherence") {
}