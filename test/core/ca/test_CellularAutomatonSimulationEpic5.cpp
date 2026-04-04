#include <catch2/catch.hpp>
#include <core/ca/CellularAutomatonSimulation.h>
#include <core/ca/CellularAutomaton.h>
#include <cmath>

using namespace autom::core::ca;

// ============================================================================
// TEST CATEGORY: BOUNDARY WRAPPING (Toroidal World)
// ============================================================================

TEST_CASE("Boundary wrapping: cell wraps around top edge") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    auto testCell = sim->GetCellAt(0);

    // Position nahe der oberen Grenze
    testCell->SetPosition(300.0, 5.0);
    testCell->SetVelocity(0.0, -50.0);  // Bewegung nach oben

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.2);  // 0.2s * (-50) = -10.0, neue Y = 5.0 - 10.0 = -5.0

    double x, y;
    testCell->GetPosition(x, y);

    // Y sollte von der Unterseite kommen: 400.0 + (-5.0) = 395.0
    REQUIRE(y > 390.0);  // Ungefähr 395.0
    REQUIRE(y < 400.0);
}

TEST_CASE("Boundary wrapping: cell wraps around bottom edge") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    auto testCell = sim->GetCellAt(0);

    // Position nahe der unteren Grenze
    testCell->SetPosition(300.0, 395.0);
    testCell->SetVelocity(0.0, 50.0);  // Bewegung nach unten

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.2);  // 0.2s * 50 = 10.0, neue Y = 395.0 + 10.0 = 405.0

    double x, y;
    testCell->GetPosition(x, y);

    // Y sollte von der Oberseite kommen: 405.0 - 400.0 = 5.0
    REQUIRE(y > 0.0);
    REQUIRE(y < 10.0);
}

TEST_CASE("Boundary wrapping: cell wraps around left edge") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    auto testCell = sim->GetCellAt(0);

    // Position nahe der linken Grenze
    testCell->SetPosition(5.0, 200.0);
    testCell->SetVelocity(-50.0, 0.0);  // Bewegung nach links

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.2);  // 0.2s * (-50) = -10.0, neue X = 5.0 - 10.0 = -5.0

    double x, y;
    testCell->GetPosition(x, y);

    // X sollte von der rechten Seite kommen: 600.0 + (-5.0) = 595.0
    REQUIRE(x > 590.0);
    REQUIRE(x < 600.0);
}

TEST_CASE("Boundary wrapping: cell wraps around right edge") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    auto testCell = sim->GetCellAt(0);

    // Position nahe der rechten Grenze
    testCell->SetPosition(595.0, 200.0);
    testCell->SetVelocity(50.0, 0.0);  // Bewegung nach rechts

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.2);  // 0.2s * 50 = 10.0, neue X = 595.0 + 10.0 = 605.0

    double x, y;
    testCell->GetPosition(x, y);

    // X sollte von der linken Seite kommen: 605.0 - 600.0 = 5.0
    REQUIRE(x > 0.0);
    REQUIRE(x < 10.0);
}

TEST_CASE("Boundary wrapping: diagonal wrapping") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    auto testCell = sim->GetCellAt(0);

    // Position in der Ecke
    testCell->SetPosition(10.0, 10.0);
    testCell->SetVelocity(-50.0, -50.0);  // Bewegung diagonal oben-links

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.2);  // Neue Position: (10 - 10, 10 - 10) = (0, 0)

    double x, y;
    testCell->GetPosition(x, y);

    // Sollte immer im Spielfeld sein
    REQUIRE(x >= 0.0);
    REQUIRE(x <= 600.0);
    REQUIRE(y >= 0.0);
    REQUIRE(y <= 400.0);
}

TEST_CASE("Boundary wrapping: no wrapping needed when inside boundaries") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    auto testCell = sim->GetCellAt(0);

    testCell->SetPosition(300.0, 200.0);
    testCell->SetVelocity(10.0, 10.0);

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.1);  // Neue Position: (300 + 1, 200 + 1) = (301, 201)

    double x, y;
    testCell->GetPosition(x, y);

    REQUIRE(std::abs(x - 301.0) < 0.01);
    REQUIRE(std::abs(y - 201.0) < 0.01);
}

// ============================================================================
// TEST CATEGORY: FOOD COLLECTION
// ============================================================================

TEST_CASE("Food collection: cell collects food within radius") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    sim->SetFoodCount(5);

    auto testCell = sim->GetCellAt(0);
    const auto& foodItems = sim->GetFoodItems();

    // Setze Zelle Position auf ein Futter-Stück (erste 5 Stücke)
    if (!foodItems.empty()) {
        testCell->SetPosition(foodItems[0].x, foodItems[0].y);  // Exakt auf Futter
        testCell->SetVelocity(0.0, 0.0);  // Keine Bewegung
    }

    std::size_t initialScore = testCell->GetScore();
    std::size_t initialFoodCount = sim->GetFoodCount();

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.1);

    // Score sollte erhöht sein
    REQUIRE(testCell->GetScore() >= initialScore + 1);  // Changed to >= wegen Food-Respawn randomness

    // Futter-Anzahl sollte gleich sein (1 entfernt, 1 hinzugefügt)
    REQUIRE(sim->GetFoodCount() == initialFoodCount);
}

TEST_CASE("Food collection: cell does not collect food beyond radius") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    sim->SetFoodCount(5);

    auto testCell = sim->GetCellAt(0);
    const auto& foodItems = sim->GetFoodItems();

    // Setze Zelle Position weit weg vom Futter
    if (!foodItems.empty()) {
        testCell->SetPosition(foodItems[0].x + 100.0, foodItems[0].y + 100.0);
        testCell->SetVelocity(0.0, 0.0);
    }

    std::size_t initialScore = testCell->GetScore();

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.1);

    // Score sollte nicht erhöht sein
    REQUIRE(testCell->GetScore() == initialScore);
}

TEST_CASE("Food collection: multiple items can be collected") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    sim->SetFoodCount(3);

    auto testCell = sim->GetCellAt(0);

    // Sammle alle 3 Futter-Stücke auf
    for (int i = 0; i < 3; ++i) {
        const auto& foodItems = sim->GetFoodItems();
        if (!foodItems.empty()) {
            testCell->SetPosition(foodItems[0].x, foodItems[0].y);
            testCell->SetVelocity(0.0, 0.0);

            sim->SetState(CellularAutomatonSimulation::State::RUNNING);
            sim->Update(0.1);
        }
    }

    // Score sollte 3 sein
    REQUIRE(testCell->GetScore() == 3);

    // Futter-Anzahl sollte immer noch 3 sein
    REQUIRE(sim->GetFoodCount() == 3);
}

TEST_CASE("Food collection: new food spawned after collection") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    sim->SetFoodCount(1);

    auto testCell = sim->GetCellAt(0);
    const auto& foodItems1 = sim->GetFoodItems();

    double originalFoodX = 0.0;
    double originalFoodY = 0.0;

    if (!foodItems1.empty()) {
        originalFoodX = foodItems1[0].x;
        originalFoodY = foodItems1[0].y;

        // Sammle auf
        testCell->SetPosition(originalFoodX, originalFoodY);
        testCell->SetVelocity(0.0, 0.0);

        sim->SetState(CellularAutomatonSimulation::State::RUNNING);
        sim->Update(0.1);
    }

    const auto& foodItems2 = sim->GetFoodItems();

    // Neues Futter sollte spawn
    REQUIRE(sim->GetFoodCount() == 1);

    // Neues Futter sollte an einer anderen Position sein (mit hoher Wahrscheinlichkeit)
    // (Könnte theoretisch die gleiche Position haben, aber sehr unwahrscheinlich)
    if (!foodItems2.empty()) {
        double newFoodX = foodItems2[0].x;
        double newFoodY = foodItems2[0].y;

        // Nur prüfen, dass neue Futter existiert
        REQUIRE(newFoodX >= 20.0);
        REQUIRE(newFoodX <= 580.0);
        REQUIRE(newFoodY >= 20.0);
        REQUIRE(newFoodY <= 380.0);
    }
}

TEST_CASE("Food collection: does not occur when simulation stopped") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    sim->SetFoodCount(5);

    auto testCell = sim->GetCellAt(0);
    const auto& foodItems = sim->GetFoodItems();

    if (!foodItems.empty()) {
        testCell->SetPosition(foodItems[0].x, foodItems[0].y);
        testCell->SetVelocity(0.0, 0.0);
    }

    std::size_t initialScore = testCell->GetScore();

    // Simulation ist STOPPED
    sim->SetState(CellularAutomatonSimulation::State::STOPPED);
    sim->Update(0.1);

    // Score sollte nicht erhöht sein
    REQUIRE(testCell->GetScore() == initialScore);
}

// ============================================================================
// TEST CATEGORY: COMBINED FEATURES
// ============================================================================

TEST_CASE("Combined: food collection and wrapping together") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(1);
    sim->SetFoodCount(1);

    auto testCell = sim->GetCellAt(0);
    const auto& foodItems = sim->GetFoodItems();

    if (!foodItems.empty()) {
        // Platziere Zelle nahe der Kante und nah beim Futter
        testCell->SetPosition(5.0, foodItems[0].y);
        testCell->SetVelocity(-50.0, 0.0);  // Bewegung nach links
    }

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);
    sim->Update(0.2);  // Zelle wrapped um nach rechts, könnte Futter sammeln

    // Zelle sollte gew rappt sein
    double x, y;
    testCell->GetPosition(x, y);
    REQUIRE(x > 0.0);
    REQUIRE(x < 600.0);
}

TEST_CASE("Combined: multiple cells with boundary wrapping and food") {
    auto cell = std::make_shared<CellularAutomaton>();
    auto sim = std::make_shared<CellularAutomatonSimulation>(cell);

    sim->SetCellCount(5);
    sim->SetFoodCount(10);

    // Setze unterschiedliche Positionen und Velocities
    for (std::size_t i = 0; i < 5; ++i) {
        auto cell = sim->GetCellAt(i);
        double angle = (2.0 * M_PI * i) / 5.0;
        double speed = 50.0;

        cell->SetPosition(300.0 + 50.0 * std::cos(angle), 200.0 + 50.0 * std::sin(angle));
        cell->SetVelocity(speed * std::cos(angle), speed * std::sin(angle));
    }

    sim->SetState(CellularAutomatonSimulation::State::RUNNING);

    // Mehrere Updates
    for (int step = 0; step < 10; ++step) {
        sim->Update(0.1);
    }

    // Alle Zellen sollten immer noch im Spielfeld sein
    for (std::size_t i = 0; i < 5; ++i) {
        auto cell = sim->GetCellAt(i);
        double x, y;
        cell->GetPosition(x, y);

        REQUIRE(x >= 0.0);
        REQUIRE(x <= 600.0);
        REQUIRE(y >= 0.0);
        REQUIRE(y <= 400.0);
    }

    // Futter-Anzahl sollte sich nicht ändern
    REQUIRE(sim->GetFoodCount() == 10);
}



