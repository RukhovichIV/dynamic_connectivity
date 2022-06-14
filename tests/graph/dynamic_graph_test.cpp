#define CATCH_CONFIG_MAIN

#include "../../src/graph/dynamic_graph.cpp"
#include "../catch/catch.hpp"

TEST_CASE("Test empty graph") {
    DynamicGraph g = DynamicGraph(0);
    SECTION("Edge cannot be added") {
        CHECK_THROWS_AS(g.insert(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.insert(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.insert(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.insert(1, 1), std::runtime_error);
    }
    SECTION("Edge cannot be removed") {
        CHECK_THROWS_AS(g.erase(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 1), std::runtime_error);
    }
    SECTION("No connection in empty graph") {
        CHECK_THROWS_AS(g.is_connected(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(1, 1), std::runtime_error);
    }
}

TEST_CASE("Test one vertex graph") {
    DynamicGraph g = DynamicGraph(1);
    SECTION("Edge cannot be added") {
        CHECK_THROWS_AS(g.insert(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.insert(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.insert(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.insert(1, 1), std::runtime_error);
    }
    SECTION("Edge cannot be removed") {
        CHECK_THROWS_AS(g.erase(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 1), std::runtime_error);
    }
    SECTION("No connection in one-verticed graph") {
        CHECK_THROWS_AS(g.is_connected(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(1, 1), std::runtime_error);
    }
}

TEST_CASE("Test two vertex graph") {
    DynamicGraph g = DynamicGraph(2);
    SECTION("Not existing edge") {
        CHECK_THROWS_AS(g.insert(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.insert(1, 2), std::runtime_error);
        CHECK_THROWS_AS(g.insert(2, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 2), std::runtime_error);
        CHECK_THROWS_AS(g.erase(2, 1), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(1, 2), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(2, 1), std::runtime_error);
        CHECK_FALSE(g.is_connected(0, 1));
        CHECK_FALSE(g.is_connected(1, 0));
    }
    SECTION("Edge can be added") {
        SECTION("Way #1") {
            CHECK_NOTHROW(g.insert(0, 1));
            CHECK(g.is_connected(0, 1));
            CHECK(g.is_connected(1, 0));
            CHECK_NOTHROW(g.erase(0, 1));
            CHECK_FALSE(g.is_connected(0, 1));
            CHECK_FALSE(g.is_connected(1, 0));
        }
        SECTION("Way #2") {
            CHECK_NOTHROW(g.insert(1, 0));
            CHECK(g.is_connected(0, 1));
            CHECK(g.is_connected(1, 0));
            CHECK_NOTHROW(g.erase(0, 1));
            CHECK_FALSE(g.is_connected(0, 1));
            CHECK_FALSE(g.is_connected(1, 0));
        }
        SECTION("Way #3") {
            CHECK_NOTHROW(g.insert(0, 1));
            CHECK(g.is_connected(0, 1));
            CHECK(g.is_connected(1, 0));
            CHECK_NOTHROW(g.erase(1, 0));
            CHECK_FALSE(g.is_connected(0, 1));
            CHECK_FALSE(g.is_connected(1, 0));
        }
    }
}

TEST_CASE("Test three vertex graph") {
    DynamicGraph g = DynamicGraph(3);
    SECTION("Not existing edge") {
        CHECK_THROWS_AS(g.insert(0, 0), std::runtime_error);
        CHECK_THROWS_AS(g.insert(2, 2), std::runtime_error);
        CHECK_THROWS_AS(g.insert(2, 3), std::runtime_error);
        CHECK_THROWS_AS(g.insert(3, 2), std::runtime_error);
        CHECK_THROWS_AS(g.erase(0, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 2), std::runtime_error);
        CHECK_THROWS_AS(g.erase(2, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(0, 2), std::runtime_error);
        CHECK_THROWS_AS(g.erase(2, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(0, 3), std::runtime_error);
        CHECK_THROWS_AS(g.erase(3, 0), std::runtime_error);
        CHECK_THROWS_AS(g.erase(1, 3), std::runtime_error);
        CHECK_THROWS_AS(g.erase(3, 1), std::runtime_error);
        CHECK_THROWS_AS(g.erase(2, 3), std::runtime_error);
        CHECK_THROWS_AS(g.erase(3, 2), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(1, 3), std::runtime_error);
        CHECK_THROWS_AS(g.is_connected(3, 1), std::runtime_error);
        CHECK_FALSE(g.is_connected(0, 1));
        CHECK_FALSE(g.is_connected(1, 0));
        CHECK_FALSE(g.is_connected(0, 2));
        CHECK_FALSE(g.is_connected(2, 0));
        CHECK_FALSE(g.is_connected(1, 2));
        CHECK_FALSE(g.is_connected(2, 1));
    }
    SECTION("One edge") {
        CHECK_NOTHROW(g.insert(0, 1));
        CHECK(g.is_connected(0, 1));
        CHECK(g.is_connected(1, 0));
        CHECK_FALSE(g.is_connected(0, 2));
        CHECK_FALSE(g.is_connected(2, 0));
        CHECK_FALSE(g.is_connected(1, 2));
        CHECK_FALSE(g.is_connected(2, 1));
        CHECK_NOTHROW(g.erase(0, 1));
        CHECK_FALSE(g.is_connected(0, 1));
        CHECK_FALSE(g.is_connected(1, 0));
        CHECK_FALSE(g.is_connected(0, 2));
        CHECK_FALSE(g.is_connected(2, 0));
        CHECK_FALSE(g.is_connected(1, 2));
        CHECK_FALSE(g.is_connected(2, 1));
        SECTION("Add same edge again") {
            CHECK_NOTHROW(g.insert(1, 0));
            CHECK(g.is_connected(0, 1));
            CHECK(g.is_connected(1, 0));
            CHECK_FALSE(g.is_connected(0, 2));
            CHECK_FALSE(g.is_connected(2, 0));
            CHECK_FALSE(g.is_connected(1, 2));
            CHECK_FALSE(g.is_connected(2, 1));
        }
        SECTION("Add another edge") {
            CHECK_NOTHROW(g.insert(0, 2));
            CHECK(g.is_connected(0, 2));
            CHECK(g.is_connected(2, 0));
            CHECK_FALSE(g.is_connected(0, 1));
            CHECK_FALSE(g.is_connected(1, 0));
            CHECK_FALSE(g.is_connected(1, 2));
            CHECK_FALSE(g.is_connected(2, 1));
        }
    }
    SECTION("Full connection") {
        CHECK_NOTHROW(g.insert(0, 1));
        CHECK_NOTHROW(g.insert(2, 0));
        CHECK(g.is_connected(0, 1));
        CHECK(g.is_connected(1, 0));
        CHECK(g.is_connected(0, 2));
        CHECK(g.is_connected(2, 0));
        CHECK(g.is_connected(1, 2));
        CHECK(g.is_connected(2, 1));
        SECTION("Remove first") {
            CHECK_NOTHROW(g.erase(0, 1));
            CHECK(g.is_connected(0, 2));
            CHECK(g.is_connected(2, 0));
            CHECK_FALSE(g.is_connected(0, 1));
            CHECK_FALSE(g.is_connected(1, 0));
            CHECK_FALSE(g.is_connected(1, 2));
            CHECK_FALSE(g.is_connected(2, 1));
        }
        SECTION("Remove second") {
            CHECK_NOTHROW(g.erase(0, 2));
            CHECK(g.is_connected(0, 1));
            CHECK(g.is_connected(1, 0));
            CHECK_FALSE(g.is_connected(0, 2));
            CHECK_FALSE(g.is_connected(2, 0));
            CHECK_FALSE(g.is_connected(1, 2));
            CHECK_FALSE(g.is_connected(2, 1));
        }
    }
}
