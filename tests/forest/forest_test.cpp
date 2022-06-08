#define CATCH_CONFIG_MAIN

#include "../../src/forest/spanning_forest.h"
#include "../catch/catch.hpp"

TEST_CASE("Test empty forest") {
    Forest f = Forest(0);
    SECTION("Edge cannot be added") {
        CHECK_THROWS_AS(f.add_new_edge(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(0, 1), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(1, 0), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(1, 1), std::runtime_error);
    }
    SECTION("Edge cannot be removed") {
        CHECK_THROWS_AS(f.remove_existing_edge(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(0, 1), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(1, 0), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(1, 1), std::runtime_error);
    }
    SECTION("No connection in empty graph") {
        CHECK_THROWS_AS(f.is_connected(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(0, 1), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(1, 0), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(1, 1), std::runtime_error);
    }
}

TEST_CASE("Test one vertex forest") {
    Forest f = Forest(1);
    SECTION("Edge cannot be added") {
        CHECK_THROWS_AS(f.add_new_edge(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(0, 1), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(1, 0), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(1, 1), std::runtime_error);
    }
    SECTION("Edge cannot be removed") {
        CHECK_THROWS_AS(f.remove_existing_edge(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(0, 1), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(1, 0), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(1, 1), std::runtime_error);
    }
    SECTION("No connection in one-verticed graph") {
        CHECK_THROWS_AS(f.is_connected(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(0, 1), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(1, 0), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(1, 1), std::runtime_error);
    }
}

TEST_CASE("Test two vertex forest") {
    Forest f = Forest(2);
    SECTION("Not existing edge") {
        CHECK_THROWS_AS(f.add_new_edge(0, 0), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(1, 2), std::runtime_error);
        CHECK_THROWS_AS(f.add_new_edge(2, 1), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(1, 2), std::runtime_error);
        CHECK_THROWS_AS(f.remove_existing_edge(2, 1), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(1, 2), std::runtime_error);
        CHECK_THROWS_AS(f.is_connected(2, 1), std::runtime_error);
    }
    SECTION("Edge can be added") {
        SECTION("Way #1") {
            CHECK_FALSE(f.is_connected(0, 1));
            CHECK_FALSE(f.is_connected(1, 0));
            CHECK_NOTHROW(f.add_new_edge(0, 1));
            CHECK(f.is_connected(0, 1));
            CHECK(f.is_connected(1, 0));
            CHECK_NOTHROW(f.remove_existing_edge(0, 1));
            CHECK_FALSE(f.is_connected(0, 1));
            CHECK_FALSE(f.is_connected(1, 0));
        }
        SECTION("Way #2") {
            CHECK_FALSE(f.is_connected(0, 1));
            CHECK_FALSE(f.is_connected(1, 0));
            CHECK_NOTHROW(f.add_new_edge(1, 0));
            CHECK(f.is_connected(0, 1));
            CHECK(f.is_connected(1, 0));
            CHECK_NOTHROW(f.remove_existing_edge(0, 1));
            CHECK_FALSE(f.is_connected(0, 1));
            CHECK_FALSE(f.is_connected(1, 0));
        }
        SECTION("Way #3") {
            CHECK_FALSE(f.is_connected(0, 1));
            CHECK_FALSE(f.is_connected(1, 0));
            CHECK_NOTHROW(f.add_new_edge(0, 1));
            CHECK(f.is_connected(0, 1));
            CHECK(f.is_connected(1, 0));
            CHECK_NOTHROW(f.remove_existing_edge(1, 0));
            CHECK_FALSE(f.is_connected(0, 1));
            CHECK_FALSE(f.is_connected(1, 0));
        }
    }
}