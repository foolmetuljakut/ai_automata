#include <catch2/catch.hpp>
#include "core/ca/CellularAutomatonSimulation.h"
#include "../../../test/mocks/ca/MockCellularAutomaton.h"

TEST_CASE("CellularAutomatonSimulation: Initialization", "[simulation]") {
    auto cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    autom::core::ca::CellularAutomatonSimulation sim(cell);

    SECTION("Default state is STOPPED") {
        REQUIRE(sim.GetState() == autom::core::ca::CellularAutomatonSimulation::State::STOPPED);
    }

    SECTION("Cell is accessible") {
        REQUIRE(sim.GetCell() == cell);
    }
}

TEST_CASE("CellularAutomatonSimulation: State Management", "[simulation]") {
    auto cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    autom::core::ca::CellularAutomatonSimulation sim(cell);

    SECTION("SetState changes to RUNNING") {
        sim.SetState(autom::core::ca::CellularAutomatonSimulation::State::RUNNING);
        REQUIRE(sim.GetState() == autom::core::ca::CellularAutomatonSimulation::State::RUNNING);
    }

    SECTION("SetState changes back to STOPPED") {
        sim.SetState(autom::core::ca::CellularAutomatonSimulation::State::RUNNING);
        sim.SetState(autom::core::ca::CellularAutomatonSimulation::State::STOPPED);
        REQUIRE(sim.GetState() == autom::core::ca::CellularAutomatonSimulation::State::STOPPED);
    }
}

TEST_CASE("CellularAutomatonSimulation: Update when STOPPED", "[simulation]") {
    auto cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    cell->SetPosition(0.0, 0.0);
    cell->SetVelocity(1.0, 1.0);

    autom::core::ca::CellularAutomatonSimulation sim(cell);

    SECTION("Position does not change when STOPPED") {
        sim.Update(1.0);  // deltaTime = 1.0

        double x, y;
        cell->GetPosition(x, y);
        REQUIRE(x == 0.0);
        REQUIRE(y == 0.0);
    }
}

TEST_CASE("CellularAutomatonSimulation: Update when RUNNING", "[simulation]") {
    auto cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    cell->SetPosition(10.0, 20.0);
    cell->SetVelocity(5.0, -3.0);

    autom::core::ca::CellularAutomatonSimulation sim(cell);
    sim.SetState(autom::core::ca::CellularAutomatonSimulation::State::RUNNING);

    SECTION("Position updates correctly with velocity and deltaTime") {
        sim.Update(2.0);  // deltaTime = 2.0

        double x, y;
        cell->GetPosition(x, y);

        // newX = 10.0 + 5.0 * 2.0 = 20.0
        // newY = 20.0 + (-3.0) * 2.0 = 14.0
        REQUIRE(x == 20.0);
        REQUIRE(y == 14.0);
    }

    SECTION("Multiple updates accumulate") {
        sim.Update(1.0);  // deltaTime = 1.0
        sim.Update(1.0);  // deltaTime = 1.0

        double x, y;
        cell->GetPosition(x, y);

        // After first update: (10 + 5*1, 20 - 3*1) = (15, 17)
        // After second update: (15 + 5*1, 17 - 3*1) = (20, 14)
        REQUIRE(x == 20.0);
        REQUIRE(y == 14.0);
    }
}

TEST_CASE("CellularAutomatonSimulation: Update with zero velocity", "[simulation]") {
    auto cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    cell->SetPosition(5.0, 10.0);
    cell->SetVelocity(0.0, 0.0);  // Keine Bewegung

    autom::core::ca::CellularAutomatonSimulation sim(cell);
    sim.SetState(autom::core::ca::CellularAutomatonSimulation::State::RUNNING);

    SECTION("Position remains unchanged with zero velocity") {
        sim.Update(5.0);

        double x, y;
        cell->GetPosition(x, y);
        REQUIRE(x == 5.0);
        REQUIRE(y == 10.0);
    }
}

