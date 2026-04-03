#include <catch2/catch.hpp>
#include "core/ca/CellularAutomaton.h"

TEST_CASE("CellularAutomaton: Position", "[ca]") {
    autom::core::ca::CellularAutomaton cell;

    SECTION("Default position is (0, 0)") {
        double x, y;
        cell.GetPosition(x, y);
        REQUIRE(x == 0.0);
        REQUIRE(y == 0.0);
    }

    SECTION("SetPosition updates position") {
        cell.SetPosition(10.5, 20.3);
        double x, y;
        cell.GetPosition(x, y);
        REQUIRE(x == 10.5);
        REQUIRE(y == 20.3);
    }
}

TEST_CASE("CellularAutomaton: Velocity", "[ca]") {
    autom::core::ca::CellularAutomaton cell;

    SECTION("Default velocity is (0, 0)") {
        double vx, vy;
        cell.GetVelocity(vx, vy);
        REQUIRE(vx == 0.0);
        REQUIRE(vy == 0.0);
    }

    SECTION("SetVelocity updates velocity") {
        cell.SetVelocity(0.5, -0.3);
        double vx, vy;
        cell.GetVelocity(vx, vy);
        REQUIRE(vx == 0.5);
        REQUIRE(vy == -0.3);
    }
}

TEST_CASE("CellularAutomaton: Score", "[ca]") {
    autom::core::ca::CellularAutomaton cell;

    SECTION("Default score is 0") {
        REQUIRE(cell.GetScore() == 0);
    }

    SECTION("SetScore updates score") {
        cell.SetScore(42);
        REQUIRE(cell.GetScore() == 42);
    }
}

TEST_CASE("CellularAutomaton: ID", "[ca]") {
    autom::core::ca::CellularAutomaton cell;

    SECTION("Default ID is 0") {
        REQUIRE(cell.GetId() == 0);
    }

    SECTION("SetId updates ID") {
        cell.SetId(5);
        REQUIRE(cell.GetId() == 5);
    }
}


